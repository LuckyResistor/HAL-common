#pragma once
//
// Event Loop
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


#include <cstdint>


namespace lr {


/// A single event for the event loop.
///
class Event
{
public:
    /// The event type.
    ///
    enum class Type : uint8_t {
        Invalid = 0, ///< Invalid event.
        Immediate, ///< Immediate event.
        Poll, ///< A poll event. Never removed, called every time.
        Delayed, ///< Delayed event.
        OnInterrupt, ///< Wait for an interrupt.
    };

public:
    /// The event function.
    ///
    typedef void (*Function)();

public:
    /// Create an empty invalid event.
    ///
    Event();

    /// Create a event.
    ///
    /// @param type The type of the event.
    /// @param call The call for the event.
    /// @param data The data for the event.
    ///
    Event(Type type, const Function &call, uint32_t data);

    /// Copy the event.
    ///
    Event(const Event&) = default;

    /// Assign the event.
    ///
    Event& operator=(const Event&) = default;

public:
    /// Get the type of this event.
    ///
    Type type() const;

    /// Get the data for this event.
    ///
    uint32_t data() const;

    /// Call the event function.
    ///
    void call() const;

    /// Check if this is a valid event.
    ///
    bool isValid() const;

    /// Check if this event can be merged with another one.
    ///
    /// @param other The other event to compare.
    /// @return `true` if this event can be merged with the other one.
    ///
    bool canMerge(const Event& other) const;

    /// Check if this event is ready for execution.
    ///
    /// @param currentTime The current time in millisecond ticks.
    /// @param interruptFlags The current interrupt flags.
    /// @return `true` if this event is ready to exeute.
    ///
    bool isReady(uint32_t currentTime, uint32_t interruptFlags) const;

    /// If this event shall be removed after the call.
    ///
    bool isRemovedAfterCall() const;

private:
    Function _call; ///< The call for this event.
    uint32_t _data; ///< The data for the event. Depends on the event type.
    Type _type; ///< The event type.
};


}

