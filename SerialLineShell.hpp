#pragma once
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


#include "StringWriter.hpp"
#include "SerialLineStringWriter.hpp"
#include "Timer.hpp"

#include <functional>


namespace lr {


/// A serial line shell implementation.
///
/// This class implements the base for a shell based serial line interface. It automatically
/// reads any input from the serial line into a line buffer. Each read character is echoed back to the
/// console to provide the required feedback for the user. You can enable a prompt, which is sent to the
/// console to indicate user input is requested.
///
/// To use this class, you have to call the `pollEvent()` from your event loop. To respond to user input,
/// you can poll the `readLine()` method or register a callback.
///
class SerialLineShell : public StringWriter
{
public:
    /// The input mode for the console.
    ///
    enum class InputMode : uint8_t {
        LineEdit, ///< The user can edit the current line.
        HiddenEdit, ///< The user can type, but the input is replaced by the '*' character.
        Keys, ///< Each key press creates a callback, but there is no echo.
        Disabled, ///< All input is discarded.
    };

    /// Special keys.
    ///
    enum class Key : char {
        None = 0x00, ///< No valid key.
        CursorUp = 0x01, ///< Cursor Up.
        CursorDown = 0x02, ///< Cursor Down.
        CursorForward = 0x03, ///< Cursor right.
        CursorBack = 0x04, ///< Cursor left.
        Backspace = 0x08, ///< Delete or backspace.
        Tab = 0x09, ///< The tab key.
        Return = 0x0d, ///< If the user presses enter.
        Escape = 0x1b, ///< Escape alone is only received after a delay.
    };

    /// The line expansion result.
    ///
    enum class LineExpansion {
        /// Do nothing, line expansion is not possible.
        Failed,
        /// There was output on the console, rebuild the prompt. Assuming the cursor is in a new empty line.
        NewPrompt,
        /// The line was expanded, redraw the current line and move the cursor.
        Inline,
    };

    /// Callback for a complete line.
    ///
    /// @param 1 The complete line.
    ///
    using LineFn = void(*)(const String&);

    /// Callback for a single key in `Keys` mode.
    ///
    /// Known escape sequences are converted to the values in the `Key` enumeration.
    /// So compare all values below 0x20 with the enumerations in `Key`.
    ///
    /// @param 1 The pressed key.
    ///
    using KeysFn = void(*)(char);

    /// Callback for line expansion.
    ///
    /// @param 1 The current line as string for modifications.
    /// @param 2 The current cursor position. Can be modified.
    ///
    using LineExpansionFn = LineExpansion(*)(String&, uint8_t&);

public:
    /// Create a new serial line console.
    ///
    /// @param serialLine The serial line to use for communication.
    /// @param lineBufferSize The size of the line buffer. This limits the maximum size of a line.
    ///
    explicit SerialLineShell(SerialLine *serialLine, uint8_t lineBufferSize = 80);

    /// dtor
    ///
    ~SerialLineShell();

public:
    /// The poll event method.
    ///
    /// Call this from your event loop. All callbacks are called from this method.
    ///
    void pollEvent();

    /// Change the current input mode.
    ///
    void setInputMode(InputMode inputMode);

    /// Change the input prompt.
    ///
    void setPrompt(const String &prompt);

    /// Set a callback for every new line.
    ///
    /// This callback is called in `LineEdit` and `HiddenEdit` mode, if the user presses the enter key and
    /// the console sends `CR/LF` to this shell. You get the whole line, without the line end as parameter
    /// of the callback function.
    ///
    void setLineFn(LineFn lineFn);

    /// Set a callback for a line expansion.
    ///
    /// If this callback is set, it is called if the user presses the tab key in line edit mode.
    /// You can modify the line and cursor position in the callback to expand the line.
    ///
    void setLineExpansionFn(LineExpansionFn lineExpansionFn);

    /// Set a callback for each key press.
    ///
    /// This callback is called in `Keys` mode if the user presses any key which is sent to the serial line.
    ///
    void setKeysFn(KeysFn keysFn);

    /// Send a bell signal.
    ///
    void writeBell();

    /// Send a cursor up sequence.
    ///
    void writeCursorUp(uint8_t count = 0);

    /// Send a cursor down sequence.
    ///
    void writeCursorDown(uint8_t count = 0);

    /// Send a cursor forward sequence.
    ///
    void writeCursorForward(uint8_t count = 0);

    /// Send a cursor backward sequence.
    ///
    void writeCursorBack(uint8_t count = 0);

    /// Send a cursor horizontal absolute sequence.
    ///
    void writeCursorHorizontalAbsolute(uint8_t column);

    /// Set the cursor position.
    ///
    void writeCursorPosition(uint8_t row, uint8_t column);

    /// Send a erase in display sequence (full)
    ///
    void writeEraseInDisplay();

    /// Send a erase in line sequence (full)
    ///
    void writeEraseInLine();

    /// Send a save cursor position sequence.
    ///
    void writeSaveCursorPosition();

    /// Send a restore cursor position sequence.
    ///
    void writeRestoreCursorPosition();

    /// Send a CSI sequence.
    ///
    void writeCSI(char command);

    /// Send a CSI sequence.
    ///
    void writeCSI(char command, uint8_t x);

    /// Send a CSI sequence.
    ///
    void writeCSI(char command, uint8_t x, uint8_t y);

public: // Implement StringWriter by wrapping the SerialLineStringWriter.
    inline Status write(const String &text) override {
        return _writer.write(text);
    }
    inline Status write(char c, uint8_t count) override {
        return _writer.write(c, count);
    }
    inline Status write(const char *text) override {
        return _writer.write(text);
    }
    inline Status writeLine(const String &text) override {
        return _writer.writeLine(text);
    }
    inline Status writeLine(const char *text) override {
        return _writer.writeLine(text);
    }
    inline Status writeLine() override {
        return _writer.writeLine();
    }

private:
    /// Handle character input.
    ///
    void handleInput(char c);

    /// Handle line edit input.
    ///
    void handleLineEdit(char c);

    /// Insert a character into the line buffer.
    ///
    void insertCharacter(char c);

    /// Delete the character left of the cursor.
    ///
    void deleteCharacter();

    /// Check if a mode has a prompt.
    ///
    bool isPromptMode() const;

    /// Handle escape sequences.
    ///
    Key handleEscape(char c);

    /// Update the end of the line.
    ///
    /// Used after insert/delete characters at the cursor position.
    ///
    void updateEndOfLine();

private:
    /// Escape sequence state.
    ///
    enum class EscapeState {
        None, ///< There is no escape sequence in progress.
        Escape, ///< Received an escape character, waiting for the second character.
        ControlSequence, ///< Waiting for a control sequence
    };

private:
    SerialLineStringWriter _writer; ///< The string writer to use.
    const uint8_t _lineBufferMaximumSize; ///< The size of the line buffer.
    uint8_t _lineLength; ///< The current line length.
    uint8_t _lineCursorPosition; ///< The current cursor position.
    char *_lineBuffer; ///< The line buffer.
    InputMode  _inputMode; ///< The current input mode.
    String _prompt; ///< The current prompt.
    LineFn _lineFn; ///< The callback for each line.
    LineExpansionFn _lineExpansionFn; ///< The function for a line expansion.
    KeysFn _keyFn; ///< The callback for each keypress.
    bool _sendPrompt; ///< If a prompt has to be sent.
    Timer::Deadline _escapeDeadline; ///< A timer wait for an escape sequence.
    EscapeState _escapeState; ///< The escape state.
};


}