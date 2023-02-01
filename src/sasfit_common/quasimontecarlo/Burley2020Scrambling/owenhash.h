#pragma once
#include <cstdint>

inline uint32_t hash3(uint32_t i, uint32_t j, uint32_t k)
{
    return hash_combine(hash_combine(i,j),k);
}

#define maxbase 541

inline int permute(int b, int x, uint32_t seed)
{
    int perm[maxbase];
    perm[0] = 0;
    for (int i = 1; i < b; i++) {
        seed = seed * 1664525 + 1013904223;
        uint32_t j = seed % (i+1);
        perm[i] = perm[j];
        perm[j] = i;
    }
    return perm[x];
}


inline void nested_linear_scramble(int b, int k, int x[], uint32_t seed)
{
    uint32_t interval = 0;
    for (int j = 0; j < k; j++) {
        uint32_t r = hash3(seed,interval,j);
        int g = r%b, h = 1 + ((r/b) % (b-1));
        interval = interval * b + x[j];
        x[j] = (h * x[j] + g) % b;
    }
}


inline void nested_uniform_scramble(int b, int k, int x[], uint32_t seed)
{
    uint32_t interval = 0;
    for (int j = 0; j < k; j++) {
        uint32_t r = hash3(seed,interval,j);
        interval = interval * b + x[j];
        x[j] = permute(b,x[j],r);
    }
}
