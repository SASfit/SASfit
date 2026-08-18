/*
 * Author(s) of this file:
 *   (see top-level README.md)
 */
#include "include/private.h"
#include <sasfit_error_sq.h>

#define RADIUS         param->p[0]
#define LAMBDA1        param->p[1]
#define LAMBDA2        param->p[2]
#define LAMBDA3        param->p[3]
#define EPS1_OVER_KT   param->p[4]
#define EPS2_OVER_KT   param->p[5]
#define EPS3_OVER_KT   param->p[6]
#define VOLFRACTION    param->p[7]

static _Thread_local RFAPluginCache g_cache_RFA2_3step;
static _Thread_local int g_cacheInitialized_RFA2_3step = 0;

scalar sasfit_sq_RFA2_3step(scalar q, sasfit_param * param)
{
    SASFIT_ASSERT_PTR(param);
    SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0", q);
    SASFIT_CHECK_COND1((RADIUS <= 0.0), param, "RADIUS(%lg) <= 0", RADIUS);
    SASFIT_CHECK_COND1((LAMBDA1 <= 1.0), param, "LAMBDA1(%lg) <= 1", LAMBDA1);
    SASFIT_CHECK_COND1((LAMBDA2 <= LAMBDA1), param, "LAMBDA2(%lg) <= LAMBDA1", LAMBDA2);
    SASFIT_CHECK_COND1((LAMBDA3 <= LAMBDA2), param, "LAMBDA3(%lg) <= LAMBDA2", LAMBDA3);
    SASFIT_CHECK_COND1((VOLFRACTION <= 0.0), param, "VOLFRACTION(%lg) <= 0", VOLFRACTION);
    SASFIT_CHECK_COND1((VOLFRACTION >= 1.0), param, "VOLFRACTION(%lg) >= 1", VOLFRACTION);

    double lambda_steps[3] = { LAMBDA1, LAMBDA2, LAMBDA3 };
    double beps_steps[3]   = { EPS1_OVER_KT, EPS2_OVER_KT, EPS3_OVER_KT };
    return rfa_common_sq_n(q, 3, RADIUS, lambda_steps, beps_steps, VOLFRACTION,
                            rfa2_solve_warm, &g_cache_RFA2_3step, &g_cacheInitialized_RFA2_3step,
                            "RFA2_3step");
}

scalar sasfit_sq_RFA2_3step_f(scalar q, sasfit_param * param)
{
    (void)q; (void)param;
    return 0.0; // insert your code here
}

scalar sasfit_sq_RFA2_3step_v(scalar q, sasfit_param * param, int dist)
{
    (void)q; (void)param; (void)dist;
    return 0.0; // insert your code here
}

/* Frees this thread's cached RFA2_3step solution, if any -- see the
 * long comment on rfa_common_release_cache() in
 * include/sasfit_sq_RFA_common.h. */
void sasfit_sq_RFA2_3step_release_thread_cache(void)
{
    rfa_common_release_cache(&g_cache_RFA2_3step, &g_cacheInitialized_RFA2_3step);
}
