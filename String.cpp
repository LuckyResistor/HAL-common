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


#include <cstring>
#include <cstdlib>


namespace lr {


String::String() noexcept
    : _length(0), _capacity(0), _data(nullptr)
{
}


String::~String() noexcept
{
    if (_data != nullptr) {
        free(_data);
    }
}


String::String(const char *str) noexcept
    : _length(0), _capacity(0), _data(nullptr)
{
    _length = static_cast<Size>(std::strlen(str));
    reserve(_length);
    std::memcpy(_data, str, _length+1);
}


String::String(const char *str, String::Size count) noexcept
    : _length(count), _capacity(0), _data(nullptr)
{
    reserve(_length);
    std::memcpy(_data, str, _length);
    _data[_length] = '\0'; // Add the missing end mark.
}


String::String(const String &other) noexcept
    : _length(other._length), _capacity(other._length), _data(nullptr)
{
    reserve(other.getLength());
    std::memcpy(_data, other._data, _length+1);
}


String::String(String &&other) noexcept
    : _length(other._length), _capacity(other._length), _data(other._data)
{
    other._data = nullptr;
    other._length = 0;
    other._capacity = 0;
}


String& String::operator=(const String &other) noexcept
{
    if (_data != nullptr) {
        free(_data);
        _data = nullptr;
        _capacity = 0;
    }
    _length = other._length;
    reserve(_length);
    std::memcpy(_data, other._data, _length+1);
    return *this;
}


String& String::operator=(const char *str) noexcept
{
    if (_data != nullptr) {
        free(_data);
        _data = nullptr;
        _capacity = 0;
    }
    const Size strLen = std::strlen(str);
    _length = strLen;
    reserve(_length);
    std::strcpy(_data, str);
    return *this;
}


String& String::operator=(String &&other) noexcept
{
    _length = other._length;
    _capacity = other._length;
    _data = other._data;
    other._data = nullptr;
    other._length = 0;
    other._capacity = 0;
    return *this;
}


bool String::operator==(const String &other) const noexcept
{
    if (_data == nullptr && other._data == nullptr) {
        return true;
    } else if (_data == nullptr || other._data == nullptr) {
        return false;
    }
    return std::strcmp(_data, other._data) == 0;
}


bool String::operator==(const char *str) const noexcept
{
    if (_data == nullptr && str == nullptr) {
        return true;
    } else if (_data == nullptr || str == nullptr) {
        return false;
    }
    return std::strcmp(_data, str) == 0;
}


bool String::operator!=(const String &other) const noexcept
{
    return !operator==(other);
}


bool String::operator!=(const char *str) const noexcept
{
    return !operator==(str);
}


bool String::operator<(const String &other) const noexcept
{
    if (_data == nullptr && other._data != nullptr) {
        return true;
    } else if (_data == nullptr || other._data == nullptr) {
        return false;
    }
    return std::strcmp(_data, other._data) < 0;
}


bool String::operator<(const char *str) const noexcept
{
    if (_data == nullptr && str != nullptr) {
        return true;
    } else if (_data == nullptr || str == nullptr) {
        return false;
    }
    return std::strcmp(_data, str) < 0;
}


bool String::operator<=(const String &other) const noexcept
{
    return operator==(other)||operator<(other);
}


bool String::operator<=(const char *str) const noexcept
{
    return operator==(str)||operator<(str);
}


bool String::operator>(const String &other) const noexcept
{
    if (_data != nullptr && other._data == nullptr) {
        return true;
    } else if (_data == nullptr || other._data == nullptr) {
        return false;
    }
    return std::strcmp(_data, other._data) > 0;
}


bool String::operator>(const char *str) const noexcept
{
    if (_data != nullptr && str == nullptr) {
        return true;
    } else if (_data == nullptr || str == nullptr) {
        return false;
    }
    return std::strcmp(_data, str) > 0;
}


bool String::operator>=(const String &other) const noexcept
{
    return operator==(other)||operator>(other);
}


bool String::operator>=(const char *str) const noexcept
{
    return operator==(str)||operator>(str);
}


void String::append(const String &other) noexcept
{
    if (other.isEmpty()) {
        return;
    }
    const Size newLength = _length + other.getLength();
    reserve(newLength);
    std::strcpy(_data + _length, other._data);
    _length = newLength;
}


void String::append(const char *str) noexcept
{
    if (str == nullptr) {
        return;
    }
    const Size strLen = std::strlen(str);
    if (strLen == 0) {
        return;
    }
    const Size newLength = _length + strLen;
    reserve(newLength);
    std::strcpy(_data + _length, str);
    _length = newLength;
}


void String::append(char c) noexcept
{
    const Size newLength = _length + 1;
    reserve(newLength);
    _data[_length] = c;
    _data[_length+1] = '\0';
    _length = newLength;
}


void String::reserve(Size capacity) noexcept
{
    if (capacity < 32) {
        // For strings below 32 bytes, reserve in 8 byte steps.
        capacity = ((capacity/4)+1)*4;
    } else if (capacity < 128) {
        // For strings below 128 bytes, reserve in 16 byte steps.
        capacity = ((capacity/16)+1)*16;
    } else {
        // For larger strings, reserve in 32 byte steps.
        capacity = ((capacity/32)+1)*32;
    }
    if (_capacity >= capacity) {
        return;
    }
    // Change or allocate the memory block.
    _data = static_cast<char*>(std::realloc(_data, capacity+1));
    // If this operation shortens the string, adjust the length and add a zero byte.
    if (capacity < _length) {
        _data[capacity] = '\0';
        _length = capacity;
    }
    _capacity = capacity;
}


void String::squeeze() noexcept
{
    if (_length != _capacity) {
        _capacity = _length;
        _data = static_cast<char*>(std::realloc(_data, _capacity+1));
    }
}


namespace {

    
/// Helper function to convert a value into a hex string.
///
template<typename Type, uint8_t digitCount>
String String_hex(Type value) noexcept
{
    String result;
    result.reserve(digitCount);
    for (uint8_t i = 0; i < digitCount; ++i) {
        const auto nibble = static_cast<uint8_t>(value >> ((digitCount-1)*4));
        result.append(String::getHexDigit(nibble));
        value <<= 4;
    }
    return result;
}


/// Helper function to append a value as hex string.
///
template<typename Type, uint8_t digitCount>
void String_appendHex(String &str, Type value) noexcept
{
    for (uint8_t i = 0; i < digitCount; ++i) {
        const auto nibble = static_cast<uint8_t>(value >> ((digitCount-1)*4));
        str.append(String::getHexDigit(nibble));
        value <<= 4;
    }
}

    
/// Helper function to convert a value into a binary string.
///
template<typename Type, uint8_t digitCount>
String String_bin(Type value) noexcept
{
    String result;
    result.reserve(digitCount);
    for (uint8_t i = 0; i < digitCount; ++i) {
        if ((value & (static_cast<Type>(1)<<(digitCount-1))) != 0) {
            result.append('1');
        } else {
            result.append('0');
        }
        value <<= 1;
    }
    return result;
}

    
/// Helper function to append a value as binary string.
///
template<typename Type, uint8_t digitCount>
void String_appendBin(String &str, Type value) noexcept
{
    for (uint8_t i = 0; i < digitCount; ++i) {
        if ((value & (static_cast<Type>(1)<<(digitCount-1))) != 0) {
            str.append('1');
        } else {
            str.append('0');
        }
        value <<= 1;
    }
}

    
/// Prepare decimal digits, in reverse order.
///
inline uint8_t String_prepareDigits(char *digits, uint32_t value) noexcept
{
    uint8_t digitCount = 0;
    if (value == 0) {
        digits[0] = '0';
        digitCount = 1;
    } else {
        while (value != 0) {
            const uint8_t index = (value % 10);
            value /= 10;
            digits[digitCount] = ('0' + index);
            digitCount += 1;
        }
    }
    return digitCount;
}

    
/// Align the prepared digits, with padding and in the correct order.
///
inline void String_alignNumber(String &str, const char *digits, const uint8_t digitCount, const uint8_t width, const char fillChar)
{
    for (uint8_t i = 0; i < (width-digitCount); ++i) {
        str.append(fillChar);
    }
    if (width >= digitCount) {
        for (int8_t i = digitCount-1; i >= 0; --i) {
            str.append(digits[i]);
        }
    } else {
        const int8_t dropCount = (digitCount-width);
        for (int8_t i = digitCount-1; i >= dropCount; --i) {
            str.append(digits[i]);
        }
    }
}

    
/// Create a string with the aligned digits.
///
inline String String_createNumber(const char *digits, const uint8_t digitCount, const uint8_t width, const char fillChar)
{
    String result;
    result.reserve(width);
    String_alignNumber(result, digits, digitCount, width, fillChar);
    return result;
}


}
    
    
String String::hex(uint8_t value) noexcept
{
    return String_hex<uint8_t, 2>(value);
}

    
String String::hex(uint16_t value) noexcept
{
    return String_hex<uint16_t, 4>(value);
}

    
String String::hex(uint32_t value) noexcept
{
    return String_hex<uint32_t, 8>(value);
}

    
String String::number(uint32_t value, uint8_t width, char fillChar) noexcept
{
    // Prepare all required digits.
    char digits[11];
    const auto digitCount = String_prepareDigits(digits, value);
    if (width == 0) {
        width = digitCount;
    }
    // Create the aligned string.
    return String_createNumber(digits, digitCount, width, fillChar);
}

    
String String::number(int32_t value, uint8_t width, char fillChar) noexcept
{
    // Prepare all required digits.
    char digits[12];
    uint8_t digitCount;
    if (value < 0) {
        digitCount = String_prepareDigits(digits, value * -1);
        digits[digitCount] = '-';
        digitCount += 1;
    } else {
        digitCount = String_prepareDigits(digits, value);
    }
    if (width == 0) {
        width = digitCount;
    }
    // Create the aligned string.
    return String_createNumber(digits, digitCount, width, fillChar);
}

                      
String String::bin(uint8_t value) noexcept
{
    return String_bin<uint8_t, 8>(value);
}

                      
String String::bin(uint16_t value) noexcept
{
    return String_bin<uint16_t, 16>(value);
}

                      
String String::bin(uint32_t value) noexcept
{
    return String_bin<uint32_t, 32>(value);
}

    
void String::appendHex(uint8_t value) noexcept
{
    String_appendHex<uint8_t, 2>(*this, value);
}


void String::appendHex(uint16_t value) noexcept
{
    String_appendHex<uint16_t, 4>(*this, value);
}

    
void String::appendHex(uint32_t value) noexcept
{
    String_appendHex<uint32_t, 8>(*this, value);
}

    
void String::appendBin(uint8_t value) noexcept
{
    String_appendBin<uint8_t, 8>(*this, value);
}

    
void String::appendBin(uint16_t value) noexcept
{
    String_appendBin<uint16_t, 16>(*this, value);
}

    
void String::appendBin(uint32_t value) noexcept
{
    String_appendBin<uint32_t, 32>(*this, value);
}

    
void String::appendNumber(uint32_t value, uint8_t width, char fillChar) noexcept
{
    // Prepare all required digits.
    char digits[11];
    const auto digitCount = String_prepareDigits(digits, value);
    if (width == 0) {
        width = digitCount;
    }
    // Append the aligned string.
    String_alignNumber(*this, digits, digitCount, width, fillChar);
}

    
void String::appendNumber(int32_t value, uint8_t width, char fillChar) noexcept
{
    // Prepare all required digits.
    char digits[12];
    uint8_t digitCount;
    if (value < 0) {
        digitCount = String_prepareDigits(digits, value * -1);
        digits[digitCount] = '-';
        digitCount += 1;
    } else {
        digitCount = String_prepareDigits(digits, value);
    }
    if (width == 0) {
        width = digitCount;
    }
    // Append the aligned string.
    String_alignNumber(*this, digits, digitCount, width, fillChar);
}


String::Size String::getFirstIndex(char c) const noexcept
{
    return getFirstIndex(c, 0);
}


String::Size String::getFirstIndex(char c, String::Size offset) const noexcept
{
    if (_length == 0) {
        return notFound;
    }
    for (Size i = offset; i < _length; ++i) {
        if (_data[i] == c) {
            return i;
        }
    }
    return notFound;
}


String String::getSlice(String::Size index, String::Size length) const noexcept
{
    if (index >= _length) {
        return String();
    }
    if (index + length > _length) {
        length = (_length - index);
    }
    return String(_data + index, length);
}


String String::getTail(String::Size index) const noexcept
{
    return getSlice(index, notFound);
}


namespace {


/// Try to convert this string into an integer of the given type.
///
/// This function will return an error if the string contains an number which cannot be stored in the chosen
/// integer type.
///
/// @tparam IntType The integer type for the result.
/// @return The result of the conversion.
///
template<typename IntType>
StatusResult<IntType> toInt(const String &str) noexcept
{
    if (str.isEmpty()) {
        return StatusResult<IntType>::error();
    }
    IntType result = 0;
    IntType factor;
    if (str.getCharAt(0) == '-') {
        // Negative number are only allowed for signed integer types.
        if (!std::numeric_limits<IntType>::is_signed) {
            return StatusResult<IntType>::error();
        }
        factor = -1;
    } else {
        factor = 1;
    }
    String::Size index = str.getLength();
    while (index > 0) {
        index -= 1;
        const char c = str.getCharAt(index);
        if (c >= '0' && c <= '9') {
            const auto digit = static_cast<IntType>(c - '0');
            // Only handle digits greater than zero.
            if (digit > 0) {
                // If the factor already reached the end of the range, this will not work.
                if (factor == 0) {
                    return StatusResult<IntType>::error();
                }
                // Try to calculate the digit for the current factor.
                IntType digitWithFactor;
                if (IntegerMath::multiplyCheckOverflow(digit, factor, &digitWithFactor)) {
                    return StatusResult<IntType>::error();
                }
                // Try to add it to the result.
                if (IntegerMath::addCheckOverflow(digitWithFactor, result, &result)) {
                    return StatusResult<IntType>::error();
                }
            }
        } else if (c == '-') {
            if (index > 0) {
                // The sign has to be the last element for the number.
                return StatusResult<IntType>::error();
            }
        } else {
            // Invalid character in the number.
            return StatusResult<IntType>::error();
        }
        // If the factor already had an overflow, ignore it.
        if (factor != 0) {
            // Try to move the factor.
            if (IntegerMath::multiplyCheckOverflow(factor, static_cast<IntType>(10), &factor)) {
                factor = 0;
            }
        }
    }
    return StatusResult<IntType>::success(result);
}


}


StatusResult<uint32_t> String::toUInt32() const
{
    return toInt<uint32_t>(*this);
}


StatusResult<int32_t> String::toInt32() const
{
    return toInt<int32_t>(*this);
}


StatusResult<uint16_t> String::toUInt16() const
{
    return toInt<uint16_t>(*this);
}


StatusResult<int16_t> String::toInt16() const
{
    return toInt<int16_t>(*this);
}


StatusResult<uint8_t> String::toUInt8() const
{
    return toInt<uint8_t>(*this);
}


StatusResult<int8_t> String::toInt8() const
{
    return toInt<int8_t>(*this);
}


}

