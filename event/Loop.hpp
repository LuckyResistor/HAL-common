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


#include "Entry.hpp"
#include "hal-common/Timer.hpp"
#include "hal-common/InterruptLock.hpp"

#include <cstdint>


namespace lr {
namespace event {


/// The abstract interface to access the event loop through the whole application.
///
class Loop
{
public:
    /// The constructor, initializing the main instance pointer.
    ///
    Loop();

public:
    /// Exit this event loop.
    ///
    /// Calling this method will exit this event loop at the
    /// next possible time.
    ///
    virtual void exit() = 0;

    /// Add an immediate event.
    ///
    /// This event will be executed as soon as possible from the main loop.
    ///
    /// @param fn The function to execute for the event.
    /// @param merge Merge similar events. If the same event call is added several times, only the last is executed.
    ///
    virtual void addImmediateEvent(Entry::Function fn, bool merge = false) = 0;

    /// Add a repeated polling event at the speed of the loop.
    ///
    /// This event is called each time the event loop is checked for new events. This happens approximate
    /// every 1 milliseconds, unless the loop gets blocked for a longer time.
    ///
    /// @param fn The function to execute for the event.
    ///
    virtual void addPollEvent(Entry::Function fn) = 0;

    /// Add a single delayed event.
    ///
    /// This event will be executed after the specified delay in milliseconds once.
    ///
    /// @param fn The function to execute for the event.
    /// @param delay The delay in milliseconds. Use delays up to a minute.
    /// @param merge Merge similar events. If the same event call is added several times, only the last is executed.
    ///
    virtual void addDelayedEvent(Entry::Function fn, Milliseconds delay, bool merge = false) = 0;

    /// Add a repeated event which is called infinite at the given interval.
    ///
    /// @param fn The function to execute for the event.
    /// @param delay The delay in milliseconds which is also the interval. This value is limited
    ///              to the range of 1-32767 milliseconds. The behaviour for other values is undefined.
    ///
    virtual void addRepeatedEvent(Entry::Function fn, Milliseconds delay) = 0;

    /// Add an event on interrupt.
    ///
    /// This event will be executed if one of the interrupt flags was set.
    /// It will be executed once, unless `repeat` is set to `true`.
    ///
    /// @param fn The function to execute for the event.
    /// @param interruptFlags The interrupt flags to react to.
    /// @param repeat If this is set to `true`, the event is executed every time the interrupt flags are set.
    ///
    virtual void addInterruptEvent(Entry::Function fn, event::InterruptFlags interruptFlags, bool repeat = false) = 0;

    /// Set an interrupt flag.
    ///
    /// Call this from an interrupt function.
    ///
    /// @param mask The mask with the interrupt bits to set.
    ///
    virtual void signalInterrupt(event::InterruptFlags interruptFlags) = 0;

public:
    /// Access the main event loop.
    ///
    static Loop& main();

private:
    static Loop *_main; ///< The main instance of the event loop.
};


/// Access the main loop.
///
inline Loop& mainLoop() {
    return Loop::main();
}


/// This is a static implementation of the entry storage.
///
/// It uses a static array for the entry list to avoid dynamic memory allocation.
///
template<uint8_t entryListSize = 32>
class StaticStorage
{
public:
    /// Create a new empty storage instance.
    ///
    constexpr StaticStorage() : _count(0), _index(0) {}

public: // Implement all required storage methods
    /// Add an entry to the current storage.
    ///
    /// @param entry The entry to add.
    /// @param merge If the entry shall be merged with existing ones.
    ///
    void addEntry(const Entry &entry, bool merge) {
        if (_count == entryListSize) {
            return; // Skip new entries if the list is full.
        }
        if (merge && _count > 0) {
            // Search for entries which can be merged.
            for (uint8_t i = 0; i < _count; ++i) {
                if (entry.canMerge(_entryList[i])) {
                    // Just ignore this, if we found one.
                    return;
                }
            }
        }
        _entryList[_count] = entry;
        _count += 1;
    }

    /// Get the current number of entries.
    ///
    /// @return The current number of entries.
    ///
    inline uint8_t getCount() const {
        return _count;
    }

    /// Access a entry at the given position.
    ///
    /// @param position The position of the entry to access.
    /// @return A reference to the accessed entry.
    ///
    inline Entry& entryAt(const uint8_t position) {
        static Entry invalidEvent;
        if (position >= _count) {
            return invalidEvent;
        }
        return _entryList[position];
    }

    /// Remove an entry at the given position.
    ///
    /// @param position The position of the entry to remove.
    ///
    inline void removeEntryAt(const uint8_t position) {
        if (position >= _count) {
            return;
        }
        // To delete the last element, just decrease the count.
        const auto lastElement = (_count - 1);
        if (position < lastElement) {
            for (uint8_t i = position; i < lastElement; ++i) {
                _entryList[i] = _entryList[i + 1];
            }
        }
        _count -= 1;
    }

private:
    uint8_t _count; ///< The number of entries in the list.
    uint8_t _index; ///< The index of the first entry in the list.
    Entry _entryList[entryListSize]; ///< The list of entries.
};


/// A basic event loop for embedded applications.
///
/// This basic event loop system is designed for embedded applications.
/// This event loop can not be used directly from an interrupt.
///
template<typename StorageClass>
class BasicLoop : public Loop
{
public:
    /// Create a new event loop.
    ///
    constexpr BasicLoop() : _interruptFlags(0), _exitRequested(false), _storage() {}

public:
    /// Process the events once.
    ///
    /// Call this function from an existing `loop()` method. It will wait for the next tick and
    /// process all events once. It will allow support code for a platform to be run outside of
    /// the `loop()` call.
    ///
    inline void loopOnce() {
        Timer::waitForNextTick();
        processEvents();
    }

    /// Enter the event loop until stopped.
    ///
    /// This will process events until the loop is stopped using the `exit()` function.
    ///
    inline void loopInfinite() {
        while (!_exitRequested) {
            loopOnce();
        }
    }

    /// Process all event once.
    ///
    /// This will process the current event list once.
    ///
    void processEvents() {
        // The process position.
        uint8_t position = 0;
        // The process count, this copy is required prevent endless event loops.
        uint8_t count = _storage.getCount();
        // The current time and interrupt flags for a processed events.
        const auto currentTime = Timer::tickMilliseconds();
        const auto interruptFlags = getAndClearInterruptFlags();
        while (position < count) {
            auto &event = _storage.entryAt(position);
            if (event.isReady(currentTime, interruptFlags)) {
                // Keep a copy of the call which is valid after removing the event.
                const auto call = event.getCall();
                // Remove the event first, to prevent merge issues.
                if (event.isRemovedAfterCall()) {
                    _storage.removeEntryAt(position);
                    --count;
                } else {
                    event.updateExpireTime(currentTime);
                    ++position;
                }
                // At this point, in the `call`, new events may be added to the event queue.
                call();
            } else {
                ++position;
            }
        }
    }

public: // Implement Loop
    void exit() override {
        _exitRequested = true;
    }

    void addImmediateEvent(Entry::Function fn, bool merge) override {
        const Entry::Flags flags = Entry::Flag::Valid | Entry::Flag::Immediate;
        _storage.addEntry(Entry(flags, fn, 0_ms), merge);
    }

    void addPollEvent(Entry::Function fn) override {
        const auto flags = Entry::Flag::Valid | Entry::Flag::Immediate | Entry::Flag::Repeat;
        _storage.addEntry(Entry(flags, fn, 0_ms), false);
    }

    void addDelayedEvent(Entry::Function fn, Milliseconds delay, bool merge) override {
        const auto flags = Entry::Flag::Valid;
        const auto expireTime = Timer::tickMilliseconds() + delay;
        _storage.addEntry(Entry(flags, fn, expireTime), merge);
    }

    void addRepeatedEvent(Entry::Function fn, Milliseconds delay) override {
        const auto flags = Entry::Flag::Valid | Entry::Flag::Repeat;
        const auto intervalMs = static_cast<uint16_t>(delay.ticks());
        const uint16_t expireTimeMs = static_cast<uint16_t>(Timer::tickMilliseconds().ticks()) + intervalMs;
        _storage.addEntry(Entry(flags, fn, expireTimeMs, intervalMs), false);
    }

    void addInterruptEvent(Entry::Function fn, event::InterruptFlags interruptFlags, bool repeat) override {
        auto flags = Entry::Flag::Valid | Entry::Flag::OnInterrupt;
        if (repeat) {
            flags |= Entry::Flag::Repeat;
        }
        _storage.addEntry(Entry(flags, fn, interruptFlags), false);
    }

    void signalInterrupt(event::InterruptFlags interruptFlags) override {
        _interruptFlags |= static_cast<uint16_t>(interruptFlags);
    }

private:
    /// Get the current interrupt flags and clear them.
    ///
    InterruptFlags getAndClearInterruptFlags() {
        InterruptLock lock;
        const auto flagsCopy = _interruptFlags;
        _interruptFlags = 0;
        return InterruptFlags(flagsCopy);
    }

private:
    volatile uint16_t _interruptFlags; ///< The current interrupt flags.
    bool _exitRequested; ///< Flag is exit of this event loop was requested.
    StorageClass _storage; ///< The implementation of the entry storage.
};


}
}

