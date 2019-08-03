#pragma once
//
// Duration abstract interface
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
#include <ratio>
#include <limits>


namespace lr {


/// A simple duration type similar to the standard library.
///
/// This duration is simplified for the usage in embedded code.
/// The type definitions will use unsigned types, so there are
/// only positive durations possible.
///
/// The idea is to allow a syntax like `20_ms` in the code,
/// which can be verified by functions to make sure the correct
/// durations are passed. It does also greatly improve the
/// readability of the code.
///
template <typename Ratio>
class Duration
{
public:
    /// The type for the ticks of this duration.
    ///
    using TickType = uint32_t;

    /// The type for a delta between ticks.
    ///
    using TickDelta = int32_t;

public:
    /// Create a zero duration.
    ///
    constexpr Duration() noexcept : _ticks(0u) {}

    /// Create a new duration.
    ///
    /// @param ticks The number of ticks of this duration.
    ///
    constexpr explicit Duration(TickType ticks) noexcept : _ticks(ticks) {}

public: // Operators
    constexpr inline bool operator==(const Duration &other) const noexcept { return _ticks == other._ticks; }
    constexpr inline bool operator!=(const Duration &other) const noexcept { return _ticks != other._ticks; }
    constexpr inline bool operator>(const Duration &other) const noexcept { return _ticks > other._ticks; }
    constexpr inline bool operator>=(const Duration &other) const noexcept { return _ticks >= other._ticks; }
    constexpr inline bool operator<(const Duration &other) const noexcept { return _ticks < other._ticks; }
    constexpr inline bool operator<=(const Duration &other) const noexcept { return _ticks <= other._ticks; }
    constexpr inline Duration operator+(const Duration &other) const noexcept { return Duration(_ticks+other._ticks); }
    constexpr inline Duration operator-(const Duration &other) const noexcept { return Duration(_ticks-other._ticks); }

public:
    /// Get the number of ticks.
    ///
    constexpr inline TickType ticks() const noexcept { return _ticks; }

    /// Get the tick delta to another duration.
    ///
    /// @return The delta.
    ///
    constexpr inline TickDelta deltaTo(const Duration &other) const noexcept {
        return static_cast<TickDelta>(other._ticks - _ticks);
    }

public: // Conversion functions.
    /// Convert to another unit
    ///
    /// @tparam TargetRatio The target ratio to convert to.
    ///
    template<typename TargetRatio>
    constexpr inline Duration<TargetRatio> converted() const noexcept {
        typedef std::ratio_divide<Ratio, TargetRatio> r;
        return Duration<TargetRatio>(_ticks * r::num / r::den);
    }

    /// Convert to Seconds.
    ///
    constexpr inline Duration<std::ratio<1>> toSeconds() const noexcept {
        return converted<std::ratio<1>>();
    }

    /// Convert to Milliseconds.
    ///
    constexpr inline Duration<std::milli> toMilliseconds() const noexcept {
        return converted<std::milli>();
    }

    /// Convert to Microseconds.
    ///
    constexpr inline Duration<std::micro> toMicroseconds() const noexcept {
        return converted<std::micro>();
    }

    /// Convert to Nanoseconds.
    ///
    constexpr inline Duration<std::nano> toNanoseconds() const noexcept {
        return converted<std::nano>();
    }

    /// Convert from another one.
    ///
    /// @return A new duration with the correctly converted value.
    ///
    template <typename OtherRatio>
    constexpr inline static Duration createFrom(const Duration<OtherRatio> &other) {
        typedef std::ratio_divide<OtherRatio, Ratio> r;
        return Duration(other._ticks * r::num / r::den);
    }

private:
    TickType _ticks; ///< The number of ticks.
};


// Declare all used units.
using Days = Duration<std::ratio<60*60*24>>; ///< Days.
using Hours = Duration<std::ratio<60*60>>; ///< Hours.
using Minutes = Duration<std::ratio<60>>; ///< Minutes.
using Seconds = Duration<std::ratio<1>>; ///< Seconds.
using Milliseconds = Duration<std::milli>; ///< Milliseconds.
using Microseconds = Duration<std::micro>; ///< Microseconds.
using Nanoseconds = Duration<std::nano>; ///< Nanoseconds.


// Declare integer literals for the units.

/// Days.
///
constexpr Days operator "" _days(unsigned long long int ticks) {
    return Days(static_cast<Days::TickType>(ticks));
}

/// Hours.
///
constexpr Hours operator "" _hours(unsigned long long int ticks) {
    return Hours(static_cast<Hours::TickType>(ticks));
}

/// Minutes.
///
constexpr Minutes operator "" _minutes(unsigned long long int ticks) {
    return Minutes(static_cast<Minutes::TickType>(ticks));
}

/// Seconds.
///
constexpr Seconds operator "" _s(unsigned long long int ticks) {
    return Seconds(static_cast<Seconds::TickType>(ticks));
}

/// Milliseconds.
///
constexpr Milliseconds operator "" _ms(unsigned long long int ticks) {
    return Milliseconds(static_cast<Milliseconds::TickType>(ticks));
}

/// Microseconds.
///
constexpr Microseconds operator "" _us(unsigned long long int ticks) {
    return Microseconds(static_cast<Microseconds::TickType>(ticks));
}

/// Nanoseconds.
///
constexpr Nanoseconds operator "" _ns(unsigned long long int ticks) {
    return Nanoseconds(static_cast<Nanoseconds::TickType>(ticks));
}


}


