#pragma once
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


#include "SerialLine.hpp"
#include "StringWriter.hpp"


namespace lr {


/// An interface to write strings to a serial line.
///
/// For an optimal performance, the serial line needs to have a buffer which is large enough to hold two or more lines
/// of text data which is written to the serial line. By default, any text which does not fit into the current
/// serial line buffer is silently discarded.
///
class SerialLineStringWriter : public StringWriter
{
public:
    /// Create a new serial line text writer.
    ///
    explicit SerialLineStringWriter(SerialLine *serialLine);

public: // Implement StringWriter
    Status write(const String &text) override;
    Status write(char c, uint8_t count) override;
    Status write(const char *text) override;
    Status writeLine(const String &text) override;
    Status writeLine(const char *text) override;
    Status writeLine() override;

private:
    SerialLine *_serialLine; ///< The used serial line.
};


}

