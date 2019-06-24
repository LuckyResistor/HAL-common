//
// Serial Line Buffer
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
#include "SerialLineBuffer.hpp"


#include "IntegerMath.hpp"

#include <limits>


namespace lr {


SerialLineBuffer::SerialLineBuffer(
    SerialLine *serialLine,
    SerialLineBuffer::BufferSizeType sendSize,
    SerialLineBuffer::BufferSizeType receiveSize)
:
    _serialLine(serialLine),
    _sendBuffer(sendSize),
    _receiveBuffer(receiveSize)
{
}


SerialLine::Status SerialLineBuffer::synchronize() noexcept
{
    const DataSize bufferSize = 32;
    uint8_t buffer[bufferSize];
    if (_sendBuffer.isEnabled()) {
        const auto sendBytesAvailable = _serialLine->sendBytesAvailable();
        if (sendBytesAvailable > 0 || !_sendBuffer.isEmpty()) {
            auto bytesToSend = IntegerMath::min(sendBytesAvailable, _sendBuffer.getCount());
            while (bytesToSend > 0) {
               const auto readBytes = _sendBuffer.read(buffer, IntegerMath::min(bytesToSend, bufferSize));
               _serialLine->send(buffer, readBytes);
               bytesToSend -= readBytes;
            }
        }
    }
    if (_receiveBuffer.isEnabled()) {
        const auto receiveBytesAvailable = _serialLine->receiveBytesAvailable();
        if (receiveBytesAvailable > 0) {
            auto bytesToReceive = receiveBytesAvailable;
            while (bytesToReceive > 0) {
                DataSize receivedBytes;
                _serialLine->receive(buffer, IntegerMath::min(bytesToReceive, bufferSize), &receivedBytes);
                _receiveBuffer.write(buffer, receivedBytes);
                bytesToReceive -= receivedBytes;
            }
        }
    }
    return Status::Success;
}


SerialLine::DataSize SerialLineBuffer::sendBytesAvailable() noexcept
{
    if (_sendBuffer.isDisabled()) {
        return _serialLine->sendBytesAvailable();
    }
    return std::numeric_limits<DataSize>::max();
}


SerialLine::Status SerialLineBuffer::send(uint8_t value) noexcept
{
    if (_sendBuffer.isDisabled()) {
        return _serialLine->send(value);
    }
    _sendBuffer.write(&value, 1);
    return Status::Success;
}


SerialLine::Status
SerialLineBuffer::send(const uint8_t *data, SerialLine::DataSize dataSize, SerialLine::DataSize *dataSent) noexcept
{
    if (_sendBuffer.isDisabled()) {
        return _serialLine->send(data, dataSize, dataSent);
    }
    _sendBuffer.write(data, dataSize);
    if (dataSent != nullptr) {
        *dataSent = dataSize;
    }
    return Status::Success;
}


SerialLine::Status SerialLineBuffer::sendReset() noexcept
{
    if (_sendBuffer.isDisabled()) {
        return _serialLine->sendReset();
    }
    _sendBuffer.reset();
    _serialLine->sendReset();
    return Status::Success;
}


SerialLine::DataSize SerialLineBuffer::receiveBytesAvailable() noexcept
{
    if (_receiveBuffer.isDisabled()) {
        return _serialLine->receiveBytesAvailable();
    }
    return _receiveBuffer.getCount();
}


SerialLine::Status SerialLineBuffer::receive(uint8_t &value) noexcept
{
    if (_receiveBuffer.isDisabled()) {
        return _serialLine->receive(value);
    }
    if (_receiveBuffer.isEmpty()) {
        return Status::Partial;
    }
    _receiveBuffer.read(&value, 1);
    return Status::Success;
}


SerialLine::Status
SerialLineBuffer::receive(uint8_t *data, SerialLine::DataSize dataSize, SerialLine::DataSize *dataReceived) noexcept
{
    if (_receiveBuffer.isDisabled()) {
        return _serialLine->receive(data, dataSize, dataReceived);
    }
    const auto bytesToRead = IntegerMath::min(dataSize, _receiveBuffer.getCount());
    if (bytesToRead > 0) {
        _receiveBuffer.read(data, bytesToRead);
    }
    if (dataReceived != nullptr) {
        *dataReceived = bytesToRead;
    }
    if (bytesToRead < dataSize) {
        return Status::Partial;
    } else {
        return Status::Success;
    }
}


SerialLine::Status SerialLineBuffer::receiveBlock(
    uint8_t *data,
    SerialLine::DataSize dataSize,
    uint8_t blockEndMark,
    SerialLine::DataSize *dataReceived) noexcept
{
    if (_receiveBuffer.isDisabled()) {
        return _serialLine->receiveBlock(data, dataSize, blockEndMark, dataReceived);
    }
    auto bytesToRead = IntegerMath::min(dataSize, _receiveBuffer.getCount());
    if (bytesToRead > 0) {
        bytesToRead = _receiveBuffer.readToEnd(data, bytesToRead, blockEndMark);
    }
    if (dataReceived != nullptr) {
        *dataReceived = bytesToRead;
    }
    if (bytesToRead < dataSize) {
        return Status::Partial;
    } else {
        return Status::Success;
    }
}


SerialLine::Status SerialLineBuffer::receiveReset() noexcept
{
    if (_receiveBuffer.isDisabled()) {
        return _serialLine->receiveReset();
    }
    _receiveBuffer.reset();
    _serialLine->receiveReset();
    return Status::Success;
}


}