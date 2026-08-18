/*
 * Author(s) of this file:
 *   (see top-level README.md)
 */
#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R           param->p[0]
#define SIGMA_REL   param->p[1]
#define PHI         param->p[2]
#define TAU         param->p[3]
#define DIST        param->p[4]
#define NCLASS      param->p[5]
#define T_SHELL     param->p[6]
#define RHO_CORE    param->p[7]
#define RHO_SHELL   param->p[8]
#define RHO_SOLV    param->p[9]
// only used when DIST == RSHS_DIST_BILOGNORMAL (5): R/SIGMA_REL above
// double as mode 1's (Rmean1,srel1); these three add mode 2 + weight.
#define RMEAN2      param->p[10]
#define SREL2       param->p[11]
#define W1          param->p[12]

// Fixed defaults for the BiLogNorm adaptive integration range (see
// rshs_make_classes_bilognormal() in robertus_shs_core.h). moment=6
// matches the ~D^6 forward-scattering weighting of the sphere/
// core-shell form factor used here; n_percent=0.0001 matches SASfit's
// own find_integration_range() (src/sasfit_old/sasfit.c). Not exposed
// as fit parameters -- change here if you need different values.
#define RSHS_BILOGNORM_MOMENT     6.0
#define RSHS_BILOGNORM_N_PERCENT  0.0001

static rshs_plugin_cache g_cache = {0};

scalar sasfit_ff_RobertusSHS_CoreShell(scalar q, sasfit_param * param)
{
	int dist, nclass, cache_err;
	double Dmean1, Dmean2;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0", q);
	SASFIT_CHECK_COND1((PHI <= 0.0 || PHI >= 1.0), param, "phi(%lg) out of (0,1)", PHI);

	dist   = (int)(DIST + 0.5);
	nclass = (int)(NCLASS + 0.5);
	if (nclass < 3) nclass = 3;
	if (nclass > RSHS_MAXP) nclass = RSHS_MAXP;

	// R/RMEAN2 are RADII -- the user-facing convention here, matching
	// t_shell/rho_* etc. which are already radius-scale quantities.
	// rshs_make_classes()/_bilognormal() are diameter-based internally
	// (see robertus_shs_core.h: the Percus-Yevick contact-distance
	// formulas, e.g. r1[i][j]=0.5*(D_i+D_j) in rshs_solve(), are
	// fundamentally diameter formulas), so convert immediately here
	// and nowhere else -- nothing downstream of this point needs to
	// know a radius was ever involved. SIGMA_REL/SREL2 need no
	// conversion: they are already dimensionless ratios (sigma/mean),
	// invariant to the *2 scaling of both numerator and denominator.
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
		// rshs_last_error() already holds a specific, human-readable reason
		// (e.g. "no real solution found for lambda(i,j) ..." when tau is
		// below the critical tau_c at this density -- a genuine, expected
		// numerical/physical outcome, not a bug) -- surface it through
		// SASfit's own error channel instead of failing silently.
		sasfit_param_set_err(param, DBGINFO(SASFIT_ERR_PREFIX "%s !\n"), rshs_last_error());
		return SASFIT_RETURNVAL_ON_ERROR;
	}

	// rshs_last_solver_note() is non-empty exactly once, right after a
	// call that needed rshs_solve()'s automatic GSL fallback (KINSOL
	// failed to converge but the fallback still found a correct answer)
	// -- not an error (the result is fine), so this goes to the
	// informational channel (sasfit_out), not sasfit_err. One-shot by
	// design (see robertus_shs_core.h), so this does not repeat on every
	// subsequent cached q-point evaluation at the same parameters.
	{
		const char *note = rshs_last_solver_note();
		if (note[0]) sasfit_out("%s\n", note);
	}

	return (scalar) rshs_intensity_coreshell(&g_cache.sys, q,
	                                          T_SHELL, RHO_CORE, RHO_SHELL, RHO_SOLV);
}

scalar sasfit_ff_RobertusSHS_CoreShell_f(scalar q, sasfit_param * param)
{
	// This model combines the multicomponent interaction physics with the
	// particle form factor internally (Eq. 14 of the paper), so there is
	// no single well-defined per-particle scattering amplitude to return
	// separately -- stubbed to 0.0. (If a genuine amplitude is ever
	// needed, e.g. sqrt of the q->0 forward-scattering limit could serve
	// as an approximation, but that is not a real scattering amplitude
	// for a multicomponent, interacting system in general.)
	return 0.0;
}

scalar sasfit_ff_RobertusSHS_CoreShell_v(scalar q, sasfit_param * param, int dist)
{
	// No single particle volume applies either, for the same reason as
	// _f above -- this model describes an interacting multicomponent
	// ensemble, not one particle of one fixed size. Stubbed to 0.0.
	return 0.0;
}
