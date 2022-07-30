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
#define ALPHA	param->p[3]
#define Q		param->p[MAXPAR-1]

scalar sq_MassFractExp_mpow_x_a_core(scalar x, sasfit_param * param)
{
	scalar h;

	SASFIT_ASSERT_PTR(param);

	h = exp(-pow(x/XI,ALPHA));

	return D/pow(R0,D)*pow(x,D-1.)* h * gsl_sf_bessel_j0(Q*x);
	return D/pow(R0,D)*pow(x,D-3.)* h * x*x*sin(Q*x)/(Q*x);
}

scalar sasfit_sq_MassFractExp_mpow_x_a(scalar q, sasfit_param * param)
{
	scalar res,C;

	SASFIT_ASSERT_PTR(param);


	if (q*XI > 100.) {
		Q = 100./XI;
		res = sasfit_integrate(0.0, GSL_POSINF, sq_MassFractExp_mpow_x_a_core, param);
		C = res*pow(Q*XI,D);
		res = C*pow(q*XI,-D);
	} else {
		Q = q;
		res = sasfit_integrate(0.0, GSL_POSINF, sq_MassFractExp_mpow_x_a_core, param);
	}

	return 1.0+res;
}
scalar sasfit_sq_mass_fractal__exp__x_a__cut_off_(scalar q, sasfit_param * param)
{
    scalar res,C;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R0 <= 0.0), param, "r0(%lg) <= 0",R0); // modify condition to your needs
	SASFIT_CHECK_COND1((XI <= 0.0), param, "xi(%lg) <= 0",XI); // modify condition to your needs
	SASFIT_CHECK_COND1((D <= 1.0), param, "D(%lg) <= 1",D); // modify condition to your needs
	SASFIT_CHECK_COND1((D > 3.0), param, "D(%lg) > 3",D); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA < 0.0), param, "alpha(%lg) < 0",ALPHA); // modify condition to your needs

	// insert your code here

	if (q*XI > 100.) {
		Q = 100./XI;
		res = sasfit_integrate(0.0, GSL_POSINF, sq_MassFractExp_mpow_x_a_core, param);
		C = res*pow(Q*XI,D);
		res = C*pow(q*XI,-D);
	} else {
		Q = q;
		res = sasfit_integrate(0.0, GSL_POSINF, sq_MassFractExp_mpow_x_a_core, param);
	}

	return 1.0+res;
}

scalar sasfit_sq_mass_fractal__exp__x_a__cut_off__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_mass_fractal__exp__x_a__cut_off__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

