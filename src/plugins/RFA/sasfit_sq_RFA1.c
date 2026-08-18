/*
 * Author(s) of this file:
 *   (see top-level README.md)
 */
#include "include/private.h"
#include <sasfit_error_sq.h>

#define RADIUS            param->p[0]
#define LAMBDA            param->p[1]
#define EPSILON_OVER_KT   param->p[2]
#define VOLFRACTION       param->p[3]

/* File scope, not function-local: sasfit_sq_RFA1_release_thread_cache()
 * below needs to reach the SAME cache that sasfit_sq_RFA1() populates --
 * a function-local `static` of the same name in a different function
 * would be a distinct object, not shared, which was a real bug caught
 * and fixed here before this shipped (see the flat, non-plugin-structured
 * predecessor of this file, and the main project's own README "Bugs
 * found" for the general class of _Thread_local pitfalls this project
 * has run into more than once). */
static _Thread_local RFAPluginCache g_cache_RFA1;
static _Thread_local int g_cacheInitialized_RFA1 = 0;

scalar sasfit_sq_RFA1(scalar q, sasfit_param * param)
{
    SASFIT_ASSERT_PTR(param);
    SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0", q);
    SASFIT_CHECK_COND1((RADIUS <= 0.0), param, "RADIUS(%lg) <= 0", RADIUS);
    SASFIT_CHECK_COND1((LAMBDA <= 1.0), param, "LAMBDA(%lg) <= 1", LAMBDA);
    SASFIT_CHECK_COND1((VOLFRACTION <= 0.0), param, "VOLFRACTION(%lg) <= 0", VOLFRACTION);
    SASFIT_CHECK_COND1((VOLFRACTION >= 1.0), param, "VOLFRACTION(%lg) >= 1", VOLFRACTION);
    /* Note: SASFIT_CHECK_COND1 does not itself stop execution here --
     * see sasfit_common.h's header comment. The checks above are kept
     * for consistency with real SASfit plugin convention (and for
     * whatever error reporting the real macro provides); the actual
     * early-exit on bad input happens inside rfa_common_sq_n(), via
     * rfa_potential_init()'s own validation. */

    double lambda_steps[1] = { LAMBDA };
    double beps_steps[1]   = { EPSILON_OVER_KT };
    return rfa_common_sq_n(q, 1, RADIUS, lambda_steps, beps_steps, VOLFRACTION,
                            rfa1_solve_warm, &g_cache_RFA1, &g_cacheInitialized_RFA1, "RFA1");
}

scalar sasfit_sq_RFA1_f(scalar q, sasfit_param * param)
{
    /* Real precedent (baba_ahmed/sasfit_sq_baba_ahmed_1.c): a structure
     * factor's "_f" (scattering amplitude) variant is stubbed to 0.0,
     * since "amplitude" is not a meaningful concept for a structure
     * factor -- this is not an oversight, it's what SASfit's own
     * structure-factor plugins actually do. */
    (void)q; (void)param;
    return 0.0; // insert your code here
}

scalar sasfit_sq_RFA1_v(scalar q, sasfit_param * param, int dist)
{
    /* Same reasoning as sasfit_sq_RFA1_f() above: "particle volume" is
     * not a meaningful concept for a structure factor either. */
    (void)q; (void)param; (void)dist;
    return 0.0; // insert your code here
}

/* Frees this thread's cached RFA1 solution, if any. Call this before a
 * fitting thread that has used sasfit_sq_RFA1() exits -- see the long
 * comment on rfa_common_release_cache() in include/sasfit_sq_RFA_common.h
 * for why this is needed at all and how much it matters in practice.  */
void sasfit_sq_RFA1_release_thread_cache(void)
{
    rfa_common_release_cache(&g_cache_RFA1, &g_cacheInitialized_RFA1);
}
