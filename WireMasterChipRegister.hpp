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


#include "WireMasterChip.hpp"


namespace lr {


/// A template class to simplify the access to a chip using a register enum.
///
/// Many chips have a simple set of enumerated registers and accessed in
/// very similar way. This template class takes a enum defining all the
/// registers and provide a set of simple read/write operations to this
/// registers.
///
/// @tparam Register The enum with the register definitions.
///     This enum has to be the underlying type of an uint8_t.
///
template<typename Register>
class WireMasterRegisterChip : public WireMasterChip
{
public:
    static_assert(std::is_enum<Register>::value, "The Register parameter needs to be an enum.");

public:
    /// Create a new instance of the chip acess.
    ///
    /// @param[in] bus The pointer to the bus to use.
    /// @param[in] address The address of the chip.
    ///
    WireMasterRegisterChip(WireMaster *bus, uint8_t address) : WireMasterChip(bus, address) {}
    
public: // Get all functions from the superclass
    using WireMasterChip::readRegisterData;
    using WireMasterChip::readRegister;
    using WireMasterChip::writeRegister;
    using WireMasterChip::writeBits;
    using WireMasterChip::testBits;
    using WireMasterChip::changeBits;

public: // Wrapper functions to use the enum as register.
    /// @copydoc WireMasterChip::readRegisterData(const uint8_t, uint8_t*, const uint8_t)
    inline Status readRegisterData(const Register reg, uint8_t *data, const uint8_t count) const {
        return WireMasterChip::readRegisterData(static_cast<const uint8_t>(reg), data, count);
    }
    /// @copydoc WireMasterChip::readRegisterData(const uint8_t, uint8_t&)
    inline Status readRegister(const Register reg, uint8_t &data) const {
        return WireMasterChip::readRegister(static_cast<const uint8_t>(reg), data);
    }
    /// @copydoc WireMasterChip::readRegisterData(const uint8_t, uint16_t&)
    inline Status readRegister(const Register reg, uint16_t &value) const {
        return WireMasterChip::readRegister(static_cast<const uint8_t>(reg), value);
    }
    /// @copydoc WireMasterChip::readRegisterData(const uint8_t, uint32_t&)
    inline Status readRegister(const Register reg, uint32_t &value) const {
        return WireMasterChip::readRegister(static_cast<const uint8_t>(reg), value);
    }
    /// @copydoc WireMasterChip::writeRegisterData(const uint8_t, const uint8_t*, const uint8_t)
    inline Status writeRegisterData(const Register reg, const uint8_t *data, const uint8_t count) const {
        return WireMasterChip::writeRegisterData(static_cast<const uint8_t>(reg), data, count);
    }
    /// @copydoc WireMasterChip::writeRegisterData(const uint8_t, const uint8_t)
    inline Status writeRegister(const Register reg, const uint8_t data) const {
        return WireMasterChip::writeRegister(static_cast<const uint8_t>(reg), data);
    }
    /// @copydoc WireMasterChip::writeRegisterData(const uint8_t, const uint16_t)
    inline Status writeRegister(const Register reg, const uint16_t value) const {
        return WireMasterChip::writeRegister(static_cast<const uint8_t>(reg), value);
    }
    /// @copydoc WireMasterChip::writeRegisterData(const uint8_t, const uint32_t)
    inline Status writeRegister(const Register reg, const uint32_t value) const {
        return WireMasterChip::writeRegister(static_cast<const uint8_t>(reg), value);
    }

public:
    /// @copydoc WireMasterChip::writeBitsTemplate()
    inline Status writeBits(const Register reg, const uint8_t bitMask, const uint8_t value) const {
        return writeBitsTemplate<uint8_t>(static_cast<uint8_t>(reg), bitMask, value);
    }
    /// @copydoc WireMasterChip::writeBitsTemplate()
    inline Status writeBits(const Register reg, const uint16_t bitMask, const uint16_t value) const {
        return writeBitsTemplate<uint16_t>(static_cast<uint8_t>(reg), bitMask, value);
    }
    /// @copydoc WireMasterChip::writeBitsTemplate()
    inline Status writeBits(const Register reg, const uint32_t bitMask, const uint32_t value) const {
        return writeBitsTemplate<uint32_t>(static_cast<uint8_t>(reg), bitMask, value);
    }
    /// @copydoc WireMasterChip::testBitsTemplate()
    inline Status testBits(const Register reg, const uint8_t bitMask, BitResult &bitResult) const {
        return testBitsTemplate<uint8_t>(static_cast<uint8_t>(reg), bitMask, bitResult);
    }
    /// @copydoc WireMasterChip::testBitsTemplate()
    inline Status testBits(const Register reg, const uint16_t bitMask, BitResult &bitResult) const {
        return testBitsTemplate<uint16_t>(static_cast<uint8_t>(reg), bitMask, bitResult);
    }
    /// @copydoc WireMasterChip::testBitsTemplate()
    inline Status testBits(const Register reg, const uint32_t bitMask, BitResult &bitResult) const {
        return testBitsTemplate<uint32_t>(static_cast<uint8_t>(reg), bitMask, bitResult);
    }
    /// @copydoc WireMasterChip::changeBitsTemplate()
    inline Status changeBits(const Register reg, const uint8_t bitMask, const BitOperation bitOperation) const {
        return changeBitsTemplate<uint8_t>(static_cast<uint8_t>(reg), bitMask, bitOperation);
    }
    /// @copydoc WireMasterChip::changeBitsTemplate()
    inline Status changeBits(const Register reg, const uint16_t bitMask, const BitOperation bitOperation) const {
        return changeBitsTemplate<uint16_t>(static_cast<uint8_t>(reg), bitMask, bitOperation);
    }
    /// @copydoc WireMasterChip::changeBitsTemplate()
    inline Status changeBits(const Register reg, const uint32_t bitMask, const BitOperation bitOperation) const {
        return changeBitsTemplate<uint32_t>(static_cast<uint8_t>(reg), bitMask, bitOperation);
    }
};


}
