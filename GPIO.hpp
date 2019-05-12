#pragma once
//
// The GPIO abstraction layer
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


#include "StatusTools.hpp"

#undef min
#undef max
#include <cstdint>
#include <limits>


/// @namespace lr::GPIO
/// This is a simple abstraction layer to access the GPIO of the used MCU.
///
/// The pin numbering depends on the implementation.
/// The initialization depends on the implementation.
///
/// This abstraction layer is very simple to allow low performance IO
/// operations from driver libraries. E.g. operating a reset line.
///
/// Advanced configuringation like perpherals has do be done directly
/// on the platform.
///


namespace lr {
namespace GPIO {


/// The pin mode.
///
enum class Mode : uint8_t {
    Input, ///< Configure the pin to act as input.
    HighImpendance, ///< Configure the pin into high impendance mode.
    High, ///< Pull the pin to VCC.
    Low ///< Pull the pin to GND.
};

/// The pull up/down configuration.
///
enum class Pull : uint8_t {
    None, ///< No pull up or down.
    Up, ///< Enable the pull up resistor.
    Down, ///< Enable the pull down resistor.
};

/// The status of function calls.
///
using Status = CallStatus;


/// The type for the pin number.
///
typedef uint8_t PinNumber;

/// A constant defining "no pin" assigned.
///
/// This constant is used in drivers to signal there is no pin
/// assigned.
///
const PinNumber cNoPin = std::numeric_limits<PinNumber>::max();


/// Initialize the GPIO layer.
///
/// @return 'Success' if the initialization was successful.
/// 
Status initialize();

/// Configure the pin mode.
///
/// @param pin The number of the pin to configure.
/// @param mode The mode for the pin.
/// @param pull The mode for the pull up/down resistors.
/// @return `Success` if the mode could be successfully set.
///
Status setMode(PinNumber pin, Mode mode, Pull pull = Pull::None);

/// Read the digital input from the pin.
///
/// @param pin The number of the pin to read.
/// @return `true` if the pin is in high state, `false` if the pin
///   is in low state.
///
bool getState(PinNumber pin);


}
}
