#pragma once
#include "bitmanip.hpp"

// This interface takes a block (inbuffer) and compresses or decompresses it to outbuffer
class BlockArchiver
{
  public:
    virtual bytes compress(bytes::const_iterator beg, bytes::const_iterator end) const = 0;
    virtual bytes decompress(bytes::const_iterator, bytes::const_iterator end) const = 0;

    virtual ~BlockArchiver() {}
};

class HuffmanArchiver : public BlockArchiver
{
  public:
    bytes compress(bytes::const_iterator beg, bytes::const_iterator end) const;
    bytes decompress(bytes::const_iterator, bytes::const_iterator end) const;
};