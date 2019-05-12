#pragma once
//
// 16bit fixed fraction value
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


/// Fixed 16bit math.
///
class Fixed16
{
public:
	/// The internal type of the pixel value.
	///
	typedef int32_t Type;

public: // Constructors
	/// Create a zero pixel value.
	///
	constexpr Fixed16() noexcept : _value(0) {}

	/// Create a value from the internal type.
	///
	constexpr Fixed16(Type value) noexcept : _value(value) {};

	/// Create a value from the given float.
	///
	constexpr Fixed16(float value) noexcept : _value(static_cast<Type>((value>=0)?(value*65536.0f+0.5f):(value*65536.0f-0.5f))) {}

	/// Create a value from the given integer.
	///
	constexpr Fixed16(int16_t value) noexcept : _value(static_cast<Type>(value)<<16) {};

	/// Copy constructor.
	///
	Fixed16(const Fixed16 &value) noexcept;

public: // Assignment
	Fixed16& operator=(const Fixed16 &other) noexcept;

public: // Operators
	Fixed16 operator+(const Fixed16 &other) const noexcept;
	Fixed16 operator-(const Fixed16 &other) const noexcept;
	Fixed16 operator*(const Fixed16 &other) const noexcept;
	Fixed16 operator/(const Fixed16 &other) const noexcept;

public: // Manipulators
	Fixed16& operator+=(const Fixed16 &other) noexcept;
	Fixed16& operator-=(const Fixed16 &other) noexcept;
	Fixed16& operator*=(const Fixed16 &other) noexcept;
	Fixed16& operator/=(const Fixed16 &other) noexcept;

public: // Saturating operators
	Fixed16 saturatingAdd(const Fixed16 &other) const noexcept;
	Fixed16 saturatingSubtract(const Fixed16 &other) const noexcept;
	Fixed16 saturatingMultiply(const Fixed16 &other) const noexcept;
	Fixed16 saturatingDivide(const Fixed16 &other) const noexcept;

public: // Comparators
	bool operator==(const Fixed16 &other) const noexcept;
	bool operator!=(const Fixed16 &other) const noexcept;
	bool operator<=(const Fixed16 &other) const noexcept;
	bool operator>=(const Fixed16 &other) const noexcept;
	bool operator<(const Fixed16 &other) const noexcept;
	bool operator>(const Fixed16 &other) const noexcept;

public: // Constants
	static inline Fixed16 one() noexcept { return Fixed16(static_cast<Type>(0x00010000UL)); }
	static inline Fixed16 pi() noexcept { return Fixed16(static_cast<Type>(205887UL)); }
	static inline Fixed16 e() noexcept { return Fixed16(static_cast<Type>(178145UL)); }
	static inline Fixed16 minimum() noexcept { return Fixed16(static_cast<Type>(0x80000000UL)); }
	static inline Fixed16 maximum() noexcept { return Fixed16(static_cast<Type>(0x7FFFFFFFUL)); }
	static inline Fixed16 overflow() noexcept { return Fixed16(static_cast<Type>(0x80000000UL)); }

public: // Checks
	/// Check if this value is zero
	///
	constexpr bool isZero() const noexcept {
		return _value == 0;
	}
	
public: // Math
	/// Get the absolute value.
	///
	inline Fixed16 getAbsolute() const {
		return Fixed16(_value < 0 ? -_value : _value);
	}
	
	/// Get the floor value.
	///
	inline Fixed16 getFloor() const {
		return Fixed16(static_cast<Type>(_value & 0xffff0000UL));
	}
	 
    /// Get the rounded value.
    ///
    inline Fixed16 getRounded() const {
        auto result = static_cast<Type>(_value & 0xffff0000UL);
        const auto signs = _value & 0x80008000UL;
        if (signs == 0x00008000UL) {
            result += 0x00010000UL;
        } else if (signs == 0x80000000) {
            result -= 0x00010000UL;
        }
        return result;
    }
  
	/// Get the ceil value.
	///
	inline Fixed16 getCeiling() const {
		return Fixed16(static_cast<Type>((_value & 0xFFFF0000UL) + (((_value & 0x0000FFFFUL) != 0)?0x00010000:0x00000000)));
	}

	/// Get just the fraction.
	///
	inline Fixed16 getFraction() const {
		return Fixed16(static_cast<Type>(_value & 0x0000ffffu));
	}
	
	/// Get the minimum from this and the other value.
	///
	inline Fixed16 getMinimum(const Fixed16 &other) const {
		return Fixed16((_value < other._value) ? _value : other._value);
	}
	
	/// Get the maximum from this and the other value.
	///
	inline Fixed16 getMaximum(const Fixed16 &other) const {
		return Fixed16((_value > other._value) ? _value : other._value);
	}
	
	/// Clamp this value to the selected range.
	///
	inline Fixed16 getClamped(const Fixed16 &minimum, const Fixed16 &maximum) const {
		return (*this).getMinimum(maximum).getMaximum(minimum);
	}

public: // Conversions
	/// Access the raw value.
	///
	inline Type toRawValue() const { return _value; }

	/// Get the integer part of this value.
	///
	inline int16_t toRawInteger() const { return (_value >> 16); }
		
	/// Get the fraction part of this value.
	///
	inline uint16_t toRawFraction() const {
		return static_cast<uint16_t>(_value & 0x0000ffffu);
	}

	/// Get a float value.
	///
	float toFloat() const;

public: // String conversion and display.
    /// Get the number of digits of the integer part.
    ///
    /// ```
    /// 0.1 => 1
    /// 1.23 => 1
    /// 12 => 2
    /// 12.234 => 2
    /// 1293 => 4
    /// ```
    ///
    uint8_t getIntegerDigitCount() const;

    /// Convert the value into a string.
    ///
    /// The buffer needs to have enough space for the maximum
    /// possible number and all fraction parts plus the null
    /// character to mark the end of the string. E.g 12 characters
    /// should cover every possible situation. This is maximum
    /// 6 characters for the integer part, the decimal point,
    /// and maximum 4 fraction digits plus the zero end mark.
    ///
    /// @param[out] buffer The buffer to write the string into.
    /// @param[in] fractionDigits The number of fraction digits. 0-4.
    ///
    void toString(char *buffer, uint8_t fractionDigits) const;

protected:
	Type _value;
};


}


