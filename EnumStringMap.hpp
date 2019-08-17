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


#include <cstring>


namespace lr {


/// A class to handle enum to string maps.
///
/// The idea is to provide a simple wrapper class around static enum to string maps. The lookup of the enum values
/// is slow and only suitable for a small number of strings (~ <100). The benefit is the flexibility of the map,
/// where reordering and the actual values of the Enums does not matter.
///
/// **Usage:**
/// ```
/// const EnumStringMap<MyEnum>({
///     {MyEnum::A, "A"},
///     {MyEnum::B, "B"},
///     {MyEnum::C, "C"},
///     {MyEnum::A, nullptr}, // end mark
/// });
/// ```
///
/// The enum of the last entry does not matter. The `nullptr` as string will mark the last entry in the map.
///
/// The method `string()` will always use the last entry from the map if the enum value is not found.
/// Therefore you can add a default value for not mapped entries in the last position:
///
/// ```
/// const EnumStringMap<MyEnum>({
///     {MyEnum::A, "A"},
///     {MyEnum::B, "B"},
///     {MyEnum::C, "C"},
///     {MyEnum::A, "Unknown"}, // <-- last value
///     {MyEnum::A, nullptr}, // end mark
/// });
/// ```
///
/// This also works with the `value()` method, which will return the enum of the last value if the string
/// does not match:
///
/// ```
/// const EnumStringMap<MyEnum>({
///     {MyEnum::First, "A"},
///     {MyEnum::Second, "B"},
///     {MyEnum::Unknown, ""}, // <-- last value, default for `value()`
///     {MyEnum::Unknown, nullptr}, // end mark
/// });
/// ```
///
template<typename Enum>
class EnumStringMap
{
public:
    /// A single entry in the map.
    ///
    struct Entry {
        Enum value; ///< The enum value to map to a string.
        const char* const str; ///< The string for the enum value.
    };
    
public:
    /// Create a new instance of the enum string map.
    ///
    /// @param map The static map with the strings.
    ///
    constexpr EnumStringMap(const Entry* const map) noexcept
        : _map(map)
    {
    }
    
    /// Get the string for a given enum value.
    ///
    /// @param value The enum value to map.
    /// @return A pointer to the string for the entry.
    ///    If the value is not found, the last string is used.
    ///
    const char* string(const Enum value) const noexcept {
        const Entry *entry = _map;
        while (entry->str != nullptr) {
            if (entry->value == value) {
                return entry->str;
            }
            ++entry;
        }
        --entry;
        return entry->str;
    }

    /// Get the enum value for a given string.
    ///
    Enum value(const char *str) const noexcept {
        const Entry *entry = _map;
        while (entry->str != nullptr) {
            if (std::strcmp(entry->str, str) == 0) {
                return entry->value;
            }
            ++entry;
        }
        --entry;
        return entry->value;
    }

private:
    const Entry* const _map; ///< The assigned map.
};


}

