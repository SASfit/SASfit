
#pragma once

#include <cstdint>
#include "radicalinverse.h"

extern uint8_t faure05matrices[5][13][13];

inline void faure05(int dim, std::vector<int>& digits)
{
    if (dim >= 5) return;
    int temp[13];
    for (int d = 0; d < 13; d++) {
        temp[d] = 0;
        for (int j = 0; j < 13; j++) {
            temp[d] += faure05matrices[dim][d][j] * digits[j];
        }
        temp[d] %= 5;
    }
    for (int d = 0; d < 13; d++) { digits[d] = temp[d]; }
}
