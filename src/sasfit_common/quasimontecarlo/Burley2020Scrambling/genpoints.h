#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#pragma once
#include "stdint.h"

EXTERNC  void genpoints(const char* seq, uint32_t n, uint32_t dim, uint32_t seed, double* x);
EXTERNC  void genpoint_random(           uint32_t n, uint32_t dim, uint32_t seed, double* x);
EXTERNC  void genpoint_sobol(            uint32_t n, uint32_t dim, uint32_t seed, double* x);
EXTERNC void genpoint_sobol_rds(        uint32_t n, uint32_t dim, uint32_t seed, double* x);
EXTERNC void genpoint_sobol_owen(       uint32_t n, uint32_t dim, uint32_t seed, double* x);
EXTERNC void genpoint_laine_karras(     uint32_t n, uint32_t dim, uint32_t seed, double* x);
EXTERNC void genpoint_faure05(          uint32_t n, uint32_t dim, uint32_t seed, double* x);
EXTERNC void genpoint_faure05_owen(     uint32_t n, uint32_t dim, uint32_t seed, double* x);

#undef EXTERNC