/*
 * Author(s) of this file:
 *   (see README.md)
 */
#include "include/private.h"
#include <sasfit_error_ff.h>
#include <string.h>

// define shortcuts for local parameters/variables
#define SIGMA       param->p[0]
#define S_REL       param->p[1]
#define PHI         param->p[2]
#define ZVAL        param->p[3]
#define L_B         param->p[4]
#define ALPHA       param->p[5]
#define NCLASS      param->p[6]
#define CHARGE_EXP  param->p[7]

/* Grid: N+1 must be a power of two, because the radial transform is a DST-I
 * built from a radix-2 real FFT (see ry_polydisperse_core.c's own dst1()).
 * N = 2^12 - 1 with 100 points per sigma matches the Python OZ solver's own
 * default grid exactly, so the two implementations can be compared directly
 * -- they agree to about 1e-8 relative on S^M(q) (1e-11 on the Picard path). */
#define RYP_GRID_N            4095
#define RYP_POINTS_PER_SIGMA  100.0
#define RYP_MAX_ITERATIONS    20000
#define RYP_TOLERANCE         1e-8

/* One cached solve. A single OZ solve costs a few tenths of a second, so
 * re-solving per q-point would make any q-scan or fit unusable; instead the
 * solved system is kept and reused whenever the parameters are unchanged.
 * Successive solves also warm-start from the previous gamma, which cuts a
 * typical small parameter change substantially. Same caching idea the
 * twoyukawa and robertus_shs plugins already use, but keyed on the whole
 * parameter set rather than a fixed-size slot table.
 *
 * NOT thread-safe: one static cache shared by all callers. SASfit evaluates
 * a model function serially over q, so this matches how the plugin is
 * actually driven; if that ever changes, make the cache __thread. */
typedef struct {
    int    valid;
    double sigma, sRel, phi, Z, LB, alpha, chargeExp;
    int    nclass;
    ryp_system sys;
} ryp_cache;

static ryp_cache g_cache = { 0 };

static int cacheMatches(const ryp_cache *c, double sigma, double sRel, double phi,
                        double Z, double LB, double alpha, int nclass, double chargeExp)
{
    return c->valid
        && c->sigma == sigma && c->sRel == sRel && c->phi == phi
        && c->Z == Z && c->LB == LB && c->alpha == alpha
        && c->nclass == nclass && c->chargeExp == chargeExp;
}

static ryp_status cacheRefresh(double sigma, double sRel, double phi,
                               double Z, double LB, double alpha, int nclass, double chargeExp)
{
    ryp_status st;
    if (cacheMatches(&g_cache, sigma, sRel, phi, Z, LB, alpha, nclass, chargeExp))
        return RYP_OK;

    /* Reallocate only when the discretisation itself changes; otherwise keep
     * the existing system so that its gamma survives as a warm-start guess. */
    if (!g_cache.valid || g_cache.nclass != nclass) {
        ryp_free(&g_cache.sys);
        st = ryp_alloc(&g_cache.sys, nclass, RYP_GRID_N, RYP_POINTS_PER_SIGMA);
        if (st != RYP_OK) return st;
    }

    /* Lengths inside the solver are in units of the mean diameter, so sigma
     * itself is 1 there and the Bjerrum length has to be scaled by it. */
    st = ryp_set_schulz(&g_cache.sys, sRel, 1.0);
    if (st != RYP_OK) return st;
    st = ryp_set_charges(&g_cache.sys, Z, chargeExp, LB / sigma, NULL, NULL);
    if (st != RYP_OK) return st;
    st = ryp_set_volume_fraction(&g_cache.sys, phi);
    if (st != RYP_OK) return st;
    st = ryp_build_potential(&g_cache.sys);
    if (st != RYP_OK) return st;
    st = ryp_solve_robust(&g_cache.sys, alpha, RYP_MAX_ITERATIONS, RYP_TOLERANCE);
    if (st != RYP_OK) return st;

    g_cache.sigma = sigma; g_cache.sRel = sRel; g_cache.phi = phi;
    g_cache.Z = Z; g_cache.LB = LB; g_cache.alpha = alpha;
    g_cache.nclass = nclass; g_cache.chargeExp = chargeExp;
    g_cache.valid = 1;
    return RYP_OK;
}

scalar sasfit_ff_RYPolydisperseYukawa(scalar q, sasfit_param * param)
{
	int nclass;
	ryp_status st;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0", q);
	SASFIT_CHECK_COND1((SIGMA <= 0.0), param, "sigma(%lg) <= 0", SIGMA);
	SASFIT_CHECK_COND1((S_REL < 0.0 || S_REL >= 1.0), param, "s_rel(%lg) out of [0,1)", S_REL);
	SASFIT_CHECK_COND1((PHI <= 0.0 || PHI >= 1.0), param, "phi(%lg) out of (0,1)", PHI);
	SASFIT_CHECK_COND1((L_B <= 0.0), param, "L_B(%lg) <= 0", L_B);
	SASFIT_CHECK_COND1((ALPHA <= 0.0), param, "alpha(%lg) <= 0", ALPHA);

	nclass = (int)(NCLASS + 0.5);
	if (nclass < 1) nclass = 1;
	if (nclass > RYP_MAXP) nclass = RYP_MAXP;
	/* A vanishing width is the monodisperse limit; forcing one class keeps
	 * the moment-matching quadrature well defined. */
	if (S_REL <= 0.0) nclass = 1;

	st = cacheRefresh(SIGMA, S_REL, PHI, ZVAL, L_B, ALPHA, nclass, CHARGE_EXP);
	if (st != RYP_OK) {
		sasfit_param_set_err(param, DBGINFO(SASFIT_ERR_PREFIX "%s !\n"), ryp_strerror(st));
		return SASFIT_RETURNVAL_ON_ERROR;   /* == 1.0 for structure factors */
	}

	/* q arrives in the caller's own reciprocal length units; the solver works
	 * in units of the mean diameter, hence the q*sigma.
	 *
	 * THIS RETURNS S^M, THE MEASURABLE STRUCTURE FACTOR, which already
	 * carries the form-factor weighting and is normalised by <F^2>. SASfit's
	 * I = FF(q) * SQ(q) is therefore correct ONLY IF the form factor it
	 * multiplies by uses the same distribution and the same width as the one
	 * baked in here -- and nothing checks that. A mismatch gives a plausible
	 * curve, no warning, and a fit that converges to meaningless parameters.
	 *
	 * That is why this belongs under ff_ rather than sq_: computed as a form
	 * factor the whole I(q) comes from one distribution and the mismatch
	 * becomes unrepresentable. See README.md for what the port involves. */
	return (scalar) ryp_SM(&g_cache.sys, q * SIGMA);
}

scalar sasfit_ff_RYPolydisperseYukawa_f(scalar q, sasfit_param * param)
{
	// no scattering amplitude is defined for a structure factor; stubbed to
	// 0.0, matching the convention of SASfit's own sq_* plugins.
	//
	// WORTH KNOWING IF THIS MOVES TO ff_. An interacting polydisperse system
	// has no single scattering amplitude to offer: the interaction couples
	// the sizes, which is the whole reason for solving the multicomponent OZ
	// equations rather than averaging independent particles. SASfit uses _f
	// for its OWN polydispersity integration, so before porting, establish
	// what it does when a distribution is attached to an ff_ plugin whose _f
	// returns zero -- if it integrates over _f, a stub yields zero intensity
	// rather than an error. See README.md.
	return 0.0;
}

scalar sasfit_ff_RYPolydisperseYukawa_v(scalar q, sasfit_param * param, int dist)
{
	// no particle volume applies to a structure factor either
	return 0.0;
}
