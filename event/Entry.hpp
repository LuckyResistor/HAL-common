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


#include "Data.hpp"

#include "../BitTools.hpp"


namespace lr {
namespace event {


/// A single entry for the event loop.
///
class Entry
{
public:
    /// The flags for the entry
    ///
    enum Flag : uint8_t {
        None = 0,
        Valid = oneBit8(0), ///< Flag set for all valid events.
        Immediate = oneBit8(1), ///< Flag set if the event happens each time.
        OnInterrupt = oneBit8(2), ///< Flag set if the event happens if an interrupt flag is set.
        Repeat = oneBit8(3), ///< Flag set if the event is repeated in a given interval or after an interrupt.
    };
    LR_DECLARE_FLAGS(Flag, Flags);

public:
    /// The event function.
    ///
    typedef void (*Function)();

public:
    /// Create an empty invalid event.
    ///
    constexpr Entry() : _call(), _flags()
    {}


    /// Create a delayed event.
    ///
    /// @param flags The flags for the event.
    /// @param call The call for the event.
    /// @param expireTime The expire time of the event in milliseconds.
    ///
    constexpr Entry(Flags flags, const Function &call, Milliseconds expireTime)
        : _call(call), _data(expireTime), _flags(flags)
    {}


    /// Create a repeated event.
    ///
    /// @param flags The flags for the event.
    /// @param call The call for the event.
    /// @param expireTimeMs The expire time of the event in milliseconds.
    /// @param intervalMs The interval time in milliseconds.
    ///
    constexpr Entry(Flags flags, const Function &call, uint16_t expireTimeMs, uint16_t intervalMs)
        : _call(call), _data(expireTimeMs, intervalMs), _flags(flags)
    {}


    /// Create a on-interrupt event.
    ///
    /// @param flags The flags for the event.
    /// @param call The call for the event.
    /// @param interruptFlags The interrupt flags to trigger the event.
    ///
    constexpr Entry(Flags flags, const Function &call, event::InterruptFlags interruptFlags)
        : _call(call), _data(interruptFlags), _flags(flags)
    {}


    /// Copy the event.
    ///
    Entry(const Entry &) = default;

    /// Assign the event.
    ///
    Entry &operator=(const Entry &) = default;

public:
    /// Check if this event can be merged with another one.
    ///
    /// @param other The other event to compare.
    /// @return `true` if this event can be merged with the other one.
    ///
    inline bool canMerge(const Entry &other) const
    {
        return (_call == other._call) && (_flags == other._flags);
    }


    /// Check if this event is ready for execution.
    ///
    /// @param currentTime The current time in millisecond ticks.
    /// @param interruptFlags The current interrupt flags.
    /// @return `true` if this event is ready to exeute.
    ///
    inline bool isReady(Milliseconds currentTime, event::InterruptFlags interruptFlags) const
    {
        if (!_flags.isSet(Flag::Valid)) {
            return false;
        }
        if (_flags.isSet(Flag::Immediate)) {
            return true;
        }
        if (_flags.isSet(Flag::OnInterrupt)) {
            return (_data.onInterrupt.interruptFlags & interruptFlags).isOneSet();
        } else if (_flags.isSet(Flag::Repeat)) {
            const auto currentTime16 = static_cast<uint16_t>(currentTime.ticks());
            const auto delta = static_cast<int16_t>(_data.repeated.expireTimeMs - currentTime16);
            return delta <= 0;
        } else {
            return currentTime.deltaTo(_data.delayed.expireTime) <= 0;
        }
    }


    /// If this event shall be removed after the call.
    ///
    inline bool isRemovedAfterCall() const
    {
        return !_flags.isSet(Flag::Repeat);
    }


    /// Update the expire time.
    ///
    inline void updateExpireTime(Milliseconds currentTime)
    {
        if (!_flags.isSet(Flag::Immediate)) {
            const auto currentTime16 = static_cast<uint16_t>(currentTime.ticks());
            _data.repeated.expireTimeMs = currentTime16 + _data.repeated.intervalMs;
        }
    }


    /// Check if this is a valid event.
    ///
    inline bool isValid() const
    {
        return _flags.isSet(Flag::Valid);
    }


    /// Get the call for the event.
    inline Function getCall() const
    {
        return _call;
    }


private:
    Function _call; ///< The call for this event.
    event::Data _data; ///< The data for this event.
    Flags _flags; ///< The flags for the event.
};


LR_DECLARE_OPERATORS_FOR_FLAGS(Entry::Flags);


}
}



