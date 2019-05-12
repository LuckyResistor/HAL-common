#pragma once
//
// Status tools
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


#include <cstdint>
#include <type_traits>


namespace lr {


/// A simple call status enumeration.
///
enum class CallStatus : uint8_t {
    Success, ///< The call was successful.
    Error ///< The call failed.
};


/// Check if the return status of a fucntion was successful.
///
/// @tparam Enum The enum which is used to test the status.
/// @param status The status.
/// @return `true` if the status matches the enum `Enum::Success`.
///
template<typename Enum>
constexpr inline bool isSuccessful(Enum status) noexcept {
    static_assert(std::is_enum<Enum>::value, "Enum status required.");
    return status == Enum::Success;
}

/// Check if there was an error with a function call.
///
/// @tparam Enum The enum which is used to test the status.
/// @param status The status.
/// @return `true` if the status *does not match* the enum `Enum::Success`.
///
template<typename Enum>
constexpr inline bool hasError(Enum status) noexcept {
    static_assert(std::is_enum<Enum>::value, "Enum status required.");
    return status != Enum::Success;
}


}


/// @file StatusTools.hpp
/// The *status tools* header declares two simple template methods
/// to test the status of any `enum` result which follows a simple
/// logic. The used enum has one element `Success` if the call
/// was successful, and other elements if there was an error.
///
/// Enum Requirements
/// -----------------
/// The templates work with any `enum class` like this:
/// ```
/// enum class Result : uint8_t {
///    Success, ///< The call was successful.
///    Failed ///< The call failed.
/// };
/// ```
/// In this simple case, you can reuse the call result declaration
/// in this header:
/// ```
/// using Result = lr::CallResult;
/// ```
/// You can use any number of failure states, as long there is
/// only one state indicate a successful call. You must not use the
/// call result to return actual information using the call result.
/// ```
/// enum class Result : uint8_t {
///    Success,
///    AddressError,
///    ParameterError,
///    NotFound
/// };
/// ```
///
/// Usage
/// -----
/// If you use this header from user code, outside of the `lr`
/// namespace, you may want to import the namespace or just the
/// functions into your namespace using:
///
/// - `using namespace lr;`
///
/// Or using:
///
/// - `using lr::isSuccessful;`
/// - `using lr::hasError;`
///
/// Now you use the functions to check for a successful call
/// like this:
/// ```
/// if (isSuccessful(Chip::setFunction(0xa5))) {
///     // Do something nice...
/// }
/// ```
/// Or you check for a failure like this:
/// ```
/// if (hasError(Chip::setFunction(0xa5))) {
///     // handle the error case...
/// }
/// ```
/// To create the usual error handling chain, declare a
/// initial status variable:
/// ```
/// MyClass::Status MyClass::doSomething() {
///     Chip::Status status;
///     if (hasError(status = _chip.setFunctionA(0xa5)) {
///        // check `status` for details.
///        return Status::Error;
///     }
///     if (hasError(status = _chip.setFunctionB(0xb2)) {
///        // check `status` for details.
///        return Status::Error;
///     }
///     return Status::Success;
/// }
/// ```
///


