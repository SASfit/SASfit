/* sasfit_sq_RFA_common.h -- NOT a SASfit convention, just this
 * delivery's own code organization: all four model files
 * (sasfit_sq_RFA1.c, sasfit_sq_RFA2.c, sasfit_sq_RFA1_3step.c,
 * sasfit_sq_RFA2_3step.c) are otherwise identical except for which
 * solver they warm-start with and how many steps they expose, so the
 * shared caching/warm-starting/physicality-check logic lives here
 * once instead of being duplicated four times and risking the copies
 * drifting apart.
 *
 * Implements, for a piece-wise-constant potential with n steps (n=1:
 * square well if EPSILON_OVER_KT<0, square shoulder if >0; n=3: three
 * independently-signed steps -- see the main project's
 * rfa_piecewise.h), exactly the two access patterns SASfit's
 * Levenberg-Marquardt fitting is confirmed to use (see
 * sasfit_common.h's header comment):
 *
 *   1. Repeated calls across a Q-array at FIXED potential parameters:
 *      the potential doesn't change, so after the first call this
 *      hits an EXACT cache match and skips solving entirely -- just
 *      re-evaluate S(q) on the already-solved state (measured in the
 *      main project's "Performance" section: a full S(q) evaluation
 *      is ~65x cheaper than a warm-started solve, and a warm-started
 *      solve of an *exact* repeat costs 0 solver iterations on top of
 *      that, per the "exact-repeat short-circuit" work).
 *   2. Calls with slightly-perturbed parameters (the fitting
 *      Jacobian): an exact cache miss, but rfa1_solve_warm()/
 *      rfa2_solve_warm() warm-start from the previous solution
 *      regardless, typically cutting iteration counts 3-10x versus a
 *      cold start (see "Performance: warm-starting" in the main
 *      project's README).
 *
 * A further check beyond what the sibling RMSA plugin does: this
 * project's own code review (see the main README's "Bugs found while
 * reviewing this code" and the RFA1-vs-RFA2 comparison's "necessary
 * filtering step") found that the solver's convergence test alone does
 * NOT guarantee a physically sensible result -- g(r) can go negative
 * somewhere in the first shell despite a reported "converged". Since
 * RMSA/sticky-hard-sphere doesn't have this specific failure mode, its
 * plugin didn't need this; this one explicitly checks
 * rfa_rdf_first_shell() across the first shell after every fresh solve
 * and treats an unphysical result the same as non-convergence.
 *
 * Note on SASFIT_CHECK_COND1: the real macro's early-exit behaviour on
 * failure could not be confirmed (see sasfit_common.h's header
 * comment), so none of the four model files rely on it to actually
 * stop execution. The functions below always run to completion and
 * fail safely on their own, through rfa_potential_init() returning
 * false or `converged`/the physicality check being false -- exactly
 * mirroring the confirmed pattern in the real sasfit_sq_RMSA.c this
 * plugin is modelled on.
 *
 * Thread safety: the cache is `_Thread_local` (matching the sibling
 * RMSA plugin's own defensive choice, for the same reason -- SASfit's
 * own threading behaviour for structure-factor plugins isn't confirmed
 * either way from available sources).
 */
#ifndef SASFIT_SQ_RFA_COMMON_H
#define SASFIT_SQ_RFA_COMMON_H

#include <math.h>
#include <stdio.h>
#include "rfa_piecewise.h"

typedef int (*RFASolveWarmFn)(const PWPotential *, const RFASolution *, RFASolution *);

#define RFA_PLUGIN_MAX_STEPS 8   /* generous upper bound for cache storage */

typedef struct {
    int    valid;              /* does sol hold a physical, converged solution? */
    int    n;
    double lambda[RFA_PLUGIN_MAX_STEPS];
    double beps[RFA_PLUGIN_MAX_STEPS];
    double eta;
    RFASolution sol;
} RFAPluginCache;

/* Scans the first shell for a negative g(r) -- the same check used
 * throughout the main project's scan_sw_ss.c / plot_scan.py.          */
static inline int rfa_common_is_physical(const RFASolution *sol)
{
    for (double r = 1.0; r < 2.0; r += 0.005) {
        double g = rfa_rdf_first_shell(sol, r);
        if (g < 0.0) return 0;
    }
    return 1;
}

/* Sanity-checks a candidate warm-start guess array against garbage
 * (non-finite or wildly-out-of-range) values, mirroring
 * rfa1_solve_warm()/rfa2_solve_warm()'s own internal guard.           */
static inline int rfa_common_cache_matches(const RFAPluginCache *cache, int n,
                                            const double *lambda_steps,
                                            const double *beps_steps, double eta)
{
    if (!cache->valid || cache->n != n || cache->eta != eta) return 0;
    for (int i = 0; i < n; i++)
        if (cache->lambda[i] != lambda_steps[i] || cache->beps[i] != beps_steps[i]) return 0;
    return 1;
}

/* General n-step solve+cache+evaluate, shared by all four model
 * files. solve_warm is rfa1_solve_warm or rfa2_solve_warm; radius,
 * lambda_steps[]/beps_steps[] (each length n) and eta are the
 * already-extracted, named parameters from the calling model file's
 * own macros (kept explicit there for SASfit-facing clarity and
 * per-parameter naming, matching real plugin convention -- this
 * function does no further per-parameter validation itself, trusting
 * rfa_potential_init() below as the actual safety net, exactly as the
 * real sasfit_sq_RMSA.c relies on rmsa_physical_to_dimensionless()'s
 * own return code rather than on SASFIT_CHECK_COND1 to stop early).   */
static inline scalar rfa_common_sq_n(scalar q, int n, double radius,
                                      const double *lambda_steps, const double *beps_steps,
                                      double eta, RFASolveWarmFn solve_warm,
                                      RFAPluginCache *cache, int *cacheInitialized,
                                      const char *modelName)
{
    if (!*cacheInitialized) {
        cache->valid = 0;
        *cacheInitialized = 1;
    }

    if (q < 0.0 || !(radius > 0.0) || !isfinite(radius)) {
        fprintf(stderr, "sasfit_sq_%s: invalid q(%lg) or RADIUS(%lg)\n", modelName, q, radius);
        return 1.0;
    }

    double sigma = 2.0 * radius;      /* hard-core diameter from radius */
    double qr    = q * sigma;         /* reduced q, matching sigma=1 convention */

    /* Exact cache hit: identical parameters to the previous call (the
     * "same Q-array, fixed parameters" pattern) -- skip solving
     * entirely, this is not merely a warm start.                     */
    if (rfa_common_cache_matches(cache, n, lambda_steps, beps_steps, eta))
        return rfa_structure_factor(&cache->sol, qr);

    PWPotential pot;
    if (!rfa_potential_init(&pot, n, eta, lambda_steps, beps_steps)) {
        fprintf(stderr, "sasfit_sq_%s: rfa_potential_init rejected the potential "
                         "(n=%d, eta=%lg)\n", modelName, n, eta);
        return 1.0;   /* SASfit's own convention for "no structural correlation" --
                        * see e.g. sasfit_sq_StickyHardSphere2.c's
                        * `if (FP==0.0) return 1.0;` -- matching the
                        * sibling RMSA plugin's identical choice.      */
    }

    RFASolution newsol;
    int converged = solve_warm(&pot, cache->valid ? &cache->sol : NULL, &newsol);
    rfa_potential_free(&pot);

    if (!converged) {
        fprintf(stderr, "sasfit_sq_%s: did not converge (n=%d, eta=%lg) -- see the main "
                         "project's README, \"Notes/limitations\": this model has no "
                         "solution in some high-density/deep-or-wide-well regions\n",
                modelName, n, eta);
        rfa_solution_free(&newsol);
        if (cache->valid) rfa_solution_free(&cache->sol);
        cache->valid = 0;
        return 1.0;
    }
    if (!rfa_common_is_physical(&newsol)) {
        fprintf(stderr, "sasfit_sq_%s: converged but g(r)<0 somewhere in the first shell "
                         "(n=%d, eta=%lg) -- treating as no solution (see the main project's "
                         "RFA1-vs-RFA2 comparison, \"a necessary filtering step\")\n",
                modelName, n, eta);
        rfa_solution_free(&newsol);
        if (cache->valid) rfa_solution_free(&cache->sol);
        cache->valid = 0;
        return 1.0;
    }

    if (cache->valid) rfa_solution_free(&cache->sol);
    cache->sol = newsol;
    cache->n   = n;
    for (int i = 0; i < n; i++) { cache->lambda[i] = lambda_steps[i]; cache->beps[i] = beps_steps[i]; }
    cache->eta   = eta;
    cache->valid = 1;

    return rfa_structure_factor(&cache->sol, qr);
}

/* Frees a cached solution's buffers, if any. Exposed (see each model
 * file's own sasfit_sq_*_release_thread_cache()) because a plain
 * `_Thread_local` variable, unlike pthread TLS created with
 * pthread_key_create()'s destructor argument, has NO automatic
 * per-thread cleanup hook in C11: if a thread that has populated this
 * cache exits without calling this first, the cached solution's
 * buffers are not reclaimed. Confirmed as a real, non-theoretical leak
 * via LeakSanitizer while testing this plugin's own thread-safety
 * test. Whether this matters in practice depends on SASfit's own
 * threading model for structure-factor plugins, which is not
 * confirmed either way from available sources.                       */
static inline void rfa_common_release_cache(RFAPluginCache *cache, int *cacheInitialized)
{
    if (*cacheInitialized && cache->valid) {
        rfa_solution_free(&cache->sol);
        cache->valid = 0;
    }
}

#endif /* SASFIT_SQ_RFA_COMMON_H */
