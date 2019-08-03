//
// Serial Line String Writer
// ---------------------------------------------------------------------------
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
#include "SerialLineStringWriter.hpp"


#include "SerialLine.hpp"


namespace lr {


SerialLineStringWriter::SerialLineStringWriter(SerialLine *serialLine)
    : _serialLine(serialLine)
{
}


StringWriter::Status SerialLineStringWriter::write(const String &text)
{
    // Just write as much data as possible.
    const auto result = _serialLine->send(
        reinterpret_cast<const uint8_t*>(text.getData()),
        text.getLength());
    if (result == SerialLine::Status::Error) {
        return Status::Error;
    }
    return Status::Success;
}


StringWriter::Status SerialLineStringWriter::write(char c, uint8_t count)
{
    for (uint8_t i = 0; i < count; ++i) {
        const auto result = _serialLine->send(static_cast<uint8_t>(c));
        if (result == SerialLine::Status::Error) {
            return Status::Error;
        }
    }
    return Status::Success;
}


StringWriter::Status SerialLineStringWriter::write(const char *text)
{
    uint8_t textSize = 0;
    for (uint8_t i = 0; i < 0xff; ++i) {
        if (text[i] == '\0') {
            textSize = i;
            break;
        }
    }
    if (textSize > 0) {
        const auto result = _serialLine->send(reinterpret_cast<const uint8_t*>(text), textSize);
        if (result == SerialLine::Status::Error) {
            return Status::Error;
        }
    }
    return Status::Success;
}


StringWriter::Status SerialLineStringWriter::writeLine(const String &text)
{
    if (hasError(write(text))) {
        return Status::Error;
    }
    if (hasError(writeLine())) {
        return Status::Error;
    }
    return Status::Success;
}


StringWriter::Status SerialLineStringWriter::writeLine(const char *text)
{
    if (hasError(write(text))) {
        return Status::Error;
    }
    if (hasError(writeLine())) {
        return Status::Error;
    }
    return Status::Success;
}


StringWriter::Status SerialLineStringWriter::writeLine()
{
    auto result = _serialLine->send('\r');
    if (result == SerialLine::Status::Error) {
        return Status::Error;
    }
    result = _serialLine->send('\n');
    if (result == SerialLine::Status::Error) {
        return Status::Error;
    }
    return Status::Success;
}


}

