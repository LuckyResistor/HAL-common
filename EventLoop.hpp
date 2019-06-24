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


#include "Event.hpp"
#include "Timer.hpp"

#include <cstdint>


namespace lr {


// Access the global pointer to the main event loop.
class EventLoop;
extern EventLoop *gMainEventLoop;


/// A simple event loop for embedded applications.
///
/// This very simple event loop system is designed for embedded applications.
/// The default implementation creates a static event loop as main event
/// loop in the implementation file.
///
/// This event loop can not be used directly from an interrupt.
///
class EventLoop
{
public:
    /// Create a new event loop.
    ///
    EventLoop();

    /// The default dtor.
    ///
    virtual ~EventLoop() = default;
    
public:
    /// Enter the event loop.
    ///
    /// This will process events until the loop is stopped
    /// using the `exit()` function.
    ///
    void loop();

    /// Exit this event loop.
    ///
    /// Calling this method will exit this event loop at the
    /// next possible time.
    ///
    void exit();

    /// Add a immediate event.
    ///
    /// This event will be executed at the next time in the
    /// loop in the order the events were added.
    ///
    /// @param fn The function to execute for the event.
    /// @param merge Merge similar events.
    ///
    void addImmediateEvent(Event::Function fn, bool merge = false);

    /// Add a poll event.
    ///
    /// This event is called each time the event loop
    /// is checked for new events. This happens approximate
    /// every 1 milliseconds.
    ///
    /// @param fn The function to execute for the event.
    ///
    void addPollEvent(Event::Function fn);

    /// Add a delayed event.
    ///
    /// This event will be executed after the specified delay
    /// in milliseconds.
    ///
    /// @param fn The function to execute for the event.
    /// @param delay The delay in milliseconds. Use delays up to a minute.
    /// @param merge Merge similar events.
    ///
    void addDelayedEvent(Event::Function fn, Milliseconds delay, bool merge = false);

    /// Add a delayed event.
    ///
    /// This event will be executed after the specified delay
    /// in seconds.
    ///
    /// @param fn The function to execute for the event.
    /// @param delay The delay in seconds. Use delays up to a minute.
    /// @param merge Merge similar events.
    ///
    void addDelayedEvent(Event::Function fn, Seconds delay, bool merge = false);

    /// Add a event on interrupt.
    ///
    /// This event will be executed after _any_ of the interrupts
    /// was set defined in the mask.
    ///
    /// @param fn The function to execute for the event.
    /// @param mask The mask with the interrupt bits to react to.
    /// @param merge Merge similar events.
    ///
    void addInterruptEvent(Event::Function fn, uint32_t mask, bool merge = false);

    /// Process a single event.
    ///
    /// This will scan the current event list for the next matching event.
    /// This event is executed and removed from the list.
    ///
    void processEvent();

    /// Set an interrupt flag.
    ///
    /// Call this from a function to set one or more interrupt bits.
    /// The idea is to actually call this from an interrupt function.
    ///
    /// @param mask The mask with the interrupt bits to set.
    ///
    void setInterrupt(uint32_t mask);

protected:
    /// Add a new event.
    ///
    /// The event will be added to the end of the event list.
    /// If the event list is full, the event is ignored.
    ///
    /// @param event The event to add.
    /// @param merge If `true` similar events are "merged".
    ///
    virtual void addEvent(const Event &event, bool merge) = 0;

    /// Get the number of events in the event list.
    ///
    /// @return The current number of events in the list.
    ///
    virtual uint8_t eventCount() const = 0;
    
    /// Access the event at the given position in the list.
    ///
    /// @param position The position of the event to access.
    /// @return The event at the given position.
    ///
    virtual const Event& eventAt(uint8_t position) const = 0;

    /// Remove the event at the given position.
    ///
    /// @param position The position of the event to remove.
    ///
    virtual void removeEventAt(uint8_t position) = 0;
    
private:
    /// Get the current interrupt flags and clear them.
    ///
    uint32_t getAndClearInterruptFlags();
    
public:
    /// Access the main event loop.
    ///
    /// @return The main event loop.
    ///
    inline static EventLoop& main() {
        return *gMainEventLoop;
    }

private:
    volatile uint32_t _interruptFlags; ///< The current interrupt flags.
    bool _exitRequested; ///< Flag is exit of this event loop was requested.
};


/// This is a static implementation of the event loop.
///
/// It uses a static array for the event list to avoid dynamic memory allocation.
///
template<uint8_t eventListSize>
class StaticEventLoop : public EventLoop
{
public:
    /// Create a new empty static event loop.
    ///
    StaticEventLoop() : _count(0), _index(0) {
    }

public: // Implement EventLoop
    void addEvent(const Event &event, bool merge) override {
        if (_count == eventListSize) {
            return; // Skip new events if the list is full.
        }
        if (merge && _count > 0) {
            // Search for events which can be merged.
            for (uint8_t i = 0; i < _count; ++i) {
                if (event.canMerge(_eventList[i])) {
                    // Just ignore this, if we found one.
                    return;
                }
            }
        }
        _eventList[_count] = event;
        _count += 1;
    }

protected: // Implement EventLoop
    uint8_t eventCount() const override {
        return _count;
    }
    const Event& eventAt(const uint8_t position) const override {
        static Event invalidEvent;
        if (position >= _count) {
            return invalidEvent;
        }
        return _eventList[position];
    }
    void removeEventAt(const uint8_t position) override {
        if (position >= _count) {
            return;
        }
        // To delete the last element, just decrease the count.
        const auto lastElement = (_count-1);
        if (position < lastElement) {
            for (uint8_t i = position; i < lastElement; ++i) {
                _eventList[i] = _eventList[i+1];
            }
        }
        _count -= 1;
    }

private:
    uint8_t _count; ///< The number of events in the list.
    uint8_t _index; ///< The index of the first event in the list.
    Event _eventList[eventListSize]; ///< The list of events.
};


}

