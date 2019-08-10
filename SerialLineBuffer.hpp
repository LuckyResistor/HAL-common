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


#include "RingBuffer.hpp"
#include "SerialLine.hpp"


namespace lr {


/// A simple additional buffer for a serial line.
///
/// The implementation of this class uses a ring buffer to store the specified amount of sent or received bytes
/// from the underlying serial interface. You can specify a size of zero for the send or receiving part to disable
/// the buffer for this direction. In this case, send or receive calls are passed to the underlying serial
/// interface directly, without any buffering.
///
/// If a buffer is full, the oldest data is overwritten with new one.
///
/// This buffer was designed for three specific use cases:
///
/// 1. Delayed serial connection.
/// -----------------------------
/// Boards which are using a serial line over USB often have a long delay until the connection is made, or the
/// USB cable is plugged in at a later stage for debugging. Wrapping the original USB serial implementation with
/// this buffer will capture all initial messages, without delaying the actual start of the firmware. As soon the
/// USB connection is ready, all messages are sent to the serial console. Or if a serial line is connected at a later
/// time, a number of previous messages are sent to the console.
///
/// 2. Interrupt decoupling.
/// ------------------------
/// Data is only sent and received in the `synchronize()` method. This method should be called from the main
/// event loop of the firmware. Therefore, all serial communication calls are made from the main loop.
/// This is allows to send data from an interrupt to a serial line with some limitations.
/// Note this implementation does not implement a proper protection if you mix e.g. `send()` calls from the main
/// loop and interrupts. Also the `synchronize()` method is not protected and you have to add additional code to
/// make sure no the interrupt does not occur in the `synchronize()` method.
///
/// 3. Caching
/// ----------
/// In an event based system, caching of messages will reduce the runtime of events and speedup writing data in
/// many small chunks. All this small data parts are cached in the buffer and sent in a larger block in the
/// `synchronize()` method in the main loop. Depending on the built-in buffer and implementation of of the underlying
/// serial line (DMA, etc,), this may improve the performance and reduce delays.
///
class SerialLineBuffer : public SerialLine
{
public:
    using BufferSizeType = uint16_t;

public:
    /// Create a new serial line buffer.
    ///
    /// @param serialLine A pointer to the underlying serial line implementation. Must not be `nullptr`.
    /// @param sendSize The size of the send buffer. `0` to disable the send buffer.
    /// @param receiveSize The size of the receive buffer, `0` to disable the receive buffer.
    ///
    SerialLineBuffer(SerialLine *serialLine, BufferSizeType sendSize, BufferSizeType receiveSize);

public:
    /// Synchronise the buffer.
    ///
    /// This method sends and receives all buffered data to and from the underlying serial interface.
    /// If a buffer is disabled (by setting it to zero), the calls are made immediately and the disabled direction
    /// is ignored in this method.
    ///
    Status synchronize() noexcept;

public: // Implement SerialLine
    DataSize sendBytesAvailable() noexcept override;
    Status send(uint8_t value) noexcept override;
    Status send(const uint8_t *data, DataSize dataSize, DataSize *dataSent = nullptr) noexcept override;
    Status sendReset() noexcept override;
    DataSize receiveBytesAvailable() noexcept override;
    Status receive(uint8_t &value) noexcept override;
    Status receive(uint8_t *data, DataSize dataSize, DataSize *dataReceived = nullptr) noexcept override;
    Status receiveBlock(uint8_t *data, DataSize dataSize, uint8_t blockEndMark, DataSize *dataReceived) noexcept override;
    Status receiveReset() noexcept override;

private:
    SerialLine *_serialLine; ///< The underlying serial line.
    RingBuffer<BufferSizeType, uint8_t> _sendBuffer; ///< The send buffer.
    RingBuffer<BufferSizeType, uint8_t> _receiveBuffer; ///< The receive buffer.
};


}

