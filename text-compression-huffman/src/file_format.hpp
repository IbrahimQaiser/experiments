#pragma once
#include "bitmanip.hpp"

class FileFormatHeader
{
  public:
    virtual bytes create_header() const = 0;

    virtual bool verify_header(bytes::const_iterator beg, bytes::const_iterator end) const = 0;

    virtual uint64_t size() const = 0;

    virtual ~FileFormatHeader() {}
};

class FileFormatHeaderV1 : public FileFormatHeader
{
  public:
    bytes create_header() const
    {
        bytes buffer;
        // Write the File magic number
        buffer.push_back(static_cast<uint8_t>(0xC));
        buffer.push_back(static_cast<uint8_t>(0x0));
        buffer.push_back(static_cast<uint8_t>(0xD));
        buffer.push_back(static_cast<uint8_t>(0xE));
        // Write the file version (in little endian), 2 bytes
        buffer.push_back(static_cast<uint8_t>(1));
        buffer.push_back(static_cast<uint8_t>(0));

        // Write the 8 reserved bytes
        for (int i = 0; i < 8; i++)
            buffer.push_back(static_cast<uint8_t>(0));

        return buffer;
    }

    // Checks if the given buffer is a valid file header (for V1)
    //
    bool verify_header(bytes::const_iterator beg, bytes::const_iterator end) const
    {
        if ((end - beg) != 14)
        {
            return false;
        }
        // Check the magic number
        if (beg[0] != 0xC || beg[1] != 0x0 || beg[2] != 0xD || beg[3] != 0xE)
            return false;
        // Check the file version
        if (beg[4] != 1)
            return false;
        if (beg[5] != 0)
            return false;
        return true;
    }

    uint64_t size() const { return 14; }
};