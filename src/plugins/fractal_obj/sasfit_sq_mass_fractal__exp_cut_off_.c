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

scalar sasfit_sq_mass_fractal__exp_cut_off_(scalar q, sasfit_param * param)
{
    scalar P16;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R0 <= 0.0), param, "r0(%lg) <= 0",R0); // modify condition to your needs
	SASFIT_CHECK_COND2((XI < R0), param, "xi(%lg) < R0",XI,R0); // modify condition to your needs
	SASFIT_CHECK_COND1((D <= 1.0), param, "D(%lg) <= 1",D); // modify condition to your needs

	// insert your code here

	if ((XI == 0) || (R0 == 0)) return 1.0;

	P16 = 1.0/pow(q*R0,D);
	P16 = P16*D*gsl_sf_gamma(D-1.0)/pow(1.0+1.0/(q*q*XI*XI),(D-1.0)/2.0);
	P16 = P16*sin((D-1.0)*atan(q*XI));
	P16 = (1.0+P16);
	return P16;
}

scalar sasfit_sq_mass_fractal__exp_cut_off__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_mass_fractal__exp_cut_off__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

