#pragma once
// Date/Time
// ---------------------------------------------------------------------------
// (c)2018 by Lucky Resistor. See LICENSE for details.
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


#include <cstdint>


/// @namespace lr::BCD
/// This is a helper for BCD conversion.
///

namespace lr {
namespace BCD {

    
/// @internal
/// Function to convert BCD format into binary format.
///
constexpr inline uint8_t convertBcdToBin(const uint8_t bcd)
{
    return (bcd&0xf)+((bcd>>4)*10);
}


/// @internal
/// Function to convert binary to BCD format.
///
constexpr inline uint8_t convertBinToBcd(const uint8_t bin)
{
    return (bin%10)+((bin/10)<<4);
}


}
}

