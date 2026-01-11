#pragma once
#include <stdint.h>

struct Symbol
{
    uint8_t sym;

    bool operator<(const Symbol &other) const { return sym < other.sym; }
};
