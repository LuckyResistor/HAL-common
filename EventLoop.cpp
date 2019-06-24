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
#include "EventLoop.hpp"


#include "InterruptLock.hpp"
#include "Timer.hpp"


namespace lr {



EventLoop *gMainEventLoop = nullptr;


EventLoop::EventLoop()
    : _interruptFlags(0), _exitRequested(false)
{
    gMainEventLoop = this;
}


void EventLoop::loop()
{
    while (!_exitRequested) {
        processEvent();
        Timer::delay(1_ms);
    }
}


void EventLoop::addImmediateEvent(Event::Function fn, bool merge)
{
    addEvent(Event(Event::Type::Immediate, fn, 0), merge);
}


void EventLoop::addPollEvent(Event::Function fn)
{
    addEvent(Event(Event::Type::Poll, fn, 0), false);
}


void EventLoop::addDelayedEvent(Event::Function fn, const Milliseconds delay, bool merge)
{
    const uint32_t data = (Timer::tickMilliseconds()+delay).ticks();
    addEvent(Event(Event::Type::Delayed, fn, data), merge);
}


void EventLoop::addDelayedEvent(Event::Function fn, const Seconds delay, bool merge)
{
    const uint32_t data = (Timer::tickMilliseconds().ticks()+(delay.ticks()*static_cast<uint32_t>(1000)));
    addEvent(Event(Event::Type::Delayed, fn, data), merge);
}


void EventLoop::addInterruptEvent(Event::Function fn, const uint32_t mask, bool merge)
{
    addEvent(Event(Event::Type::OnInterrupt, fn, mask), merge);
}


void EventLoop::processEvent()
{
    // The process position.
    uint8_t position = 0;
    // The process count, this copy is required prevent endless event loops.
    uint8_t count = eventCount();
    // The current time and interrupt flags for a processed events.
    const uint32_t currentTime = Timer::tickMilliseconds().ticks();
    const uint32_t interruptFlags = getAndClearInterruptFlags();
    while (position < count) {
        const auto &event = eventAt(position);
        if (event.isReady(currentTime, interruptFlags)) {
            // Keep a copy which is valid after removing the event.
            const auto eventCopy = event;
            // Remove the event first, to prevent merge issues.
            if (eventCopy.isRemovedAfterCall()) {
                removeEventAt(position);
                --count;
            } else {
                ++position;
            }
            // At this point, in the `call`, new events may be added to the event queue.
            eventCopy.call();
        } else {
            ++position;
        }
    }
}


void EventLoop::setInterrupt(const uint32_t mask)
{
    _interruptFlags |= mask;
}


uint32_t EventLoop::getAndClearInterruptFlags()
{
    InterruptLock lock;
    auto flagsCopy = _interruptFlags;
    _interruptFlags = 0;
    return flagsCopy;
}


void EventLoop::exit()
{
    _exitRequested = true;
}


}

