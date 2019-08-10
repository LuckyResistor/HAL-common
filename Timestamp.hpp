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


#include "DateTime.hpp"


namespace lr {


/// A 32 bit precision timestamp.
///
/// Use this class to store date/time values from from 2000-01-01 00:00:00
/// up to 2136-02-07 06:28:15 in a compact 32bit value.
///
/// This class can also be used to do limited and slow time calculations in 32bit precision.
///
/// The timestamp 0 equals 2000-01-01 00:00:00.
///
class Timestamp32
{
public:
    /// Create a zero timestamp which equals 2000-01-01 00:00:00
    ///
    Timestamp32();

    /// Create a timestamp from the given `DateTime` object.
    ///
    /// This is a slow operation, optimized for memory.
    ///
    /// It only works with dates from 2000-01-01 00:00:00 up to 2136-02-07 06:28:15 due the 32bit precision of the
    /// calculation used.
    ///
    explicit Timestamp32(const lr::DateTime &dateTime);

public:
    /// Add seconds to this timestamp.
    ///
    void addSeconds(int32_t seconds);

    /// Add days to this timestamp.
    ///
    void addDays(int32_t days);

    /// Get the number of seconds to the other timestamp.
    ///
    int32_t secondsTo(const Timestamp32 &other) const;

    /// Get the actual value of this timestamp.
    ///
    /// This is the number of seconds since 2000-01-01 00:00:00
    ///
    inline int32_t getValue() const
    {
        return _value;
    }

    /// Convert this timestamp to a unix timestamp.
    ///
    inline uint32_t toUnixTimestamp() const
    {
        return _value + 0x386D4380u;
    }

    /// Convert this timestamp to a `DateTime` object.
    ///
    DateTime toDateTime() const;

private:
    uint32_t _value; ///< The value of the timestamp.
};


/// A 64 bit precision timestamp.
///
/// This timestamp will work with the full range of dates from 2000-01-01 up to 9999-12-31.
///
/// This class can also be used to do limited and slow time calculations in 64bit precision.
///
/// The timestamp 0 equals 2000-01-01 00:00:00.
///
class Timestamp64
{
public:
    /// Create a zero timestamp which equals 2000-01-01 00:00:00
    ///
    Timestamp64();

    /// Create a timestamp from the given `DateTime` object.
    ///
    /// This is a slow operation, optimized for memory.
    ///
    /// It only works with dates from 2000-01-01 00:00:00 up to 2136-02-07 06:28:15 due the 32bit precision of the
    /// calculation used.
    ///
    explicit Timestamp64(const lr::DateTime &dateTime);

public:
    /// Add seconds to this timestamp.
    ///
    void addSeconds(int64_t seconds);

    /// Add days to this timestamp.
    ///
    void addDays(int64_t days);

    /// Get the number of seconds to the other timestamp.
    ///
    int64_t secondsTo(const Timestamp64 &other) const;

    /// Get the actual value of this timestamp.
    ///
    /// This is the number of seconds since 2000-01-01 00:00:00
    ///
    inline int64_t getValue() const
    {
        return _value;
    }

    /// Convert this timestamp to a unix timestamp.
    ///
    inline uint64_t toUnixTimestamp() const
    {
        return _value + 0x386D4380ull;
    }

    /// Convert this timestamp to a `DateTime` object.
    ///
    DateTime toDateTime() const;

private:
    uint64_t _value; ///< The value of the timestamp.
};


}

