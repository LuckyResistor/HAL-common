//
// (c)2019 by Lucky Resistor. See LICENSE for details.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
#include "SerialLineShell.hpp"


#include <cstring>
#include <cstdlib>


namespace lr {


namespace {
const char *cCSI = "\x1b[";
}


SerialLineShell::SerialLineShell(lr::SerialLine *serialLine, const uint8_t lineBufferSize)
:
    _writer(serialLine),
    _lineBufferMaximumSize(lineBufferSize),
    _lineLength(0),
    _lineCursorPosition(0),
    _lineBuffer(nullptr),
    _inputMode(InputMode::LineEdit),
    _prompt(),
    _lineFn(nullptr),
    _lineExpansionFn(nullptr),
    _keyFn(nullptr),
    _sendPrompt(true),
    _escapeDeadline(10_ms),
    _escapeState(EscapeState::None)
{
    _lineBuffer = static_cast<char*>(malloc(lineBufferSize+1));
    std::memset(_lineBuffer, 0, lineBufferSize+1);
}


SerialLineShell::~SerialLineShell()
{
    free(_lineBuffer);
}


void SerialLineShell::pollEvent()
{
    if (_sendPrompt && isPromptMode()) {
        write(_prompt);
        _sendPrompt = false;
    }
    if (_escapeState != EscapeState::None && _escapeDeadline.hasTimeout()) {
        _escapeState = EscapeState::None;
        handleInput(static_cast<char>(Key::Escape));
    }
    uint8_t readByte;
    while (isSuccessful(_writer.serialLine()->receive(readByte))) {
        const Key key = handleEscape(readByte);
        if (key != Key::None) {
            readByte = static_cast<char>(key);
        }
        if (_escapeState == EscapeState::None) {
            handleInput(static_cast<char>(readByte));
        }
    }
}


bool SerialLineShell::isPromptMode() const
{
    return _inputMode == InputMode::LineEdit || _inputMode == InputMode::HiddenEdit;
}


SerialLineShell::Key SerialLineShell::handleEscape(char c)
{
    Key key = Key::None;
    switch (_escapeState) {
    case EscapeState::None:
        switch (c) {
        case static_cast<char>(Key::Escape):
            _escapeState = EscapeState::Escape;
            _escapeDeadline.restart(10_ms);
            break;
        case 0x7f: // Convert DEL to backspace.
            key = Key::Backspace;
            break;
        case 0x0a: // Convert NL to carriage return.
            key = Key::Return;
            break;
        default:
            break;
        }
        break;
    case EscapeState::Escape:
        if (c == '[') {
            _escapeState = EscapeState::ControlSequence;
        } else {
            _escapeState = EscapeState::None;
        }
        break;
    case EscapeState::ControlSequence:
        // Just wait for the end character, ignore any parameters.
        if (c >= 0x40) {
            switch (c) {
            case 'A': key = Key::CursorUp; break;
            case 'B': key = Key::CursorDown; break;
            case 'C': key = Key::CursorForward; break;
            case 'D': key = Key::CursorBack; break;
            default: break;
            }
            _escapeState = EscapeState::None;
        }
        break;
    }
    return key;
}


void SerialLineShell::handleInput(char c)
{
    switch (_inputMode) {
    case InputMode::Keys:
        if (_keyFn != nullptr) {
            _keyFn(c);
        }
        break;
    case InputMode::LineEdit:
    case InputMode::HiddenEdit:
        handleLineEdit(c);
        break;
    default:
        break;
    }
}


void SerialLineShell::insertCharacter(char c)
{
    if (_lineCursorPosition < _lineLength) {
        const auto source = _lineBuffer + _lineCursorPosition;
        const auto destination = source + 1;
        const auto count = _lineLength - _lineCursorPosition;
        std::memmove(destination, source, count);
    }
    _lineBuffer[_lineCursorPosition] = c;
    _lineCursorPosition += 1;
    _lineLength += 1;
}


void SerialLineShell::deleteCharacter()
{
    if (_lineCursorPosition > 0 && _lineLength > 0) {
        const auto source = _lineBuffer + _lineCursorPosition;
        const auto destination = source - 1;
        const auto count = _lineLength - _lineCursorPosition;
        std::memmove(destination, source, count);
        _lineCursorPosition -= 1;
        _lineLength -= 1;
    }
}


void SerialLineShell::handleLineEdit(char c)
{
    // handle regular characters.
    if (c >= 0x20 && c <= 0x7e) {
        if (_lineLength == _lineBufferMaximumSize) {
            return; // Ignore any additional input if the line buffer is full.
        }
        insertCharacter(c);
        if (_inputMode == InputMode::LineEdit) {
            write(c, 1);
            if (_lineCursorPosition < _lineLength) {
                updateEndOfLine();
            }
        } else {
            write('*', 1);
        }
    } else {
        switch (static_cast<Key>(c)) {
        case Key::CursorForward:
            if (_inputMode != InputMode::HiddenEdit && _lineCursorPosition < _lineLength) {
                _lineCursorPosition += 1;
                writeCursorForward();
            } else {
                writeBell();
            }
            break;
        case Key::CursorBack:
            if (_inputMode != InputMode::HiddenEdit && _lineCursorPosition > 0) {
                _lineCursorPosition -= 1;
                writeCursorBack();
            } else {
                writeBell();
            }
            break;
        case Key::Backspace:
            if (_lineLength > 0 && _lineCursorPosition > 0) {
                deleteCharacter();
                write('\b', 1);
                updateEndOfLine();
            } else {
                writeBell();
            }
            break;
        case Key::Return:
            writeLine();
            if (_lineFn != nullptr && _lineLength > 0) {
                const String line(_lineBuffer, _lineLength);
                _lineFn(line);
            }
            _lineLength = 0;
            _lineCursorPosition = 0;
            _sendPrompt = true;
            break;
        case Key::Tab:
        case Key::Escape:
            if (_lineExpansionFn != nullptr) {
                uint8_t cursorPosition = _lineCursorPosition;
                String line(_lineBuffer, _lineLength);
                const auto lineExpansion = _lineExpansionFn(line, cursorPosition);
                if (lineExpansion != LineExpansion::Failed) {
                    _lineLength = line.getLength();
                    std::memcpy(_lineBuffer, line.getData(), _lineLength);
                    _lineCursorPosition = cursorPosition;
                    if (_lineCursorPosition > _lineLength) {
                        _lineCursorPosition = _lineLength;
                    }
                }
                switch (lineExpansion) {
                case LineExpansion::Failed:
                    writeBell();
                    break;
                case LineExpansion::Inline:
                    writeEraseInLine();
                    writeCursorHorizontalAbsolute(0);
                case LineExpansion::NewPrompt:
                    write(_prompt);
                    write(line);
                    writeCursorHorizontalAbsolute(_lineCursorPosition + _prompt.getLength());
                    break;
                }
            }
            break;
        default:
            writeBell();
            break;
        }
    }
}


void SerialLineShell::updateEndOfLine()
{
    const uint8_t endCount = _lineLength - _lineCursorPosition;
    writeSaveCursorPosition();
    if (endCount > 0) {
        const auto endData = reinterpret_cast<uint8_t*>(_lineBuffer + _lineCursorPosition);
        _writer.serialLine()->send(endData, endCount);
    }
    write(' ', 1);
    writeRestoreCursorPosition();
}


void SerialLineShell::setInputMode(SerialLineShell::InputMode inputMode)
{
    if (_inputMode != inputMode) {
        _inputMode = inputMode;
        if (isPromptMode()) {
            _sendPrompt = true;
        }
    }
}


void SerialLineShell::setPrompt(const String &prompt)
{
    _prompt = prompt;
}


void SerialLineShell::setLineFn(LineFn lineFn)
{
    _lineFn = lineFn;
}


void SerialLineShell::setLineExpansionFn(LineExpansionFn lineExpansionFn)
{
    _lineExpansionFn = lineExpansionFn;
}


void SerialLineShell::setKeysFn(KeysFn keysFn)
{
    _keyFn = keysFn;
}


void SerialLineShell::writeCSI(char command)
{
    String text(cCSI);
    text.append(command);
    write(text);
}


void SerialLineShell::writeCSI(char command, uint8_t x)
{
    String text(cCSI);
    if (x > 0) {
        text.appendNumber(x);
    }
    text.append(command);
    write(text);
}


void SerialLineShell::writeCSI(char command, uint8_t x, uint8_t y)
{
    String text(cCSI);
    if (x > 0) {
        text.appendNumber(x);
    }
    text.append(';');
    if (y > 0) {
        text.appendNumber(y);
    }
    text.append(command);
    write(text);
}


void SerialLineShell::writeBell()
{
    write('\a', 1);
}


void SerialLineShell::writeCursorUp(uint8_t count)
{
    writeCSI('A', count);
}


void SerialLineShell::writeCursorDown(uint8_t count)
{
    writeCSI('B', count);
}


void SerialLineShell::writeCursorForward(uint8_t count)
{
    writeCSI('C', count);
}


void SerialLineShell::writeCursorBack(uint8_t count)
{
    writeCSI('D', count);
}


void SerialLineShell::writeCursorHorizontalAbsolute(uint8_t column)
{
    writeCSI('G', column+1);
}


void SerialLineShell::writeCursorPosition(uint8_t row, uint8_t column)
{
    writeCSI('H', row+1, column+1);
}


void SerialLineShell::writeEraseInDisplay()
{
    writeCSI('J', 2);
}


void SerialLineShell::writeEraseInLine()
{
    writeCSI('K', 2);
}


void SerialLineShell::writeSaveCursorPosition()
{
    writeCSI('s');
}


void SerialLineShell::writeRestoreCursorPosition()
{
    writeCSI('u');
}


}
