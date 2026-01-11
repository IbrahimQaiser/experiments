#pragma once
#include <cstddef>
#include <ostream>
#include <stdexcept>
#include <stdint.h>
#include <vector>
// This file contains functions related to bit manipulation, bit streams, bit reader and writer


using bits = std::vector<bool>;
using bytes = std::vector<uint8_t>;

// If bit is 0, returns l as it is
// If bit is 1, add the bit to l and return the result
inline bits add(bits l, int bit)
{
    if (l.empty())
        throw std::logic_error("Bits is empty");

    if (bit == 0)
    {
        return l;
    }
    else if (bit == 1)
    {
        int carry = 1;
        for (int i = static_cast<int>(l.size() - 1); i >= 0; i--)
        {
            int sum = l[i] + carry;
            l[i] = sum % 2;
            if (sum <= 1)
            {
                // Carry has not been generated
                return l;
            }
        }
        // If control reaches here, it means that overflow has occured
        l.insert(l.begin(), 1);
        return l;
    }
    else
    {
        throw std::logic_error("Bit can only be 0 or 1");
    }
}

// Adds shift number of zeroes after value, thereby shifting it to the left
inline bits left_shift(bits value, int shift)
{
    for (int i = 0; i < shift; i++)
        value.push_back(0);
    return value;
}

inline std::ostream &operator<<(std::ostream &os, const bits &b)
{
    for (const auto &bitval : b)
        os << bitval;
    return os;
}

class BitStreamWriter
{
    bits stream;

  public:
    void write(bool bit) { stream.push_back(bit); }

    void write(const bits &b) { stream.insert(stream.end(), b.begin(), b.end()); }

    // Return the stream as bytes, after calling this function, the stream is cleared
    bytes as_bytes()
    {
        // Pad with extra zero bits at the end
        while (stream.size() % 8 != 0)
            stream.push_back(0);

        bytes result;
        result.reserve(stream.size() / 8);
        uint8_t temp_byte = 0;
        for (uint64_t i = 0; i < stream.size(); i++)
        {
            if (i != 0 && i % 8 == 0)
            {
                result.push_back(temp_byte);
                temp_byte = 0;
            }
            temp_byte <<= 1;
            temp_byte |= stream[i];

            // Check if this is the last byte
            if ((i + 1) == stream.size())
            {
                result.push_back(temp_byte);
            }
        }
        clear();
        return result;
    }

    void clear() { stream.clear(); }
};

// TODO: Use an iterator, instead of expanding all bits of every byte
class BitStreamReader
{
    bits stream;
    uint64_t stream_pointer;

  public:
    BitStreamReader() : stream_pointer(0) {}

    // Returns the bit 0/1 or -1 if read fails
    int8_t read()
    {
        if (stream_pointer == stream.size())
            return -1;
        return stream[stream_pointer++];
    }

    // Reset the stream pointer, does not cause the stream to lose content
    void reset() { stream_pointer = 0; }

    void clear()
    {
        stream.clear();
        stream_pointer = 0;
    }

    // Converts the sequence of bytes to a bit stream
    void from_bytes(const bytes &b)
    {
        stream.reserve(b.size() * 8);
        for (const auto &byte : b)
        {
            from_byte(byte);
        }
    }

    void from_byte(uint8_t byte)
    {
        stream.push_back((byte & 0x80) >> 7);
        stream.push_back((byte & 0x40) >> 6);
        stream.push_back((byte & 0x20) >> 5);
        stream.push_back((byte & 0x10) >> 4);
        stream.push_back((byte & 0x8) >> 3);
        stream.push_back((byte & 0x4) >> 2);
        stream.push_back((byte & 0x2) >> 1);
        stream.push_back((byte & 0x1));
    }
};

inline bytes to_little_endian(uint64_t value)
{
    bytes result;
    for (int i = 0; i < 8; ++i)
    {
        result.push_back(static_cast<uint8_t>(value & 0xFF));
        value >>= 8;
    }
    return result;
}

inline uint64_t from_little_endian_to_uint64_t(const std::vector<uint8_t> &vec)
{
    if (vec.size() != 8)
    {
        throw std::invalid_argument("vec must be of size 8");
    }

    uint64_t result = 0;
    for (int i = 0; i < 8; ++i)
    {
        result |= static_cast<uint64_t>(vec[i]) << (i * 8);
    }

    return result;
}
