// bessel.cpp
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include "FBT.h"
#include "sasfit_lru_cache.h"
extern "C"
{
    #include "sasfit_constants.h"
	#include "sasfit_function.h"
    #include "sasfit_hankel.h"
}

// Multi-slot LRU cache of FBT objects, replacing a single static slot
// that thrashed (rebuilding the Bessel-zero/weight tables from
// scratch) whenever calls alternated between more than one distinct
// (nu, option, N, Q) -- e.g. two coexisting populations evaluated
// interleaved within one fit. Same design as sasfit_qdht_cache/
// sasfit_fftlog_cache/sasfit_bestlime.cpp's own cache, generalized via
// SasfitLruCache<Key,T> (see sasfit_lru_cache.h) since FBT is a plain
// C++ object here, not something needing a C ABI.
//
// NOTE on what this does and doesn't fix: this only avoids rebuilding
// the (nu, N)-dependent zero/weight tables. It does NOT touch FBT's
// per-call cost at a given q -- with the default "option 0" (adaptive
// modified Ogata), every sasfit_FBT() call still runs a fresh
// Brent-minimization search (FBT::get_hu(), up to 1000 iterations,
// each evaluating the target function) to pick a step size for THAT
// q, because Ogata's method rescales which points of the function it
// samples based on q (see f_for_ogata: g(x/q)/q) -- unlike QDHT/FFTLog,
// there is no q-independent grid to batch across. That search is
// almost certainly the dominant per-call cost, not the table lookup
// this cache addresses; if it becomes a bottleneck, "option 2" (fixed
// h=0.05, already implemented in FBT::fbt(), skips the search
// entirely) is the more direct lever.

struct FBTKey {
    double nu = 0.0;
    int option = 0;
    int N = 10;
    double Q = 1.0;
};

static SasfitLruCache<FBTKey, FBT>& fbt_cache() {
    static SasfitLruCache<FBTKey, FBT> cache(8);
    return cache;
}

static FBT* current_fbt = nullptr;

void sasfit_set_FBT(double nu, int option, int N, double Q) {
    FBTKey key{};
    key.nu = nu;
    key.option = option;
    key.N = N;
    key.Q = Q;

    FBT* entry = fbt_cache().lookup(key);
    if (!entry) {
        entry = &fbt_cache().insert(key);
        *entry = FBT(nu, option, N, Q);
    }
    current_fbt = entry;
}

scalar sasfit_FBT(scalar x, double (*intKern_fct)(double, void *), void * fparams){
    if (!current_fbt) {
        // safety fallback if sasfit_FBT is ever called before
        // sasfit_set_FBT -- matches the original code's own defaults
        // (nu_def=0.0, option_def=0, N_def=10, Q_def=1.0)
        sasfit_set_FBT(0.0, 0, 10, 1.0);
    }
	return current_fbt->fbt(std::bind(intKern_fct, std::placeholders::_1, fparams),x);
} // test function to transform data allows to send anything else to the function
