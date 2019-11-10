#pragma once
//
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


#undef min
#undef max

#include <cstdint>
#include <type_traits>
#include <limits>


/// @namespace lr::IntegerMath
///
/// Common very simple integer mathematics for embedded systems, which are not
/// provided by the standard libraries in this simple form.
///
/// Defined in this header to make the code simpler to read and provide
/// better examples how to deal with this simple problems.
///

namespace lr {
namespace IntegerMath {


/// Ring addition to a value.
///
/// A specialized addition to keep a position like value in a defined range.
/// The range is defined by `size` which limits the index position.
///
/// The `value` will always be >= 0 and < `size`.
///
/// The `value` is incremented by `count` but kept in the range of `size`.
/// If the value exceeds the range defined by `max` it will overflow and
/// start from zero.
///
/// If `value` is out of the defined range, the call if this function is
/// not defined.
///
/// Example:
/// ```
/// uint16_t x = 5;
/// ringIncrement<uint16_t>(x, 6, 10);
/// ```
/// After this call, `x` will be `1`.
///
/// @param value The value to modify. Must be >= 0 and < `size`.
/// @param delta The number to increment `value` by. Must be >= 0 && <= `max`!
/// @param size The size of the `value` range. If `size` is `10`,
///    `value` may be in the range of `0` to `9`.
///
template<typename Type>
inline void ringIncrement(Type &value, Type delta, Type size) noexcept
{
    static_assert(std::is_integral<Type>::value, "Must be int type.");
    static_assert(std::is_unsigned<Type>::value, "Must be unsigned type.");
    if (delta != 0 && delta != size) {
        const auto rest = size - value;
        if (delta == rest) {
            value = 0;
        } else if (delta < rest) {
            value += delta;
        } else {
            value = (delta - rest);
        }
    }
}


/// Add to a range limited value and return excess.
///
/// A specialized function to keep a element count in a given range.
/// `value` will always be in the range of >= 0 and <= `size`. So the
/// maximum of `value` is equal to `size`, which will be the maximum
/// count of elements.
///
/// If adding `delta` will exceed the maximum, this function will return
/// the exceeding number of elements. If adding elements will stay in the
/// range defined by `size`, this function will return zero.
///
/// @param value The value to modify. Must be >= 0 and <= `size`.
/// @param delta The number add to `value`. Must be <= `size`.
/// @param size The maximum size for the value.
/// @return The exceeding number of elements.
///
template<typename Type>
inline Type addWithOverflow(Type &value, Type delta, Type size) noexcept
{
    static_assert(std::is_integral<Type>::value, "Must be int type.");
    static_assert(std::is_unsigned<Type>::value, "Must be unsigned type.");
    const auto rest = size - value;
    if (delta > rest) {
        value = size;
        return delta - rest;
    } else {
        value += delta;
        return 0;
    }
}


/// Reimplementation of the `min` function.
///
template<typename Type>
constexpr inline Type min(Type a, Type b) noexcept
{
    return (a <= b) ? a : b;
}


/// Reimplementation of the `max` function.
///
template<typename Type>
constexpr inline Type max(Type a, Type b) noexcept
{
    return (a >= b) ? a : b;
}


/// Add and check for overflow.
///
/// `c = a + b`
///
/// @param a Fist value.
/// @param b Second value.
/// @param c Pointer to result.
/// @return `true` if there was an overflow.
///
template<typename IntType>
inline bool addCheckOverflow(IntType a, IntType b, IntType *c)
{
#if !defined(UNITTEST) && (defined(__GNUC__) && __GNUC__ >= 5)
    return __builtin_add_overflow(a, b, c);
#elif !defined(UNITTEST) && (defined(__clang__) && __clang__ >= 4)
    return __builtin_add_overflow(a, b, c);
#else
    const auto result = static_cast<IntType>(a + b);
    *c = result;
    if (std::numeric_limits<IntType>::is_signed) {
        // Two values with different signs always add without overflow, so focus on values with the same sign.
        if ((a < 0) == (b < 0)) {
            return ((a < 0 && result > a) || (a >= 0 && result < a));
        } else {
            return false;
        }
    } else {
        return result < a;
    }
#endif
}


#if !defined(UNITTEST) && ((defined(__GNUC__) && __GNUC__ >= 5) || (defined(__clang__) && __clang__ >= 4))
/// Multiply and check for overflow.
///
/// `c = a * b`
///
/// @param a Fist factor.
/// @param b Second factor.
/// @param c Pointer to result.
/// @return `true` if there was an overflow.
///
template<typename IntType>
constexpr inline bool multiplyCheckOverflow(IntType a, IntType b, IntType *c)
{
    return __builtin_mul_overflow(a, b, c);
}
#else
/// Multiply and check for overflow.
///
/// `c = a * b`
///
/// @param a Fist factor.
/// @param b Second factor.
/// @param c Pointer to result.
/// @return `true` if there was an overflow.
///
template<typename IntType, typename LargeType>
inline bool _multiplyCheckOverflow(const IntType a, const IntType b, IntType* const c)
{
    // A manual implementation which works up to 32bit, but may not produce perfect results.
    const LargeType result = static_cast<LargeType>(a) * static_cast<LargeType>(b);
    *c = static_cast<IntType>(result);
    return (result < std::numeric_limits<IntType>::min()) || (result > std::numeric_limits<IntType>::max());
}

/// @copydoc _addCheckOverflow
inline bool multiplyCheckOverflow(int8_t a, int8_t b, int8_t *c)
{
    return _multiplyCheckOverflow<int8_t, int16_t>(a, b, c);
}

/// @copydoc _addCheckOverflow
inline bool multiplyCheckOverflow(uint8_t a, uint8_t b, uint8_t *c)
{
    return _multiplyCheckOverflow<uint8_t, uint16_t>(a, b, c);
}

/// @copydoc _addCheckOverflow
inline bool multiplyCheckOverflow(int16_t a, int16_t b, int16_t *c)
{
    return _multiplyCheckOverflow<int16_t, int32_t>(a, b, c);
}

/// @copydoc _addCheckOverflow
inline bool multiplyCheckOverflow(uint16_t a, uint16_t b, uint16_t *c)
{
    return _multiplyCheckOverflow<uint16_t, uint32_t>(a, b, c);
}

/// @copydoc _addCheckOverflow
inline bool multiplyCheckOverflow(int32_t a, int32_t b, int32_t *c)
{
    return _multiplyCheckOverflow<int32_t, int64_t>(a, b, c);
}

/// @copydoc _addCheckOverflow
inline bool multiplyCheckOverflow(uint32_t a, uint32_t b, uint32_t *c)
{
    return _multiplyCheckOverflow<uint32_t, uint64_t>(a, b, c);
}
#endif


}
}
