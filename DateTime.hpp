#pragma once
// Date/Time
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


#include "String.hpp"

#include <cstdint>


namespace lr {


/// A rock solid date/time class for a Gregorian calendar.
///
/// This class was specifically made for the embedded environment. It works
/// well with 8bit and 32bit micro controllers.
///
/// Leap years are correctly handled. The class is fully tested with all
/// possible values up to 2136-02-07 06:28:15.
///
/// Note: For the formatting I follow the ISO 8601 standard with some common
/// derivations for better readability. This whole class has ISO standards
/// in mind, therefore I use everywhere the logical ordering of dates:
/// year, month, day, hour, minute, second or for the date day, month and year.
///
class DateTime
{
public:
    /// The string format to convert a date time.
    ///
    enum class Format : uint8_t {
        ISO, /// yyyy-MM-ddThh:mm:ss
        Long, /// yyyy-MM-dd hh:mm:ss
        ISODate, /// yyyy-MM-dd
        ISOBasicDate, /// yyyyMMdd
        ISOTime, /// hh:mm:ss
        ISOBasicTime, /// hhmmss
        ShortDate, /// dd.MM.
        ShortTime, /// hh:mm
    };

public:
    /// Create the first possible date/time which is 2000-01-01 00:00:00.
    ///
    /// You can check for this special date/time using the isFirst()
    /// method.
    ///
    DateTime();

    /// Create a new date/time with the given values.
    /// All values are constrained to valid values, day of week is calculated.
    ///
    /// @param year The year from 2000-9999.
    /// @param month The month from 1=January to 12=December.
    /// @param day The day of the month from 1 to 31.
    /// @param hour The hour from 0 to 23.
    /// @param minute The minute from 0 to 59.
    /// @param second The second from 0 to 59.
    ///
    DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour = 0, uint8_t minute = 0, uint8_t second = 0);

    /// Copy
    ///
    DateTime(const DateTime &other);

    /// dtor
    ///
    ~DateTime() = default;

    /// Assignment
    ///
    DateTime& operator=(const DateTime &other);

public:
    /// Compare this date/time to another value
    ///
    bool operator==(const DateTime &other) const;

    /// Compare this date/time to another value
    ///
    bool operator!=(const DateTime &other) const;

    /// Compare this date/time to another value
    ///
    bool operator<(const DateTime &other) const;

    /// Compare this date/time to another value
    ///
    bool operator<=(const DateTime &other) const;

    /// Compare this date/time to another value
    ///
    bool operator>(const DateTime &other) const;

    /// Compare this date/time to another value
    ///
    bool operator>=(const DateTime &other) const;

public:
    /// Set the date.
    ///
    /// The assigned values are automatically limited to valid ranges.
    ///
    /// @param year The year from 2000-9999.
    /// @param month The month from 1-12.
    /// @param day The day from 1-31.
    ///
    void setDate(uint16_t year, uint16_t month, uint16_t day);

    /// Set the time.
    ///
    /// The assigned values are automatically limited to valid ranges.
    ///
    /// @param hour The hour 0-23.
    /// @param minute The minute 0-59.
    /// @param second The second 0-59.
    ///
    void setTime(uint8_t hour, uint8_t minute, uint8_t second);

    /// Add one second to this date/time.
    ///
    /// This function can be used to implement a running RTC in the
    /// background. The function only requires minimal time, except
    /// if the day changes, because of the leap year calculation.
    ///
    void addOneSecond();

    /// Get the year.
    /// Value from 2000-9999.
    ///
    uint16_t getYear() const;

    /// Get the month.
    /// Value from 1=January to 12=December.
    ///
    uint8_t getMonth() const;

    /// Get the day.
    /// Value from 1 to 31.
    ///
    uint8_t getDay() const;

    /// Get the day of the week.
    /// Value from 0=Sunday, 1=Monday to 6=Saturday.
    ///
    uint8_t getDayOfWeek() const;

    /// Get the hour.
    /// Value from 0-23.
    ///
    uint8_t getHour() const;

    /// Get the minute.
    /// Value from 0-59.
    ///
    uint8_t getMinute() const;

    /// Get the second.
    /// Value from 0-59.
    ///
    uint8_t getSecond() const;

    /// Check if this is the first possible date/time 2000-01-01 00:00:00.
    ///
    bool isFirst() const;

    /// Convert this date/time into a string using the given format.
    ///
    String toString(Format format) const;

public:
    /// Create a new completely unchecked date time object from the given values.
    ///
    /// You have to make sure all values are in the correct ranges.
    /// This method is provided to create `DateTime` objects from RTC clocks and such without overhead.
    ///
    /// @param year The year from 2000-9999.
    /// @param month The month from 1=January to 12=December.
    /// @param day The day of the month from 1 to 31.
    /// @param hour The hour from 0 to 23.
    /// @param minute The minute from 0 to 59.
    /// @param second The second from 0 to 59.
    /// @param dayOfWeek The day of week 0=Sunday, 1=Monday to 6=Saturday.
    ///
    static DateTime fromUncheckedValues(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, uint8_t dayOfWeek);

private:
    /// Create a new unchecked date/time object.
    ///
    /// @param year The year from 2000-9999.
    /// @param month The month from 1=January to 12=December.
    /// @param day The day of the month from 1 to 31.
    /// @param hour The hour from 0 to 23.
    /// @param minute The minute from 0 to 59.
    /// @param second The second from 0 to 59.
    /// @param dayOfWeek The day of week 0=Sunday, 1=Monday to 6=Saturday.
    ///
    DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, uint8_t dayOfWeek);

private: // 64bit
    uint16_t _year; ///< The year
    uint8_t _month; ///< The month 1-12
    uint8_t _day; ///< The day 1-31
    uint8_t _hour; ///< The hour 0-23
    uint8_t _minute; ///< The minute 0-59
    uint8_t _second; ///< The second 0-59
    uint8_t _dayOfWeek; ///< The day of the week 0-6
};


}


