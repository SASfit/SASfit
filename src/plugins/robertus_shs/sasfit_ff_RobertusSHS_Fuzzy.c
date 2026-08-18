/*
 * Author(s) of this file:
 *   (see top-level README.md)
 */
#include "include/private.h"
#include <sasfit_error_ff.h>

#define R             param->p[0]
#define SIGMA_REL     param->p[1]
#define PHI           param->p[2]
#define TAU           param->p[3]
#define DIST          param->p[4]
#define NCLASS        param->p[5]
#define SIGMA_FUZZY   param->p[6]
#define RHO_PARTICLE  param->p[7]
#define RHO_SOLV      param->p[8]
// only used when DIST == RSHS_DIST_BILOGNORMAL (5): R/SIGMA_REL above
// double as mode 1's (Rmean1,srel1); these three add mode 2 + weight.
#define RMEAN2        param->p[9]
#define SREL2         param->p[10]
#define W1            param->p[11]

// Fixed defaults for the BiLogNorm adaptive integration range -- see
// the identical block in sasfit_ff_RobertusSHS_CoreShell.c for the
// rationale; kept in sync with that file.
#define RSHS_BILOGNORM_MOMENT     6.0
#define RSHS_BILOGNORM_N_PERCENT  0.0001

static rshs_plugin_cache g_cache = {0};

scalar sasfit_ff_RobertusSHS_Fuzzy(scalar q, sasfit_param * param)
{
	int dist, nclass, cache_err;
	double Dmean1, Dmean2;

	SASFIT_ASSERT_PTR(param);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0", q);
	SASFIT_CHECK_COND1((PHI <= 0.0 || PHI >= 1.0), param, "phi(%lg) out of (0,1)", PHI);

	dist   = (int)(DIST + 0.5);
	nclass = (int)(NCLASS + 0.5);
	if (nclass < 3) nclass = 3;
	if (nclass > RSHS_MAXP) nclass = RSHS_MAXP;

	// see the identical block in sasfit_ff_RobertusSHS_CoreShell.c
	Dmean1 = 2.0 * R;
	Dmean2 = 2.0 * RMEAN2;

	if (dist == RSHS_DIST_BILOGNORMAL) {
		SASFIT_CHECK_COND1((W1 < 0.0 || W1 > 1.0), param, "w1(%lg) out of [0,1]", W1);
		cache_err = rshs_plugin_cache_refresh_bilognormal(&g_cache, Dmean1, SIGMA_REL,
		                                                   Dmean2, SREL2, W1, PHI, TAU, nclass,
		                                                   RSHS_BILOGNORM_MOMENT, RSHS_BILOGNORM_N_PERCENT);
	} else {
		cache_err = rshs_plugin_cache_refresh(&g_cache, Dmean1, SIGMA_REL, PHI, TAU, dist, nclass);
	}

	if (cache_err != 0) {
		// == 0.0 for form factors (sasfit_error_ff.h)
		sasfit_param_set_err(param, DBGINFO(SASFIT_ERR_PREFIX "%s !\n"), rshs_last_error());
		return SASFIT_RETURNVAL_ON_ERROR;
	}

	// see the identical block in sasfit_ff_RobertusSHS_CoreShell.c
	{
		const char *note = rshs_last_solver_note();
		if (note[0]) sasfit_out("%s\n", note);
	}

	return (scalar) rshs_intensity_fuzzy(&g_cache.sys, q,
	                                      SIGMA_FUZZY, RHO_PARTICLE, RHO_SOLV);
}

scalar sasfit_ff_RobertusSHS_Fuzzy_f(scalar q, sasfit_param * param)
{
	return 0.0; // see sasfit_ff_RobertusSHS_CoreShell_f
}

scalar sasfit_ff_RobertusSHS_Fuzzy_v(scalar q, sasfit_param * param, int dist)
{
	return 0.0; // see sasfit_ff_RobertusSHS_CoreShell_v
}
