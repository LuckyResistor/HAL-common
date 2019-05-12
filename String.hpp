#pragma once
//
// String
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


namespace lr {


/// A simple string class for embedded systems.
///
/// This string class is designed to provide a compromise between a
/// low memory footprint and fast execution time. Also most security
/// checks for imported char arrays are omitted, therefore importing
/// strings without null end byte may lead to undefined results.
///
class String
{
public:
    /// The size type for the string.
    ///
    typedef uint32_t Size;

public: // Construction and Copy
    /// Create an empty string.
    ///
    String() noexcept;

    /// Destroy the string.
    ///
    ~String() noexcept;

    /// Create a new string.
    ///
    /// This will copy the given string to create a new string object.
    ///
    /// @param[in] str The string to copy. This string has to end in a zero byte.
    ///
    String(const char *str) noexcept;
    
    /// Copy a string.
    ///
    /// @param[in] other The other string to copy.
    ///
    String(const String &other) noexcept;
    
    /// Copy a string (move).
    ///
    /// @param[in] other The other string to move.
    ///
    String(String &&other) noexcept;
    
    /// Assign a string.
    ///
    /// @param[in] other The other string to assign.
    ///
    String& operator=(const String &other) noexcept;

    /// Assign a string.
    ///
    /// @param[in] str The other string to assign.
    ///
    String& operator=(const char *str) noexcept;

    /// Assign a string (move)
    ///
    /// @param[in] other The other string to assign via move.
    ///
    String& operator=(String &&other) noexcept;
    
public: // Operators
    /// Compare this string with another one.
    ///
    bool operator==(const String &other) const noexcept;

    /// Compare this string with another one.
    ///
    bool operator==(const char *str) const noexcept;

    /// Compare this string with another one.
    ///
    bool operator!=(const String &other) const noexcept;

    /// Compare this string with another one.
    ///
    bool operator!=(const char *str) const noexcept;

    /// Compare this string with another one.
    ///
    bool operator<(const String &other) const noexcept;

    /// Compare this string with another one.
    ///
    bool operator<(const char *str) const noexcept;

    /// Compare this string with another one.
    ///
    bool operator<=(const String &other) const noexcept;

    /// Compare this string with another one.
    ///
    bool operator<=(const char *str) const noexcept;

    /// Compare this string with another one.
    ///
    bool operator>(const String &other) const noexcept;

    /// Compare this string with another one.
    ///
    bool operator>(const char *str) const noexcept;

    /// Compare this string with another one.
    ///
    bool operator>=(const String &other) const noexcept;

    /// Compare this string with another one.
    ///
    bool operator>=(const char *str) const noexcept;

public: // String Manipulation
    /// Append a string to this one.
    ///
    /// @param[in] other The other string to append.
    ///
    void append(const String &other) noexcept;

    /// Append a string to this one.
    ///
    /// @param[in] str The other string to append.
    ///
    void append(const char *str) noexcept;

    /// Append a char to this one.
    ///
    /// @param[in] c The char to append.
    ///
    void append(char c) noexcept;

    /// Append a value in hex representation to this string.
    ///
    /// @param[in] value The value to append.
    ///
    void appendHex(uint8_t value) noexcept;

    /// Append a value in hex representation to this string.
    ///
    /// @param[in] value The value to append.
    ///
    void appendHex(uint16_t value) noexcept;
    
    /// Append a value in hex representation to this string.
    ///
    /// @param[in] value The value to append.
    ///
    void appendHex(uint32_t value) noexcept;

    /// Append a value in binary representation to this string.
    ///
    /// @param[in] value The value to append.
    ///
    void appendBin(uint8_t value) noexcept;

    /// Append a value in binary representation to this string.
    ///
    /// @param[in] value The value to append.
    ///
    void appendBin(uint16_t value) noexcept;

    /// Append a value in binary representation to this string.
    ///
    /// @param[in] value The value to append.
    ///
    void appendBin(uint32_t value) noexcept;

    /// Append a value as decimal number to this string.
    ///
    /// @param[in] value The value to append.
    ///
    void appendNumber(uint32_t value, uint8_t width = 0, char fillChar = ' ') noexcept;

    /// Append a value as decimal number to this string.
    ///
    /// @param[in] value The value to append.
    ///
    void appendNumber(int32_t value, uint8_t width = 0, char fillChar = ' ') noexcept;

public:
    /// Create a string with the hex representation for the given value.
    ///
    /// @param[in] value The value to convert.
    /// @return A hex string in the format "00".
    ///
    static String hex(uint8_t value) noexcept;

    /// Create a string with the hex representation for the given value.
    ///
    /// @param[in] value The value to convert.
    /// @return A hex string in the format "0000".
    ///
    static String hex(uint16_t value) noexcept;

    /// Create a string with the hex representation for the given value.
    ///
    /// @param[in] value The value to convert.
    /// @return A hex string in the format "00000000".
    ///
    static String hex(uint32_t value) noexcept;

    /// Get a hex digit from a nibble.
    ///
    constexpr static char getHexDigit(const uint8_t value) noexcept {
        return ((value <= 9) ? ('0'+value) : ('a'-0xa+value));
    }
    
    /// Create a string with the binary representation for the given value.
    ///
    /// @param[in] value The value to convert.
    /// @return A binary string in the format "00010111".
    ///
    static String bin(uint8_t value) noexcept;

    /// Create a string with the binary representation for the given value.
    ///
    /// @param[in] value The value to convert.
    /// @return A binary string in the format "0001011100010111".
    ///
    static String bin(uint16_t value) noexcept;

    /// Create a string with the binary representation for the given value.
    ///
    /// @param[in] value The value to convert.
    /// @return A binary string in the format "00010111000101110001011100010111".
    ///
    static String bin(uint32_t value) noexcept;

    /// Create a string with a decimal representation for the given value.
    ///
    /// @param[in] value The value to convert.
    /// @param[in] width The width of the field. A zero value will
    ///     convert all digits. Any positive value will create a string with
    ///     exact this size with the number right aligned in this string.
    ///     Any excess digits are cut-off.
    /// @param[in] fillChar The character to fill the field.
    /// @return A string with the decimal number.
    ///
    static String number(uint32_t value, uint8_t width = 0, char fillChar = ' ') noexcept;

    /// Create a string with a decimal representation for the given value.
    ///
    /// @param[in] value The value to convert.
    /// @param[in] width The width of the field. A zero value will
    ///     convert all digits. Any positive value will create a string with
    ///     exact this size with the number right aligned in this string.
    ///     Any excess digits are cut-off.
    /// @param[in] fillChar The character to fill the field.
    /// @return A string with the decimal number.
    ///
    static String number(int32_t value, uint8_t width = 0, char fillChar = ' ') noexcept;
    
public: // Overloads to solve some ambiguous calls.
    /// Create a string with a decimal representation for the given value.
    ///
    inline static String number(uint8_t value, uint8_t width = 0, char fillChar = ' ') noexcept {
        return number(static_cast<uint32_t>(value), width, fillChar);
    }

    /// Create a string with a decimal representation for the given value.
    ///
    inline static String number(uint16_t value, uint8_t width = 0, char fillChar = ' ') noexcept {
        return number(static_cast<uint32_t>(value), width, fillChar);
    }

    /// Create a string with a decimal representation for the given value.
    ///
    inline static String number(int8_t value, uint8_t width = 0, char fillChar = ' ') noexcept {
        return number(static_cast<int32_t>(value), width, fillChar);
    }

    /// Create a string with a decimal representation for the given value.
    ///
    inline static String number(int16_t value, uint8_t width = 0, char fillChar = ' ') noexcept {
        return number(static_cast<int32_t>(value), width, fillChar);
    }

    /// Append a value as decimal number to this string.
    ///
    inline void appendNumber(uint8_t value, uint8_t width = 0, char fillChar = ' ') noexcept {
        appendNumber(static_cast<uint32_t>(value), width, fillChar);
    }

    /// Append a value as decimal number to this string.
    ///
    inline void appendNumber(uint16_t value, uint8_t width = 0, char fillChar = ' ') noexcept {
        appendNumber(static_cast<uint32_t>(value), width, fillChar);
    }

    /// Append a value as decimal number to this string.
    ///
    inline void appendNumber(int8_t value, uint8_t width = 0, char fillChar = ' ') noexcept {
        appendNumber(static_cast<int32_t>(value), width, fillChar);
    }

    /// Append a value as decimal number to this string.
    ///
    inline void appendNumber(int16_t value, uint8_t width = 0, char fillChar = ' ') noexcept {
        appendNumber(static_cast<int32_t>(value), width, fillChar);
    }

public:
    /// Check if this string is empty.
    ///
    bool isEmpty() const noexcept;
    
    /// Get the length of this string.
    ///
    Size getLength() const noexcept;
    
    /// Get the capacity of this string.
    ///
    Size getCapacity() const noexcept;

    /// Access the data of this string
    ///
    const char* getData() const noexcept;

    /// Access a single character of this string.
    ///
    inline char getCharAt(Size index) const noexcept {
        return _data[index];
    }
    
    /// Reserve memory for the string.
    ///
    /// This will reserve minimum space for a string with the size `size`.
    /// Depending on the string length, this function will most likely
    /// reserve more memory. The function will never reduce the capacity.
    ///
    /// @param[in] capacity The minimum capacity to reserve.
    ///
    void reserve(Size capacity) noexcept;

    /// Squeeze the memory for this string.
    ///
    /// Calling this function will reduce the memory to the minimum
    /// size. Future modifications of the string will most likely
    /// need memory reallocation which is expensive.
    ///
    void squeeze() noexcept;

private:
    Size _length; ///< The length of the string (without zero byte).
    Size _capacity; ///< The capacity of the data block.
    char *_data; ///< The string data.
};


}
