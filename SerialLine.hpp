#pragma once
//
// Serial Line
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


#include "String.hpp"
#include "StatusTools.hpp"

#include <cstdint>


namespace lr {

    
/// The interface for serial line implementations.
///
/// This is an interface for common serial lines for writing and reading binary data.
///
/// Rules for all implementations:
/// - A call must never block.
/// - If data can not be sent, this shall result in an error.
///
/// Design notes: Many APIs try to create one code IO abstraction interface
/// to handle all kinds of IO operations. The problem with this approach is
/// the different timing requirements for different kind of interfaces.
/// This interface is is a low level interface to create an abstract access layer
/// to all kinds of serial interfaces. All implementation details shall go into
/// the implementation interface. Any advanced input/output handling should be
/// implemented as a wrapper to to this interface.
///
/// All API methods with a `Status` return value can return `Error` in case there
/// is a problem with the underlying device.
///
class SerialLine
{
public:
    /// The type to specify data sizes.
    ///
    using DataSize = uint16_t;
    
    /// The status of a call.
    ///
    enum class Status : uint8_t {
        Success, ///< The call was successful.
        Partial, ///< Not all data would be sent or received.
        Error, ///< Unspecific error, see API for details.
    };
    
public: // Output
    /// Get the number of bytes which can be sent.
    ///
    /// The returned number of bytes can be sent in one call of the `send`
    /// method. If you try to send more bytes, you will get `Partial` and
    /// exceeding bytes are ignored.
    ///
    /// @return The number of bytes available to send.
    ///
    virtual DataSize sendBytesAvailable() noexcept = 0;
    
    /// Send a single byte to the serial line.
    ///
    /// @param[in] value The byte to send.
    /// @return `Success`, `Partial` or `Error`.
    ///
    virtual Status send(uint8_t value) noexcept = 0;
    
    /// Send a number of bytes to the serial line.
    ///
    /// @param[in] data A pointer to the data to be sent.
    /// @param[in] dataSize The size of the data block to send.
    /// @param[out] dataSent If this pointer is passed to the function, the
    ///     number of actually sent bytes is resturned. In this case the status
    ///     `Partial` is returned.
    /// @return `Success`, `Partial` or `Error`.
    ///
    virtual Status send(
        const uint8_t *data,
        DataSize dataSize,
        DataSize *dataSent = nullptr) noexcept = 0;
    
    /// Reset the send buffer.
    ///
    /// This will reset the send buffer, dropping all data which is waiting
    //  to be sent.
    ///
    /// @return `Success` or `Error`.
    ///
    virtual Status sendReset() noexcept = 0;
    
    /// Get the number of bytes available to read with one call.
    ///
    /// @return The number of bytes available to read.
    ///
    virtual DataSize receiveBytesAvailable() noexcept = 0;
    
    /// Receive a single byte from the serial line.
    ///
    /// @param[out] value The read byte.
    /// @return `Success`, `Partial` or `Error`.
    ///
    virtual Status receive(uint8_t &value) noexcept = 0;
    
    /// Receive a number of bytes from the serial line.
    ///
    /// This function tries to read the number of bytes passed in the given
    /// time frame. If not all data could be sent, the number of sent bytes are
    /// stored in variable `dataReceived`. To ignore the numnber of bytes sent,
    /// pass `nullptr` to this parameter.
    ///
    /// @param[out] data A pointer to a buffer to receive the data.
    /// @param[in] dataSize The maximum number of bytes to receive.
    /// @param[out] dataReceived If this pointer to a variable is passed to the
    ///      function, the actual number of bytes received is stored in
    ///      the variable.
    /// @return `Success` if the requested number of bytes was read,
    ///      `Partial` if not the full number of bytes could be retrieved.
    ///      `Error` is there was an error.
    ///
    virtual Status receive(
        uint8_t *data,
        DataSize dataSize,
        DataSize *dataReceived = nullptr) noexcept = 0;
    
    /// Receive a block of data from the serial line.
    ///
    /// This is a specialized version of the `receive` method. It works
    /// similar, but also successfully stops after reading a byte equal to the
    /// `blockEndMark`. This end mark is read and added to the buffer `data`.
    ///
    /// @param[out] data A pointer to a buffer to receive the data.
    /// @param[in] dataSize The maximum number of bytes to receive.
    /// @param[in] blockEndMark The block end mark.
    /// @param[out] dataReceived The number of bytes received in the given
    ///      timeframe.
    /// @return `Success` if the block end mark was read. `Partial` the data
    ///      block was filled with data, but no end mark was read.
    ///      `Error` if there was an error.
    ///
    virtual Status receiveBlock(
        uint8_t *data,
        DataSize dataSize,
        uint8_t blockEndMark,
        DataSize *dataReceived) noexcept = 0;
    
    /// Reset the receive buffer.
    ///
    /// A call of this function will drop all received data from
    /// the receive buffer.
    ///
    /// @return `Success` or `Error`.
    ///
    virtual Status receiveReset() noexcept = 0;
};
  

}


