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
#include "DateTime.hpp"


#include <tuple>


namespace lr {
  
    
namespace {
    

// The number of days per month.
static const uint8_t cDaysPerMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// The number of seconds per day.
static const uint32_t cSecondsPerDay = 86400;

// The number of seconds per hour.
static const uint16_t cSecondsPerHour = 3600;
    
// The number of seconds per minute.
static const uint16_t cSecondsPerMinute = 60;
    
// The number of days for a regular year.
static const uint32_t cDaysPerNormalYear = 365;
    

// Calculate the day of the week.
// Using the formula from: http://www.tondering.dk/claus/cal/chrweek.php
static uint8_t calculateDayOfWeek(int16_t year, int16_t month, int16_t day)
{
    const int16_t a = ((14 - month) / 12);
    const int16_t y = year - a;
    const int16_t m = month + (12 * a) - 2;
    const int16_t d = (day + y + (y/4) - (y/100) + (y/400) + ((31 * m)/12)) % 7;
    return d;
}

    
static inline bool isLeapYear(uint16_t year)
{
    return ((year&3) == 0 && year%100 != 0) || (year%400 == 0);
}

    
static inline uint8_t getMaxDayPerMonth(uint16_t year, uint8_t month)
{
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    return cDaysPerMonth[month];
}

    
static inline uint32_t getDaysForYear(uint16_t year)
{
    if (isLeapYear(year)) {
        return cDaysPerNormalYear + 1;
    } else {
        return cDaysPerNormalYear;
    }
}

    
}
    
    
DateTime::DateTime()
    : _year(2000), _month(1), _day(1), _hour(0), _minute(0), _second(0), _dayOfWeek(6)
{
}

    
DateTime::DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
    setDate(year, month, day);
    setTime(hour, minute, second);
}

    
DateTime::DateTime(
    uint16_t year,
    uint8_t month,
    uint8_t day,
    uint8_t hour,
    uint8_t minute,
    uint8_t second,
    uint8_t dayOfWeek)
:
    _year(year),
    _month(month),
    _day(day),
    _hour(hour),
    _minute(minute),
    _second(second),
    _dayOfWeek(dayOfWeek)
{
}


DateTime::DateTime(const DateTime &other)
:
    _year(other._year),
    _month(other._month),
    _day(other._day),
    _hour(other._hour),
    _minute(other._minute),
    _second(other._second),
    _dayOfWeek(other._dayOfWeek)
{
}


DateTime& DateTime::operator=(const DateTime &other)
{
    _year = other._year;
    _month = other._month;
    _day = other._day;
    _dayOfWeek = other._dayOfWeek;
    _hour = other._hour;
    _minute = other._minute;
    _second = other._second;
    return *this;
}


DateTime::~DateTime()
{
}

    
bool DateTime::operator==(const DateTime &other) const
{
	return std::tie(_year, _month, _day, _hour, _minute, _second)
		== std::tie(other._year, other._month, other._day, other._hour, other._minute, other._second);
}


bool DateTime::operator!=(const DateTime &other) const
{
    return !operator==(other);
}


bool DateTime::operator<(const DateTime &other) const
{
	return std::tie(_year, _month, _day, _hour, _minute, _second)
		< std::tie(other._year, other._month, other._day, other._hour, other._minute, other._second);
}


bool DateTime::operator<=(const DateTime &other) const
{
    return operator<(other) || operator==(other);
}


bool DateTime::operator>(const DateTime &other) const
{
	return std::tie(_year, _month, _day, _hour, _minute, _second)
		> std::tie(other._year, other._month, other._day, other._hour, other._minute, other._second);
}


bool DateTime::operator>=(const DateTime &other) const
{
    return operator>(other) || operator==(other);
}

    
void DateTime::setDate(uint16_t year, uint16_t month, uint16_t day)
{
    // force all values into valid ranges.
    if (year < 2000) {
        _year = 2000;
    } else if (year > 9999) {
        _year = 9999;
    } else {
        _year = year;
    }
    if (month < 1) {
        _month = 1;
    } else if (month > 12) {
        _month = 12;
    } else {
        _month = month;
    }
    const uint8_t maxDayPerMonth = getMaxDayPerMonth(_year, _month);
    if (day < 1) {
        _day = 1;
    } else if (day > maxDayPerMonth) {
        _day = maxDayPerMonth;
    } else {
        _day = day;
    }
    _dayOfWeek = calculateDayOfWeek(_year, _month, _day);
}

    
void DateTime::setTime(uint8_t hour, uint8_t minute, uint8_t second)
{
    if (hour > 23) {
        _hour = 23;
    } else {
        _hour = hour;
    }
    if (minute > 59) {
        _minute = 59;
    } else {
        _minute = minute;
    }
    if (second > 59) {
        _second = 59;
    } else {
        _second = second;
    }
}


void DateTime::addOneSecond()
{
    if (++_second < 60) return;
    _second = 0;
    if (++_minute < 60) return;
    _minute = 0;
    if (++_hour < 24) return;
    _hour = 0;
    if (++_dayOfWeek > 6) {
        _dayOfWeek = 0;
    }
    const uint8_t maxDayPerMonth = getMaxDayPerMonth(_year, _month);
    if (++_day <= maxDayPerMonth) return;
    _day = 1;
    if (++_month <= 12) return;
    _month = 1;
    ++_year;
}


uint16_t DateTime::getYear() const
{
    return _year;
}


uint8_t DateTime::getMonth() const
{
    return _month;
}


uint8_t DateTime::getDay() const
{
    return _day;
}


uint8_t DateTime::getDayOfWeek() const
{
    return _dayOfWeek;
}


uint8_t DateTime::getHour() const
{
    return _hour;
}


uint8_t DateTime::getMinute() const
{
    return _minute;
}


uint8_t DateTime::getSecond() const
{
    return _second;
}

    
DateTime DateTime::addSeconds(int32_t seconds) const
{
    return fromSecondsSince2000(toSecondsSince2000() + seconds);
}


DateTime DateTime::addDays(int32_t days) const
{
    return fromSecondsSince2000(toSecondsSince2000() + days*cSecondsPerDay);
}

    
int32_t DateTime::secondsTo(const DateTime &other) const
{
    return static_cast<int32_t>(other.toSecondsSince2000()) - static_cast<int32_t>(toSecondsSince2000());
}

    
uint32_t DateTime::toSecondsSince2000() const
{
    // This calculation will require some CPU cycles. It is an
    // programmatic solution, no mathematical one.

    uint32_t seconds = 0;
    for (uint16_t year = 2000; year < _year; ++year) {
        seconds += (getDaysForYear(year) * static_cast<uint32_t>(cSecondsPerDay));
    }
    for (uint8_t month = 1; month < _month; ++month) {
        seconds += (static_cast<uint32_t>(getMaxDayPerMonth(_year, month)) * static_cast<uint32_t>(cSecondsPerDay));
    }
    seconds += static_cast<uint32_t>(_day-1) * static_cast<uint32_t>(cSecondsPerDay);
    seconds += static_cast<uint32_t>(_hour) * static_cast<uint32_t>(cSecondsPerHour);
    seconds += static_cast<uint32_t>(_minute) * static_cast<uint32_t>(cSecondsPerMinute);
    seconds += static_cast<uint32_t>(_second);
    return seconds;
}

    
bool DateTime::isFirst() const
{
    return _year == 2000 && _month == 1 && _day == 1 && _hour == 0 && _minute == 0 && _second == 0;
}
    
    
String DateTime::toString(Format format) const
{
    String result;
    result.reserve(20); // longest format.
    switch (format) {
        case Format::ISO:
        case Format::Long:
            result.appendNumber(_year, 4, '0');
            result.append('-');
            result.appendNumber(_month, 2, '0');
            result.append('-');
            result.appendNumber(_day, 2, '0');
            if (format == Format::ISO) {
                result.append('T');
            } else {
                result.append(' ');
            }
            result.appendNumber(_hour, 2, '0');
            result.append(':');
            result.appendNumber(_minute, 2, '0');
            result.append(':');
            result.appendNumber(_second, 2, '0');
            break;
        case Format::ISODate:
            result.appendNumber(_year, 4, '0');
            result.append('-');
            result.appendNumber(_month, 2, '0');
            result.append('-');
            result.appendNumber(_day, 2, '0');
            break;
        case Format::ISOBasicDate:
            result.appendNumber(_year, 4, '0');
            result.appendNumber(_month, 2, '0');
            result.appendNumber(_day, 2, '0');
            break;
        case Format::ISOTime:
            result.appendNumber(_hour, 2, '0');
            result.append(':');
            result.appendNumber(_minute, 2, '0');
            result.append(':');
            result.appendNumber(_second, 2, '0');
            break;
        case Format::ISOBasicTime:
            result.appendNumber(_hour, 2, '0');
            result.appendNumber(_minute, 2, '0');
            result.appendNumber(_second, 2, '0');
            break;
        case Format::ShortDate:
            result.appendNumber(_day, 2, '0');
            result.append('.');
            result.appendNumber(_month, 2, '0');
            result.append('.');
            break;
        case Format::ShortTime:
            result.appendNumber(_hour, 2, '0');
            result.append(':');
            result.appendNumber(_minute, 2, '0');
            break;
    }
    return result;
}

    
DateTime DateTime::fromSecondsSince2000(uint32_t secondsSince2000)
{
    // This calculation will require some CPU cycles. It is an
    // programmatic solution, no mathematical one. This function
    // is approximate 6 times slower than mathematical implementations.
    
    // Calculate the time
    uint32_t secondsSinceMidnight = secondsSince2000%cSecondsPerDay;
    const uint8_t hours = secondsSinceMidnight/static_cast<uint32_t>(cSecondsPerHour);
    secondsSinceMidnight %= static_cast<uint32_t>(cSecondsPerHour);
    const uint8_t minutes = secondsSinceMidnight/static_cast<uint32_t>(cSecondsPerMinute);
    const uint8_t seconds = secondsSinceMidnight % static_cast<uint32_t>(cSecondsPerMinute);
    // Calculate the date
    uint32_t days = secondsSince2000/static_cast<uint32_t>(cSecondsPerDay);
    const uint8_t dayOfWeek = (days+6)%7; // 2000-01-01 was Saturday (6)
    uint16_t year = 2000;
    uint32_t daysForThisSection = getDaysForYear(year);
    while (days >= daysForThisSection) {
        ++year;
        days -= daysForThisSection;
        daysForThisSection = getDaysForYear(year);
    }
    uint16_t month = 1;
    daysForThisSection = getMaxDayPerMonth(year, month);
    while (days >= daysForThisSection) {
        ++month;
        days -= daysForThisSection;
        daysForThisSection = getMaxDayPerMonth(year, month);
    }
    return DateTime(year, month, days+1, hours, minutes, seconds, dayOfWeek);
}

    
DateTime DateTime::fromUncheckedValues(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, uint8_t dayOfWeek)
{
    return DateTime(year, month, day, hour, minute, second, dayOfWeek);
}

    
}

