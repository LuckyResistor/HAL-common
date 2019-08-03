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

#include "hal-common/Duration.hpp"
#include "hal-common/Flags.hpp"

#include <cstdint>


namespace lr {
namespace event {


/// The interrupt flags.
///
enum InterruptFlag : uint16_t {
    InterruptA = (1u<<0u),
    InterruptB = (1u<<1u),
    InterruptC = (1u<<2u),
    InterruptD = (1u<<3u),
    InterruptE = (1u<<4u),
    InterruptF = (1u<<5u),
    InterruptG = (1u<<6u),
    InterruptH = (1u<<7u),
    InterruptI = (1u<<8u),
    InterruptJ = (1u<<9u),
    InterruptK = (1u<<10u),
    InterruptL = (1u<<11u),
    InterruptM = (1u<<12u),
    InterruptN = (1u<<13u),
    InterruptO = (1u<<14u),
    InterruptP = (1u<<15u),
};
LR_DECLARE_FLAGS(InterruptFlag, InterruptFlags);

/// The data format for single delayed events.
///
struct DelayedData {
    Milliseconds expireTime;
};


/// The data format for repeated events
///
struct RepeatedData {
    uint16_t expireTimeMs;
    uint16_t intervalMs;
};


/// The data for on interrupt data.
///
struct OnInterruptData {
    InterruptFlags interruptFlags;
};


/// The data for an event.
///
union Data {
    constexpr Data() : delayed({0_ms}) {}
    constexpr explicit Data(Milliseconds expireTime) : delayed({expireTime}) {}
    constexpr explicit Data(InterruptFlags interruptFlags) : onInterrupt({interruptFlags}) {}
    constexpr Data(uint16_t expireTimeMs, uint16_t intervalMs) : repeated({expireTimeMs, intervalMs}) {}
    DelayedData delayed;
    RepeatedData repeated;
    OnInterruptData onInterrupt;
};


}
}


LR_DECLARE_OPERATORS_FOR_FLAGS(lr::event::InterruptFlags);

