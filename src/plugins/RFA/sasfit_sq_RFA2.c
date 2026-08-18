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

static _Thread_local RFAPluginCache g_cache_RFA2;
static _Thread_local int g_cacheInitialized_RFA2 = 0;

scalar sasfit_sq_RFA2(scalar q, sasfit_param * param)
{
    SASFIT_ASSERT_PTR(param);
    SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0", q);
    SASFIT_CHECK_COND1((RADIUS <= 0.0), param, "RADIUS(%lg) <= 0", RADIUS);
    SASFIT_CHECK_COND1((LAMBDA <= 1.0), param, "LAMBDA(%lg) <= 1", LAMBDA);
    SASFIT_CHECK_COND1((VOLFRACTION <= 0.0), param, "VOLFRACTION(%lg) <= 0", VOLFRACTION);
    SASFIT_CHECK_COND1((VOLFRACTION >= 1.0), param, "VOLFRACTION(%lg) >= 1", VOLFRACTION);

    double lambda_steps[1] = { LAMBDA };
    double beps_steps[1]   = { EPSILON_OVER_KT };
    return rfa_common_sq_n(q, 1, RADIUS, lambda_steps, beps_steps, VOLFRACTION,
                            rfa2_solve_warm, &g_cache_RFA2, &g_cacheInitialized_RFA2, "RFA2");
}

scalar sasfit_sq_RFA2_f(scalar q, sasfit_param * param)
{
    (void)q; (void)param;
    return 0.0; // insert your code here
}

scalar sasfit_sq_RFA2_v(scalar q, sasfit_param * param, int dist)
{
    (void)q; (void)param; (void)dist;
    return 0.0; // insert your code here
}

/* Frees this thread's cached RFA2 solution, if any. Call this before a
 * fitting thread that has used sasfit_sq_RFA2() exits -- see the long
 * comment on rfa_common_release_cache() in include/sasfit_sq_RFA_common.h. */
void sasfit_sq_RFA2_release_thread_cache(void)
{
    rfa_common_release_cache(&g_cache_RFA2, &g_cacheInitialized_RFA2);
}
