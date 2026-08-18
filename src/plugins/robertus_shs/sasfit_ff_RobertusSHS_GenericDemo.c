/*
 * Author(s) of this file:
 *   (see top-level README.md)
 */
#include <math.h>
#include "include/private.h"
#include <sasfit_error_ff.h>

// --- SASfit-style adapters (see robertus_shs_core.h's generic engine
// and the top-level README's "Generic composability layer" section) --
typedef scalar (*sasfit_sd_fn)(scalar x, sasfit_param *param);
typedef scalar (*sasfit_ff_amp_fn)(scalar q, sasfit_param *param);

typedef struct { sasfit_sd_fn sd_func; sasfit_param *param; } sd_adapter_ctx;
static double sd_adapter(double x, void *ctx_)
{
	sd_adapter_ctx *c = (sd_adapter_ctx *)ctx_;
	return (double) c->sd_func((scalar)x, c->param);
}

typedef struct { sasfit_ff_amp_fn ff_func; sasfit_param *ff_param; int size_param_index; } ff_adapter_ctx;
static double ff_adapter(double q, double R, void *ctx_)
{
	ff_adapter_ctx *c = (ff_adapter_ctx *)ctx_;
	double saved = c->ff_param->p[c->size_param_index];
	double val;
	c->ff_param->p[c->size_param_index] = R;
	val = (double) c->ff_func((scalar)q, c->ff_param);
	c->ff_param->p[c->size_param_index] = saved;
	return val;
}

// --- toy stand-ins for "some SASfit size-distribution plugin" and
// "some SASfit form-factor _f plugin", written in SASfit's own
// calling convention -- see test_generic_wiring.c for verification
// that this reproduces the equivalent built-in call exactly.
static scalar demo_sd_lognormal(scalar x, sasfit_param *param)
{
	double mean = param->p[0], sigma = param->p[1];
	double sigma_ln2, mu_ln, u;
	if (x <= 0.0) return 0.0;
	sigma_ln2 = log(1.0 + (sigma/mean)*(sigma/mean));
	mu_ln = log(mean) - 0.5*sigma_ln2;
	u = log(x) - mu_ln;
	return (scalar)(1.0/(x*sqrt(2.0*M_PI*sigma_ln2)) * exp(-0.5*u*u/sigma_ln2));
}

static scalar demo_ff_sphere_amplitude(scalar q, sasfit_param *param)
{
	double R = param->p[0], drho = param->p[1], qR;
	if (q < 1e-7) return (scalar)((4.0/3.0)*M_PI*drho*R*R*R);
	qR = q*R;
	return (scalar)(4.0*M_PI*drho*(sin(qR)-qR*cos(qR))/(q*q*q));
}

#define R          param->p[0]
#define SIGMA_REL  param->p[1]
#define PHI        param->p[2]
#define TAU        param->p[3]
#define NCLASS     param->p[4]
#define DRHO       param->p[5]

static rshs_system g_sys = {0};
static double g_last_R=-1, g_last_srel=-1, g_last_phi=-1, g_last_tau=-1;

scalar sasfit_ff_RobertusSHS_GenericDemo(scalar q, sasfit_param * param)
{
	int nclass = (int)(NCLASS + 0.5);
	double Dmean;
	if (nclass < 3) nclass = 3;
	if (nclass > RSHS_MAXP) nclass = RSHS_MAXP;

	SASFIT_ASSERT_PTR(param);
	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0", q);

	// R (radius, user-facing) -> Dmean (diameter) immediately, same
	// rationale as sasfit_ff_RobertusSHS_CoreShell.c -- see there.
	// demo_sd_lognormal()'s own "x" argument (via sd_adapter below)
	// must be evaluated in the SAME unit as rshs_make_classes_
	// generic()'s own Dmean argument, so sd_param.p[0] is set from
	// Dmean here too, not from the raw R.
	Dmean = 2.0 * R;

	if (R != g_last_R || SIGMA_REL != g_last_srel || PHI != g_last_phi || TAU != g_last_tau) {
		sasfit_param sd_param; sd_param.p[0] = Dmean; sd_param.p[1] = SIGMA_REL * Dmean;
		sd_adapter_ctx sdctx = { demo_sd_lognormal, &sd_param };

		if (rshs_make_classes_generic(&g_sys, nclass, Dmean, SIGMA_REL, PHI,
		                               sd_adapter, &sdctx) != 0) return SASFIT_RETURNVAL_ON_ERROR;
		if (rshs_solve(&g_sys, TAU) != 0) return SASFIT_RETURNVAL_ON_ERROR;
		g_last_R=R; g_last_srel=SIGMA_REL; g_last_phi=PHI; g_last_tau=TAU;
	}

	sasfit_param ff_param; ff_param.p[1] = DRHO;
	ff_adapter_ctx ffctx = { demo_ff_sphere_amplitude, &ff_param, 0 };

	return (scalar) rshs_intensity_generic(&g_sys, q, ff_adapter, &ffctx);
}

scalar sasfit_ff_RobertusSHS_GenericDemo_f(scalar q, sasfit_param * param)
{
	return 0.0; // see sasfit_ff_RobertusSHS_CoreShell_f
}

scalar sasfit_ff_RobertusSHS_GenericDemo_v(scalar q, sasfit_param * param, int dist)
{
	return 0.0; // see sasfit_ff_RobertusSHS_CoreShell_v
}
