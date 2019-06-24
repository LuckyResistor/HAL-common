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


namespace lr {


Event::Event()
    : _call(), _data(0), _type(Type::Invalid)
{
}


Event::Event(const Type type, const Function &call, const uint32_t data)
    : _call(call), _data(data), _type(type)
{
}


bool Event::canMerge(const Event& other) const
{
    return (_call == other._call) && (_type == other._type);
}


bool Event::isReady(const uint32_t currentTime, const uint32_t interruptFlags) const
{
    if (_type == Type::Immediate || _type == Type::Poll) {
        return true;
    } else if (_type == Type::Delayed) {
        // If the subtraction overflows, the delay is done.
        return ((_data - currentTime) & static_cast<uint32_t>(0x80000000ul)) != 0;
    } else if (_type == Type::OnInterrupt) {
        return (_data & interruptFlags) != 0;
    }
    return false;
}


Event::Type Event::type() const
{
    return _type;
}


uint32_t Event::data() const
{
    return _data;
}


void Event::call() const
{
    _call();
}


bool Event::isValid() const
{
    return _type != Type::Invalid;
}


bool Event::isRemovedAfterCall() const
{
    return _type != Type::Poll;
}


}



