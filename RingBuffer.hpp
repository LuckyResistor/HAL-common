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


#include "IntegerMath.hpp"

#include <cstring>
#include <cstdlib>


namespace lr {


/// A simple but universal ring buffer.
///
/// The ring buffer will store maximum `size()` elements and overwrite existing
/// elements with new ones if they are not read.
///
/// Design Note: This is a ring buffer implementation for embedded code.
/// Therefore it is small and simple and omits most of the checks keep the
/// resulting code size as small as possible.
///
/// @tparam Size The type used for size calculations. It has to be an
///              unsigned integer type.
/// @tparam Element The type for the elements in the buffer.
///                 This has to be a scalar type
///
template<typename Size, typename Element>
class RingBuffer
{
    // Check the template parameter.
    static_assert(std::is_integral<Size>::value, "Integral required");
    static_assert(std::is_unsigned<Size>::value, "Unsigned integral required");
    // Not yet implemented in GNU compiler:
    //static_assert(std::is_trivially_copyable<Element>::value, "The element has to be trivially copyable.");
    
public:
    /// Create a new ring buffer.
    ///
    /// You can specify a size of zero to "disable" the buffer. In this case,
    /// no memory is allocated for the buffer but the behaviour of most
    /// functions is undefined. You can test if the buffer is disabled using
    /// the `isDisabled()` method.
    ///
    /// @param size The size of the buffer. Zero disables it.
    ///             The size 1 will not work.
    ///
    RingBuffer(Size size) noexcept
    :
        _size(size),
        _readPos(0),
        _writePos(0),
        _count(0),
        _data(nullptr)
    {
        if (_size > 0) {
            _data = new Element[size];
        }
    }
    
    /// dtor
    ///
    ~RingBuffer() noexcept {
        delete[] _data;
    }
    
public:
    /// Check if this buffer is disabled.
    ///
    /// This is the case if the size of the buffer is zero.
    ///
    /// @return `true` if the buffer is disabled.
    ///
    bool isDisabled() const noexcept {
        return _size == 0;
    }

    /// Check if this buffer is enabled.
    ///
    /// This is the case if the size of the buffer is non-zero.
    ///
    /// @return `true` if the buffer is enabled.
    ///
    bool isEnabled() const noexcept {
        return _size != 0;
    }

    /// Get the size of the buffer.
    ///
    /// @return The size of the buffer.
    ///
    Size getSize() const noexcept {
        return _size;
    }
    
    /// Get the number of elements in the buffer.
    ///
    /// @return The number of elements in the buffer.
    ///
    Size getCount() const noexcept {
        return _count;
    }
    
    /// Check if the buffer if empty.
    ///
    /// @return `true` if the buffer is empty.
    ///
    bool isEmpty() const noexcept {
        return _count == 0;
    }
    
    /// Write elements to the buffer.
    ///
    /// If you write more bytes as the buffer can hold, only the last
    /// elements in the buffer are actually copied.
    ///
    /// @param elements A pointer to the array with elements.
    /// @param count The number of elements in the array.
    ///
    void write(const Element *elements, Size count) {
        // Ignore empty calls.
        if (count == 0 || elements == nullptr) {
            return;
        }
        // If more elements written as the size of the buffer, just
        // use the last `_size` elements.
        if (count > _size) {
            elements += (count - _size);
            count = _size;
        }
        // Copy the first batch of elements.
        auto elementsToCopy = IntegerMath::min<Size>(static_cast<Size>(_size - _writePos), count);
        std::memcpy(
            (_data + _writePos),
            elements,
            static_cast<size_t>(sizeof(Element)*elementsToCopy));
        // Do we need a second batch?
        if (elementsToCopy < count) {
            elements += elementsToCopy;
            elementsToCopy = (count - elementsToCopy);
            std::memcpy(
                _data,
                elements,
                sizeof(Element)*elementsToCopy);
        }
        // Advance the positions.
        IntegerMath::ringIncrement(_writePos, count, _size);
        const auto overflow = IntegerMath::addWithOverflow(_count, count, _size);
        IntegerMath::ringIncrement(_readPos, overflow, _size);
    }
    
    /// Read elements from the buffer.
    ///
    /// @param elements The buffer for the elements to read.
    /// @param count The maximum number of elements to read into the buffer.
    /// @return The actual number of elements stored in the buffer.
    ///
    Size read(Element *elements, Size count) {
        // Check if no read is possible.
        if (count == 0 || elements == nullptr || _count == 0) {
            return 0;
        }
        // Limit the number of read elements if we have not enough.
        if (count > _count) {
            count = _count;
        }
        // Copy the first batch.
        auto elementsToCopy = IntegerMath::min<Size>(static_cast<Size>(_size - _readPos), count);
        std::memcpy(
            elements,
            (_data + _readPos),
            static_cast<size_t>(sizeof(Element)*elementsToCopy));
        // Do we need a second batch?
        if (elementsToCopy < count) {
            elements += elementsToCopy;
            elementsToCopy = (count - elementsToCopy);
            std::memcpy(
                elements,
                _data,
                sizeof(Element)*elementsToCopy);
        }
        // Advance the positions.
        IntegerMath::ringIncrement(_readPos, count, _size);
        _count -= count;
        return count;
    }
    
    /// Read elements from the buffer, up to a given element.
    ///
    /// This function reads elements as the regular `read` method, but
    /// if an element which equals `endMark` was read, reading is stopped.
    /// The end mark is always copied to the read buffer.
    ///
    /// To see if the end mark could be read, just compare the last
    /// read element in the buffer.
    ///
    /// @param elements The buffer for the elements to read.
    /// @param count The maximum number of elements to read into the buffer.
    /// @param endMark The element which marks the end to read.
    /// @return The actual number of elements stored in the buffer.
    ///
    Size readToEnd(Element *elements, Size count, Element endMark) {
        // Check if no read is possible.
        if (count == 0 || elements == nullptr || _count == 0) {
            return 0;
        }
        // Limit the number of read elements if we have not enough.
        if (count > _count) {
            count = _count;
        }
        // Copy the first batch.
        const auto firstElementsToCopy = IntegerMath::min<Size>(static_cast<Size>(_size - _readPos), count);
        auto readPtr = (_data + _readPos);
        for (Size i = 0; i < firstElementsToCopy; ++i) {
            elements[i] = readPtr[i];
            if (readPtr[i] == endMark) {
                count = (i + 1);
                goto readToEndFinished;
            }
        }
        // Do we need a second batch?
        if (firstElementsToCopy < count) {
            elements += firstElementsToCopy;
            const auto secondElementsToCopy = (count - firstElementsToCopy);
            for (Size i = 0; i < secondElementsToCopy; ++i) {
                elements[i] = _data[i];
                if (_data[i] == endMark) {
                    count = (firstElementsToCopy + i + 1);
                    goto readToEndFinished;
                }
            }
        }
    readToEndFinished:
        // Advance the positions.
        IntegerMath::ringIncrement<Size>(_readPos, count, _size);
        _count -= count;
        return count;
    }
    
    /// Reset the buffer.
    ///
    void reset() {
        _readPos = 0;
        _writePos = 0;
        _count = 0;
    }
    
private:
    const Size _size; ///< The size of the ring buffer.
    Size _readPos; ///< The read position.
    Size _writePos; ///< The write position.
    Size _count; ///< The number of elements in the buffer.
    Element *_data; ///< The actual buffer.
};

    
}


