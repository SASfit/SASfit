#pragma once

#include <vector>
#include <cstdint>
#include <cmath>

inline float numDigitsForBase(uint32_t base)
{
    int numDigits = 0;
    for (uint64_t maxval = base; maxval < (1ull<<32); maxval *= base, numDigits++);
    return numDigits;
}

inline void extractDigits(uint32_t index, uint32_t base, std::vector<int>& digits)
{
    for (auto& digit : digits) {
        digit = index % base; index /= base;
    }
}

inline float radicalInverse(uint32_t base, std::vector<int>& digits)
{
    uint32_t result = 0;
    uint32_t maxValue = 1;
    for (auto& digit : digits) {
        result *= base;
        result += digit;
        maxValue *= base;
    }
    return result/(maxValue+1.0);
}

