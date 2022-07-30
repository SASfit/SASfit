/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define R0	param->p[0]
#define XI	param->p[1]
#define D	param->p[2]
#define Q	param->p[MAXPAR-1]

scalar sq_MassFractOverlapSph_core(scalar x, sasfit_param * param)
{
    scalar h;

	SASFIT_ASSERT_PTR(param);

    h	= (1.+x/(4.*XI))*pow(1.-x/(2.*XI),2.);
    return D/pow(R0,D)*pow(x,D-1.)* h *gsl_sf_bessel_j0(Q*x);
    return D/pow(R0,D)*pow(x,D-3.)* h *x*x*sin(Q*x)/(Q*x);
}


scalar sasfit_sq_mass_fractal__overapsph_cut_off_(scalar q, sasfit_param * param)
{
    scalar res, C;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R0 <= 0.0), param, "r0(%lg) <= 0",R0); // modify condition to your needs
	SASFIT_CHECK_COND1((XI <= 0.0), param, "xi(%lg) <= 0",XI); // modify condition to your needs
	SASFIT_CHECK_COND1((D <= 1.0), param, "D(%lg) <= 1",D); // modify condition to your needs

	// insert your code here

	if (q*XI > 50.) {
		Q = 50./XI;
		res = sasfit_integrate(0.0, 2.0*XI, sq_MassFractOverlapSph_core, param);
		C = res*pow(Q*XI,D);
		res = C*pow(q*XI,-D);
	} else {
		Q = q;
		res = sasfit_integrate(0.0, 2.0*XI, sq_MassFractOverlapSph_core, param);
	}

	return 1.0+res;
}

scalar sasfit_sq_mass_fractal__overapsph_cut_off__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_mass_fractal__overapsph_cut_off__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

