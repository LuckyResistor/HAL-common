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


#include <cstdint>


namespace lr {
namespace bit_tools {


/// Return a single bit at the given bit index.
///
/// @tparam IntType The integer type for the result.
/// @param bitIndex The bit index.
/// @return A integer with the selected bit set.
///
template<typename IntType>
constexpr IntType oneBit(uint8_t bitIndex)
{
    return static_cast<IntType>(1) << bitIndex;
}

/// Return a single bit for an unsigned 8bit integer.
///
/// @param bitIndex The bit index.
/// @return A integer with the selected bit set.
///
constexpr uint8_t oneBit8(uint8_t bitIndex)
{
    return oneBit<uint8_t>(bitIndex);
}

/// Return a single bit for an unsigned 16bit integer.
///
/// @param bitIndex The bit index.
/// @return A integer with the selected bit set.
///
constexpr uint16_t oneBit16(uint8_t bitIndex)
{
    return oneBit<uint16_t>(bitIndex);
}

/// Return a single bit for an unsigned 32bit integer.
///
/// @param bitIndex The bit index.
/// @return A integer with the selected bit set.
///
constexpr uint32_t oneBit32(uint8_t bitIndex)
{
    return oneBit<uint32_t>(bitIndex);
}

}


// Import all bit tools into the lr namespace.
// In case of a name conflict, you can access the tool function using the `bit_tools::` namespace.
using namespace bit_tools;


}

