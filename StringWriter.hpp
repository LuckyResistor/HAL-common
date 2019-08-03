#pragma once
//
// Serial Line Text Writer
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


#include "StatusTools.hpp"
#include "String.hpp"


namespace lr {


/// The abstract text writer interface.
///
class StringWriter
{
public:
    /// The status for the text writer.
    ///
    using Status = CallStatus;

public:
    /// Write the given text.
    ///
    /// @param text The string to write.
    ///
    virtual Status write(const String &text) = 0;

    /// Write a single character.
    ///
    /// @param c The character to write.
    /// @param count How many characters shall be written.
    ///
    virtual Status write(char c, uint8_t count = 1) = 0;

    /// Write the given string literal.
    ///
    /// @param text The string literal to write.
    ///
    virtual Status write(const char *text) = 0;

    /// Write the given text with a line end.
    ///
    /// @param text The string to write.
    ///
    virtual Status writeLine(const String &text) = 0;

    /// Write the given string literal with a line end.
    ///
    /// @param text The string literal to write.
    ///
    virtual Status writeLine(const char *text) = 0;

    /// Write just a line end.
    ///
    virtual Status writeLine() = 0;
};


}