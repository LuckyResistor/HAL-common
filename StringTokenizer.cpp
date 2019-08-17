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
#include "StringTokenizer.hpp"


namespace lr {


StringTokenizer::StringTokenizer(const String &str, char separator)
    : _string(str), _separator(separator), _currentOffset(0)
{
}


String StringTokenizer::getNextToken()
{
    while (hasNextToken()) {
        const auto index = _string.getFirstIndex(_separator, _currentOffset);
        if (index == String::notFound) {
            const auto token = _string.getTail(_currentOffset);
            _currentOffset = _string.getLength();
            return token;
        } else if (index > _currentOffset) {
            const auto token = _string.getSlice(_currentOffset, index-_currentOffset);
            _currentOffset = index + 1;
            return token;
        } else {
            _currentOffset += 1;
        }
    }
    return String();
}


bool StringTokenizer::hasNextToken() const
{
    return _currentOffset < _string.getLength();
}


String StringTokenizer::getTail() const
{
    if (!hasNextToken()) {
        return String();
    }
    return _string.getTail(_currentOffset);
}


}

