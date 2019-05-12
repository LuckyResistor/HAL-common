#pragma once
//
// Wire Master
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
  

/// An interface for the I2C protocol.
///
/// This is just an universal interface to access an I2C bus from
/// any hardware driver. This interface supports a proper error handling
/// including the handling for timeouts.
///
/// The driver must make sure to test for any error conditons on the
/// bus and pass it to the calling application.
///
class WireMaster
{
public:
    /// The status of an operation.
    ///
    enum class Status : uint8_t {
        Success, ///< The operation complete successfully.
        AddressNotFound, ///< There was no acknowledge from a slave for the given address.
        NoAcknowledge, ///< A transfered/received byte was not acknowledged by the slave.
        NotSupported, ///< The operation is not supported by the backend.
        Timeout, ///< There was a timeout on the operation.
        Error, ///< The operation failed. See API documentation for details.
    };

    /// The speed of the bus.
    ///
    enum class Speed : uint8_t {
        Standard, ///< The 100kHz standard mode.
        Fast, ///< The 400kHz fast mode.
        FastPlus, ///< The 1MHz fast plus mode.
        HighSpeed, ///< The 3.4MHz high-speed mode.
    };

    /// The bit operation.
    ///
    enum class BitOperation : uint8_t {
        Clear = 0, ///< Clear the bits in the register.
        Set, ///< Set the bits in the register.
        Flip, ///< Flip/Invert the bits.
    };
    
    /// The bit test result.
    ///
    enum class BitResult : uint8_t {
        Zero, ///< All tested bits are zero.
        Set, ///< All tested bits are set.
        Mixed, ///< Some bits are set.
    };

public:
    /// The virtual deconstructor.
    ///
    virtual ~WireMaster() = default;

public:
    /// Initialize the I2C interface.
    ///
    /// This must be called once while initialization.
    /// An error can happen if the hardware does not respond as expected.
    ///
    /// @return `Success` on success.
    ///         `Timeout` on any timeout.
    ///         `Error` on any error (e.g. bus error)
    ///
    virtual Status initialize() = 0;
    
    /// Reset the I2C interface.
    ///
    /// This method starts a software reset of the I2C interface.
    /// If the hardware does not respond to the software reset, this
    /// function will result in an error.
    ///
    /// @return `Success` on success.
    ///         `Error` on any error if the bus could not be reset.
    ///
    virtual Status reset() = 0;
    
    /// Set the speed for the I2C communication.
    ///
    /// @param[in] speed The speed for the bus.
    /// @return `Success` on success.
    ///         `Error` if there was a hardware problem settings the speed.
    ///         `NotSupported` if a speed change is not supported by the backend.
    ///
    virtual Status setSpeed(Speed speed) = 0;

    /// Set the speed for the I2C communication.
    ///
    /// @param[in] speed The speed for the bus in Hz.
    /// @return `Success` on success.
    ///         `Error` if there was a hardware problem settings the speed.
    ///         `NotSupported` if a speed change is not supported by the backend.
    ///
    virtual Status setSpeed(uint32_t frequencyHz) = 0;

    /// Start a write operation.
    ///
    /// This method sends a start condition and the address.
    /// It waits until there is an acknowledge on the bus.
    ///
    /// Only call `writeEndAndStop()` if all previous operations returned `Success`.
    ///
    /// @param[in] address The 7 bit address without the mode bit.
    /// @return `Success` if an acknowledge is received after the address and the slave is ready to received data.
    ///         `AddressNotFound` if no slave responds to the given address.
    ///         `Timeout` if the bus not responds in time.
    ///         `Error` if the bus is in an error state.
    ///
    virtual Status writeBegin(uint8_t address) = 0;
    
    /// Write a byte to the bus.
    ///
    /// Only call `writeEndAndStop()` if all previous operations returned `Success`.
    ///
    /// @param[in] data The data byte to send.
    /// @return `Success` if an acknowledge is received after the byte sent.
    ///         `NoAcknowledge` if there was no acknowledge after the sent byte. In this case
    ///            a stop bit will be automatically sent.
    ///         `Timeout` if the byte could not be sent in time.
    ///         `Error` if the bus is in error state.
    ///
    virtual Status writeByte(uint8_t data) = 0;
    
    /// Stop the write operation and send stop condition.
    ///
    /// Only call `writeEndAndStop()` if all previous operations returned `Success`.
    ///
    /// @return `Success` if the stop bit is successfully sent.
    ///         `Timeout` if the stop bit could not be sent in time.
    ///
    virtual Status writeEndAndStop() = 0;

    /// Stop the write operation and send a repeated start condition.
    ///
    /// You have to start a read operation after calling this method.
    ///
    /// @return `Success` if the repeated start bit condition successfully sent.
    ///         `Timeout` if the repeated start condition could not be sent in time.
    ///
    virtual Status writeEndAndStart() = 0;

    /// Write a number of bytes to the bus.
    ///
    /// @param[in] address The 7 bit address without the mode bit.
    /// @param[in] data A pointer to the data buffer to send.
    /// @param[in] count The number of bytes to send.
    ///
    /// @return `Success` All bytes were sent over the bus successfully.
    ///         `AddressNotFound` if no slave responds to the given address.
    ///         `NoAcknowledge` if not all bytes were acknowledged by the slave.
    ///         `Timeout` if the bus not responds in time.
    ///         `Error` if the bus is in an error state.
    ///
    virtual Status writeBytes(uint8_t address, const uint8_t *data, uint8_t count) = 0;

    /// Write a byte as address and a second as data.
    ///
    /// This is a method for convenience. The first byte is used as register address,
    /// followed by a second byte as data byte.
    ///
    /// [S][address+W][registerAddress][data][P]
    ///
    /// @param[in] address The 7 bit address without the mode bit.
    /// @param[in] registerAddress The register address. The first byte sent on the bus.
    /// @param[in] data One byte of data sent after the register address.
    ///
    /// @return `Success` All bytes were sent over the bus successfully.
    ///         `AddressNotFound` if no slave responds to the given address.
    ///         `NoAcknowledge` if not all bytes were acknowledged by the slave.
    ///         `Timeout` if the bus not responds in time.
    ///         `Error` if the bus is in an error state.
    ///
    virtual Status writeRegisterData(uint8_t address, uint8_t registerAddress, uint8_t data) = 0;

    /// Write a block of data, prefixed with a register address.
    ///
    /// This is a method for convenience. A first byte is sent as register address,
    /// followed by a block of data bytes.
    ///
    /// [S][address+W][registerAddress][data0][data1]...[P]
    ///
    /// @param[in] address The 7 bit address without the mode bit.
    /// @param[in] registerAddress The register address. The first byte to send.
    /// @param[in] data A pointer to the data buffer to send.
    /// @param[in] count The number of bytes to send.
    ///
    /// @return `Success` All bytes were sent over the bus successfully.
    ///         `AddressNotFound` if no slave responds to the given address.
    ///         `NoAcknowledge` if not all bytes were acknowledged by the slave.
    ///         `Timeout` if the bus not responds in time.
    ///         `Error` if the bus is in an error state.
    ///
    virtual Status writeRegisterData(uint8_t address, uint8_t registerAddress, const uint8_t *data, uint8_t count) = 0;

    /// Read a number of bytes.
    ///
    /// @param[in] address The 7 bit address without the mode bit.
    /// @param[out] data A pointer to a buffer for the read data.
    ///             The buffer has to be enough space for all read bytes.
    /// @param[in] count The number of bytes to read.
    /// @return `Success` if all bytes were read from the bus as expected.
    ///         `AddressNotFound` if no slave responds to the given address.
    ///         `Timeout` if the read operation run into any timeout. This could also mean
    ///           that not the requested number of bytes could be read.
    ///         `Error` if the bus is in error state.
    ///
    virtual Status readBytes(uint8_t address, uint8_t *data, uint8_t count) = 0;

    /// Read multiple registers.
    ///
    /// This is a method for convenience. It first starts a write operation on the bus with
    /// the `address` and writes the `registerAddress` byte. Next a repeated start is sent
    /// followed byt the `address` and reading a number of data bytes.
    ///
    /// ```
    /// [S][address+W][registerAddress][Sr][address+R][data0][data1]...[P]
    /// ```
    ///
    /// @param[in] address The 7 bit address without the mode bit.
    /// @param[in] registerAddress The register address to send as write operation.
    /// @param[out] data A pointer to a buffer for the read data.
    ///             The buffer has to be enough space for all read bytes.
    /// @param[in] count The number of bytes to read.
    /// @return `Success` if all bytes were write/read from the bus as expected.
    ///         `AddressNotFound` if no slave responds to the given address.
    ///         `Timeout` if the read operation run into any timeout. This could also mean
    ///           that not the requested number of bytes could be read.
    ///         `Error` if the bus is in error state.
    ///
    virtual Status readRegisterData(uint8_t address, uint8_t registerAddress, uint8_t *data, uint8_t count) = 0;

protected:
    /// The private constructor.
    ///
    WireMaster() = default;
};


}
