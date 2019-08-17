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


#include "String.hpp"


namespace lr {


/// A tool to split a string into tokens.
///
class StringTokenizer
{
public:
    /// Create a new tokenizer with the given string.
    ///
    /// @param str The string to process.
    /// @param separator The token separator.
    ///
    explicit StringTokenizer(const String &str, char separator);

public:
    /// Check if there is a next token.
    ///
    /// @return `true` if there is a next token.
    ///
    bool hasNextToken() const;

    /// Get the next token.
    ///
    /// The string may be empty if there are multiple subsequent separators.
    ///
    /// @return The string with the next token.
    ///
    String getNextToken();

    /// Get the current tail of the string from the current position to the end.
    ///
    /// @return The string with the tail of the string.
    ///
    String getTail() const;

private:
    const String _string; ///< The string to process.
    const char _separator; ///< The separator.
    String::Size _currentOffset; ///< The current position.
};


}

