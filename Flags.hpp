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
#include <type_traits>
#include <initializer_list>


namespace lr {


/// A flag type template.
///
/// This class supports a wide range of flag operations in combination
/// with the used enum for the bit masks.
///
/// @tparam Enum The enum type to use for the flags.
///
template<class Enum>
class EnumFlags
{
public:
    /// The enum type for this flags class.
    ///
    typedef Enum EnumType;

    /// The type used to store the mask for the flags.
    ///
    typedef typename std::underlying_type<Enum>::type FlagsType;

public:
    static_assert(std::is_enum<Enum>::value,
        "The Enum parameter needs to be a enum defining the bit masks.");
    static_assert(std::is_integral<FlagsType>::value,
        "The enum type needs to be a uint8_t, uint16_t, uint32_t or uint64_t.");
    static_assert(std::is_unsigned<FlagsType>::value,
        "The enum type needs to be an unsigned integer.");

public:
    /// Create a flags value with no flags set.
    ///
    constexpr inline EnumFlags() noexcept
        : _flags(0)
    {
    }
    
    /// Create a flags value with the given flag set.
    ///
    /// @param flag The flag to convert into flags.
    ///
    constexpr inline EnumFlags(Enum flag) noexcept
        : _flags(static_cast<FlagsType>(flag))
    {
    }
    
    /// Create a flags value from an initializer list.
    ///
    /// @param flags An initializer list with flags.
    ///
    constexpr inline EnumFlags(std::initializer_list<Enum> flags) noexcept
        : _flags(initializer_list_helper(flags.begin(), flags.end()))
    {
    }

    /// Create a flags value from a bitmask.
    ///
    constexpr inline explicit EnumFlags(const FlagsType mask) noexcept
        : _flags(mask)
    {
    }

public: // Operators
    /// Combine this flags value with another value.
    ///
    inline EnumFlags& operator|=(EnumFlags other) noexcept { _flags |= other._flags; return *this; }

    /// Combine this flags value with another value.
    ///
    inline EnumFlags& operator|=(Enum other) noexcept { _flags |= static_cast<FlagsType>(other); return *this; }

    /// Combine this flags value with another value.
    ///
    inline EnumFlags& operator&=(EnumFlags other) noexcept { _flags &= other._flags; return *this; }

    /// Combine this flags value with another value.
    ///
    inline EnumFlags& operator&=(Enum other) noexcept { _flags &= static_cast<FlagsType>(other); return *this; }

    /// Combine this flags value with another value.
    ///
    inline EnumFlags& operator^=(EnumFlags other) noexcept { _flags ^= other._flags; return *this; }

    /// Combine this flags value with another value.
    ///
    inline EnumFlags& operator^=(Enum other) noexcept { _flags ^= static_cast<FlagsType>(other); return *this; }

    /// Combine this flags value with another value.
    ///
    constexpr inline EnumFlags operator|(EnumFlags other) const noexcept { return EnumFlags(_flags|other._flags); }

    /// Combine this flags value with another value.
    ///
    constexpr inline EnumFlags operator|(Enum other) const noexcept { return EnumFlags(_flags|static_cast<FlagsType>(other)); }

    /// Combine this flags value with another value.
    ///
    constexpr inline EnumFlags operator&(EnumFlags other) const noexcept { return EnumFlags(_flags&other._flags); }

    /// Combine this flags value with another value.
    ///
    constexpr inline EnumFlags operator&(Enum other) const noexcept { return EnumFlags(_flags&static_cast<FlagsType>(other)); }

    /// Combine this flags value with another value.
    ///
    constexpr inline EnumFlags operator^(EnumFlags other) const noexcept { return EnumFlags(_flags^other._flags); }

    /// Combine this flags value with another value.
    ///
    constexpr inline EnumFlags operator^(Enum other) const noexcept { return EnumFlags(_flags^static_cast<FlagsType>(other)); }

    /// Negate this flags value.
    ///
    constexpr inline EnumFlags operator~() const noexcept { return EnumFlags(~_flags); }

    /// Check if two flags are equal.
    ///
    constexpr inline bool operator==(const EnumFlags &other) const noexcept { return _flags == other._flags; }

    /// Check if two flags are not equal.
    ///
    constexpr inline bool operator!=(const EnumFlags &other) const noexcept { return _flags != other._flags; }

    /// Convert this flags value into a bit mask.
    ///
    constexpr inline operator FlagsType() const noexcept { return _flags; }

public: // Helper methods.
    /// Check if a flag is set.
    ///
    /// @param flag The flag to test.
    /// @return `true` if the flag is set, `false` if the flag is not set.
    ///
    constexpr inline bool isSet(Enum flag) const noexcept {
        return (_flags&static_cast<FlagsType>(flag))==static_cast<FlagsType>(flag)&&(static_cast<FlagsType>(flag)!=0||_flags==static_cast<FlagsType>(flag));
    }

    /// Check if one flag is set.
    ///
    constexpr inline bool isOneSet() const noexcept {
        return _flags != 0;
    }

    /// Set a flag.
    ///
    /// @param flag The flag to set.
    ///
    inline void setFlag(Enum flag) noexcept {
        _flags |= static_cast<FlagsType>(flag);
    }

    /// Change a number of flags using a mask.
    ///
    /// @param[in] flags The flags which shall be set.
    ///    All other masked flags will be cleared.
    /// @param[in] mask The mask of all changed flags.
    ///
    inline void changeFlags(EnumFlags flags, EnumFlags mask) {
        _flags = ((_flags & ~mask) | flags);
    }

    /// Clear a flag.
    ///
    /// @param flag The flag to set.
    ///
    inline void clearFlag(Enum flag) noexcept {
        _flags &= (~static_cast<FlagsType>(flag));
    }

    /// Clear all flags.
    ///
    inline void clearAll() noexcept {
        _flags = 0;
    }

    /// Create flags from a bit mask.
    ///
    /// Helper method to make code converting bit masks into
    /// a EnumFlags object more readable.
    ///
    /// @param mask The bit mask.
    /// @return The flags object for the mask.
    ///
    inline constexpr static EnumFlags fromMask(const FlagsType mask) noexcept {
        return EnumFlags(mask);
    }

private:
    /// Helper to convert an initializer list with flags into a bitmask.
    ///
    /// The idea of this recursive helper function is to let the compiler solve
    /// the bit mask at compile time.
    ///
    constexpr static inline FlagsType initializer_list_helper(
        typename std::initializer_list<Enum>::const_iterator it,
        typename std::initializer_list<Enum>::const_iterator end) noexcept
    {
        return (it == end ? static_cast<FlagsType>(0) : (static_cast<FlagsType>(*it)|initializer_list_helper(it+1, end)));
    }
    
private:
    FlagsType _flags; ///< The flags.
};


/// Declare a new flags type.
///
/// This macro needs to be places just after the enum declaration.
/// The enum has to be a typed one and define each value as bit mask.
///
/// @param flagEnum The name of the enum to use. E.g `Channel`.
/// @param flagsName The name of the resulting flags type. E.g. `Channels`.
///
#define LR_DECLARE_FLAGS(FlagEnum, FlagsName) \
using FlagsName = ::lr::EnumFlags<FlagEnum>;

/// Declare operators for the flags.
///
/// This declares additional operatos for the flags to be combined.
///
#define LR_DECLARE_OPERATORS_FOR_FLAGS(FlagsName) \
constexpr inline ::lr::EnumFlags<FlagsName::EnumType> operator|(FlagsName::EnumType a, FlagsName::EnumType b) noexcept { \
    return ::lr::EnumFlags<FlagsName::EnumType>(a)|b; }


}
