#include <iostream>
#include <string>
#include <cstdint>
#include "genpoints.h"
#include "sobol.h"
#include "faure05.h"
#include "owenhash.h"
#include "pcg.h"

constexpr double S = double(1.0/(1ull<<32));

uint32_t hash(uint32_t x)
{
    // finalizer from murmurhash3
    x ^= x >> 16;
    x *= 0x85ebca6bu;
    x ^= x >> 13;
    x *= 0xc2b2ae35u;
    x ^= x >> 16;
    return x;
}

void genpoint_random(uint32_t n, uint32_t dim, uint32_t seed, double* x)
{    
    seed = hash(seed);
    *x = (sobol(n,dim)) * S;
}

void genpoint_sobol(uint32_t n, uint32_t dim, uint32_t seed, double* x)
{    
    *x = (sobol(n,dim)) * S;
}


void genpoint_sobol_rds(uint32_t n, uint32_t dim, uint32_t seed, double* x)
{    
    uint32_t th_seed = hash(seed);
	th_seed = hash_combine(th_seed, hash(dim));
    *x = (sobol(n,dim) ^ th_seed) * S;
}

void genpoint_sobol_owen(uint32_t n, uint32_t dim, uint32_t seed, double* x)
{    
    uint32_t th_seed = hash(seed);
    uint32_t index = nested_uniform_scramble_base2(n, th_seed);
    *x = nested_uniform_scramble_base2(sobol(index, dim), hash_combine(th_seed, dim)) * S;
}

void genpoint_laine_karras(uint32_t n, uint32_t dim, uint32_t seed, double* x)
{    
    uint32_t th_seed = hash(seed);
    th_seed = hash_combine(th_seed, hash(dim));
    *x = laine_karras_permutation(reverse_bits(n), th_seed) * S;
}

void genpoint_faure05(uint32_t n, uint32_t dim, uint32_t seed, double* x)
{  
    std::vector<int> digits;
    digits.resize(13);
	extractDigits(n, 5, digits);
    faure05(dim, digits);
    *x = radicalInverse(5, digits);
}

void genpoint_faure05_owen(uint32_t n, uint32_t dim, uint32_t seed, double* x)
{    
    uint32_t th_seed = hash(seed);
    std::vector<int> digits;
    digits.resize(13);
    extractDigits(n, 5, digits);
    faure05(dim, digits);
    nested_uniform_scramble(5, digits.size(), digits.data(), th_seed);
    *x = radicalInverse(5, digits);
}


void genpoints(const char* seqname, uint32_t n, uint32_t dim, uint32_t seed, double* x)
{
    
    seed = hash(seed);

    std::string seq(seqname);
    if (seq == "random") {
        pcg32_random_t rng;
        pcg32_srandom_r(&rng, hash_combine(seed, dim), 0);
        for (uint32_t i = 0; i < n; i++)
            x[i] = pcg32_random_r(&rng) * S;
    }
    else if (seq == "sobol") {
        for (uint32_t i = 0; i < n; i++)
            x[i] = (sobol(i,dim)) * S;
    }
    else if (seq == "sobol_rds") {
        seed = hash_combine(seed, hash(dim));
        for (uint32_t i = 0; i < n; i++)
            x[i] = (sobol(i,dim) ^ seed) * S;
    }
    else if (seq == "sobol_owen") {
        for (uint32_t i = 0; i < n; i++) {
            uint32_t index = nested_uniform_scramble_base2(i, seed);
            x[i] = nested_uniform_scramble_base2(sobol(index, dim), hash_combine(seed, dim)) * S;
        }
    }
    else if (seq == "laine_karras") {
        seed = hash_combine(seed, hash(dim));
        for (uint32_t i = 0; i < n; i++)
            x[i] = laine_karras_permutation(reverse_bits(i), seed) * S;
    }
    else if (seq == "faure05") {
        std::vector<int> digits;
        digits.resize(13);
        for (uint32_t i = 0; i < n; i++) {
            extractDigits(i, 5, digits);
            faure05(dim, digits);
            x[i] = radicalInverse(5, digits);
        }
    }
    else if (seq == "faure05_owen") {
        std::vector<int> digits;
        digits.resize(13);
        for (uint32_t i = 0; i < n; i++) {
            extractDigits(i, 5, digits);
            faure05(dim, digits);
            nested_uniform_scramble(5, digits.size(), digits.data(), seed);
            x[i] = radicalInverse(5, digits);
        }
    }
    else {
        std::cerr << "unknown sequence: " << seq << "\n";
        abort();
    }
}
