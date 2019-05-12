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


#include "WireMaster.hpp"


namespace lr {


/// A class to simplify access to a single chip with a fixed address.
///
/// This class just simplifies all calls by removing the fixed chip address.
///
class WireMasterChip
{
public:
    using BitOperation = WireMaster::BitOperation;
    using BitResult = WireMaster::BitResult;
    using Status = WireMaster::Status;

public:
    /// Create a new instance of the chip acess.
    ///
    /// @param[in] bus The pointer to the bus to use.
    /// @param[in] address The address of the chip.
    ///
    WireMasterChip(WireMaster *bus, uint8_t address)
        : _bus(bus), _address(address)
    {
    }

public: // Wrapper around the native bus functions.
    /// Begin writing to the chip.
    ///
    /// @see WireMaster::writeBegin()
    /// @return The status of the operation. See [Status](@ref WireMaster::Status) for details.
    ///
    inline Status writeBegin() const {
        return _bus->writeBegin(_address);
    }
    
    /// Write a byte to the chip.
    ///
    /// @see WireMaster::writeByte()
    /// @param[in] data The byte to write.
    /// @return The status of the operation. See [Status](@ref WireMaster::Status) for details.
    ///
    inline Status writeByte(uint8_t data) const {
        return _bus->writeByte(data);
    }
    
    /// End the write operation with a stop condition.
    ///
    /// @see WireMaster::writeEndAndStop()
    /// @return The status of the operation. See [Status](@ref WireMaster::Status) for details.
    ///
    inline Status writeEndAndStop() const {
        return _bus->writeEndAndStop();
    }
    
    /// End the write operation with a repeated start condition.
    ///
    /// @see WireMaster::writeEndAndStart()
    /// @return The status of the operation. See [Status](@ref WireMaster::Status) for details.
    ///
    inline Status writeEndAndStart() const {
        return _bus->writeEndAndStart();
    }
    
    /// Write a number of bytes to the chip.
    ///
    /// @see WireMaster::writeEndAndStart()
    /// @param[in] data A pointer to the data.
    /// @param[in] count The number of bytes to send.
    /// @return The status of the operation. See [Status](@ref WireMaster::Status) for details.
    ///
    inline Status writeBytes(const uint8_t *data, const uint8_t count) const {
        return _bus->writeBytes(_address, data, count);
    }
    
    /// Read a number of bytes from the chip.
    ///
    /// @see WireMaster::readBytes()
    /// @param[out] data A pointer to a buffer for the read bytes.
    /// @param[in] count The number of bytes to read.
    /// @return The status of the operation. See [Status](@ref WireMaster::Status) for details.
    ///
    inline Status readBytes(uint8_t *data, const uint8_t count) const {
        return _bus->readBytes(_address, data, count);
    }

    /// Write data to a register.
    ///
    /// @see WireMaster::writeRegisterData()
    /// @return The status of the operation. See [Status](@ref WireMaster::Status) for details.
    ///
    inline Status writeRegisterData(const uint8_t registerAddress, const uint8_t *data, const uint8_t count) const {
        return _bus->writeRegisterData(_address, registerAddress, data, count);
    }
    
    /// Read data from a register.
    ///
    /// @see WireMaster::readRegisterData()
    /// @return The status of the operation. See [Status](@ref WireMaster::Status) for details.
    ///
    inline Status readRegisterData(const uint8_t registerAddress, uint8_t *data, const uint8_t count) const {
        return _bus->readRegisterData(_address, registerAddress, data, count);
    }
    
public:
    /// Read a single 8bit register from the chip.
    ///
    /// This will use the following sequence to read one byte from a register of a chip.
    ///
    /// ```
    /// [S][address+W][registerAddress][Sr][address+R][data][P]
    /// ```
    ///
    /// @param[in] reg The register to read.
    /// @param[out] data The variable to store the register value.
    /// @return The status of the operation. See [Status](@ref WireMaster::Status) for details.
    ///
    inline Status readRegister(const uint8_t reg, uint8_t &data) const {
        return readRegisterData(reg, &data, static_cast<uint8_t>(1));
    }

    /// Read a single 16bit register from the chip.
    ///
    /// This will use the following sequence to read one byte from a register of a chip.
    /// It assumes the value is stored as little endian value in two sequential bytes.
    ///
    /// ```
    /// [S][address+W][registerAddress][Sr][address+R][value lsb][value msb][P]
    /// ```
    ///
    /// @param[in] reg The register to read.
    /// @param[out] value The variable to store the register value.
    /// @return The status of the operation. See [Status](@ref WireMaster::Status) for details.
    ///
    inline Status readRegister(const uint8_t reg, uint16_t &value) const {
        uint8_t data[2];
        const auto status = readRegisterData(reg, data, static_cast<uint8_t>(2));
        value = static_cast<uint16_t>(data[0])|(static_cast<uint16_t>(data[1])<<8);
        return status;
    }

    /// Read a single 32bit register from the chip.
    ///
    /// This will use the following sequence to read one byte from a register of a chip.
    /// It assumes the value is stored as little endian value in two sequential bytes.
    ///
    /// ```
    /// [S][address+W][registerAddress][Sr][address+R][value lsb][value][value][value msb][P]
    /// ```
    ///
    /// @param[in] reg The register to read.
    /// @param[out] value The variable to store the register value.
    /// @return The status of the operation. See [Status](@ref WireMaster::Status) for details.
    ///
    inline Status readRegister(const uint8_t reg, uint32_t &value) const {
        uint8_t data[4];
        const auto status = readRegisterData(reg, data, static_cast<uint8_t>(4));
        value = static_cast<uint32_t>(data[0])|
            (static_cast<uint32_t>(data[1])<<8)|
            (static_cast<uint32_t>(data[2])<<16)|
            (static_cast<uint32_t>(data[3])<<24);
        return status;
    }

    /// Write a single 8bit register to the chip.
    ///
    /// This will use the following sequence to write a single register to the chip.
    ///
    /// ```
    /// [S][address+W][registerAddress][data][P]
    /// ```
    ///
    /// @param[in] reg The register to write into.
    /// @param[in] value The byte value to write in the register.
    /// @return The status of the operation. See [Status](@ref WireMaster::Status) for details.
    ///
    inline Status writeRegister(const uint8_t reg, const uint8_t value) const {
        return writeRegisterData(static_cast<uint8_t>(reg), &value, static_cast<uint8_t>(1));
    }

    /// Write a single 16bit register to the chip.
    ///
    /// This will use the following sequence to write a single register to the chip.
    /// It assumes the value is written as little edian value in two sequential bytes.
    ///
    /// ```
    /// [S][address+W][registerAddress][value lsb][value msb][P]
    /// ```
    ///
    /// @param[in] reg The register to write into.
    /// @param[in] value The 16bit value to write in the register.
    /// @return The status of the operation. See [Status](@ref WireMaster::Status) for details.
    ///
    inline Status writeRegister(const uint8_t reg, const uint16_t value) const {
        uint8_t data[2] = {static_cast<uint8_t>(value & 0x00ffu), static_cast<uint8_t>(value>>8)};
        return writeRegisterData(static_cast<uint8_t>(reg), data, static_cast<uint8_t>(2));
    }

    /// Write a single 32bit register to the chip.
    ///
    /// This will use the following sequence to write a single register to the chip.
    /// It assumes the value is written as little edian value in two sequential bytes.
    ///
    /// ```
    /// [S][address+W][registerAddress][value lsb][value][value][value msb][P]
    /// ```
    ///
    /// @param[in] reg The register to write into.
    /// @param[in] value The 32bit value to write in the register.
    /// @return The status of the operation. See [Status](@ref WireMaster::Status) for details.
    ///
    inline Status writeRegister(const uint8_t reg, const uint32_t value) const {
        uint8_t data[4] = {
            static_cast<uint8_t>(value&0x000000fful),
            static_cast<uint8_t>((value&0x0000ff00ul)>>8),
            static_cast<uint8_t>((value&0x00ff0000ul)>>16),
            static_cast<uint8_t>((value&0xff000000ul)>>24)};
        return writeRegisterData(static_cast<uint8_t>(reg), data, static_cast<uint8_t>(4));
    }

protected:
    /// Write masked bits to a register.
    ///
    /// This will read the current value from the register and
    /// apply all bits from `value` masked using the given `mask`.
    ///
    /// This function will only write the bits back into the register
    /// if there is an actual change.
    ///
    /// @param[in] reg The register to write into.
    /// @param[in] bitMask The 8/16/32bit mask to select the bits to change.
    /// @param[in] value The 8/16/32bit value to write in the register.
    /// @return The status of the operation. See [Status](@ref WireMaster::Status) for details.
    ///
    template<typename Value>
    inline Status writeBitsTemplate(const uint8_t reg, const Value bitMask, const Value value) const {
        if (bitMask == 0) {
            return Status::Success;
        }
        Value data;
        const auto status = readRegister(reg, data);
        if (status != Status::Success) {
            return status;
        }
        if ((data&bitMask)==(value&bitMask)) {
            return Status::Success;
        }
        data &= (~bitMask);
        data |= (value&bitMask);
        return writeRegister(reg, data);
    }

    /// Template to test a number of bits.
    ///
    /// The register is read and compared to the bits in `bitMask`. If
    /// all bits in `bitMask` are set, the parameter `enabled` is set to `true`
    /// otherwise it is set to `false`.
    ///
    /// @param[in] reg The register to change.
    /// @param[in] bitMask The 8/16/32bit mask for the bits to read.
    /// @param[in] bitResult The result of the test, see [BitResult](@ref WireMaster::BitResult) for details.
    /// @return The status of the operation. See [Status](@ref WireMaster::Status) for details.
    ///
    template<typename Value>
    inline Status testBitsTemplate(const uint8_t reg, const Value bitMask, BitResult &bitResult) const {
        Value value;
        const auto status = readRegister(reg, value);
        if (status != Status::Success) {
            return status;
        }
        const auto maskedBits = (value&bitMask);
        if (maskedBits == 0) {
            bitResult = BitResult::Zero;
        } else if (maskedBits == bitMask) {
            bitResult = BitResult::Set;
        } else {
            bitResult = BitResult::Mixed;
        }
        return Status::Success;
    }

    /// Change bits in a register.
    ///
    /// The register is first read and the bits in `bitMask` changed according to
    /// the `bitOperation` parameter. If this actually changes the register, it is written
    /// back.
    ///
    /// @param[in] reg The register to write into.
    /// @param[in] bitMask The 8/16/32bit mask which bits to address.
    /// @param[in] bitOperation How the masked bits shall be modified.
    /// @return The status of the operation. See [Status](@ref WireMaster::Status) for details.
    ///
    template<typename Value>
    inline Status changeBitsTemplate(const uint8_t reg, const Value bitMask, const BitOperation bitOperation) const {
        Value data;
        const auto status = readRegister(reg, data);
        if (status != Status::Success) {
            return status;
        }
        Value newData = data;
        switch (bitOperation) {
        case BitOperation::Set: newData |= bitMask; break;
        case BitOperation::Clear: newData &= (~bitMask); break;
        case BitOperation::Flip: newData ^= bitMask; break;
        }
        if (data != newData) {
            return writeRegister(reg, newData);
        } else {
            return Status::Success;
        }
    }

public:
    /// @copydoc WireMasterChip::writeBitsTemplate()
    inline Status writeBits(const uint8_t reg, const uint8_t bitMask, const uint8_t value) const {
        return writeBitsTemplate<uint8_t>(reg, bitMask, value);
    }
    /// @copydoc WireMasterChip::writeBitsTemplate()
    inline Status writeBits(const uint8_t reg, const uint16_t bitMask, const uint16_t value) const {
        return writeBitsTemplate<uint16_t>(reg, bitMask, value);
    }
    /// @copydoc WireMasterChip::writeBitsTemplate()
    inline Status writeBits(const uint8_t reg, const uint32_t bitMask, const uint32_t value) const {
        return writeBitsTemplate<uint32_t>(reg, bitMask, value);
    }
    /// @copydoc WireMasterChip::testBitsTemplate()
    inline Status testBits(const uint8_t reg, const uint8_t bitMask, BitResult &bitResult) const {
        return testBitsTemplate<uint8_t>(reg, bitMask, bitResult);
    }
    /// @copydoc WireMasterChip::testBitsTemplate()
    inline Status testBits(const uint8_t reg, const uint16_t bitMask, BitResult &bitResult) const {
        return testBitsTemplate<uint16_t>(reg, bitMask, bitResult);
    }
    /// @copydoc WireMasterChip::testBitsTemplate()
    inline Status testBits(const uint8_t reg, const uint32_t bitMask, BitResult &bitResult) const {
        return testBitsTemplate<uint32_t>(reg, bitMask, bitResult);
    }
    /// @copydoc WireMasterChip::changeBitsTemplate()
    inline Status changeBits(const uint8_t reg, const uint8_t bitMask, const BitOperation bitOperation) const {
        return changeBitsTemplate<uint8_t>(reg, bitMask, bitOperation);
    }
    /// @copydoc WireMasterChip::changeBitsTemplate()
    inline Status changeBits(const uint8_t reg, const uint16_t bitMask, const BitOperation bitOperation) const {
        return changeBitsTemplate<uint16_t>(reg, bitMask, bitOperation);
    }
    /// @copydoc WireMasterChip::changeBitsTemplate()
    inline Status changeBits(const uint8_t reg, const uint32_t bitMask, const BitOperation bitOperation) const {
        return changeBitsTemplate<uint32_t>(reg, bitMask, bitOperation);
    }

private:
    WireMaster* const _bus; ///< The bus to use for the access.
    const uint8_t _address; ///< The address of the chip.
};


}
