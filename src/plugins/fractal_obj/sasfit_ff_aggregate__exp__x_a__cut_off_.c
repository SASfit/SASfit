/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define XI		param->p[0]
#define D		param->p[1]
#define I0		param->p[2]
#define ALPHA	param->p[3]

#define Q		param->p[MAXPAR-1]

scalar MassFractExp_mpow_x_a_core(scalar x, sasfit_param *param)
{
		SASFIT_ASSERT_PTR(param);

        return D * pow(x,D-3.0) * exp(-pow(x/XI,ALPHA)) * x*x * sin(Q*x)/(Q*x);
}

scalar sasfit_ff_aggregate__exp__x_a__cut_off_(scalar q, sasfit_param * param)
{
    scalar res, norm, C;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((XI <= 0.0), param, "xi <= 0",XI);
	SASFIT_CHECK_COND1((D >= 3.0), param, "D >= 3 ",D);
	SASFIT_CHECK_COND1((D <= 1.0), param, "D <= 1 ",D);

	// insert your code here
	if (q*XI > 100.) {
		Q = 100./XI;
		res = sasfit_integrate(0.0, GSL_POSINF, MassFractExp_mpow_x_a_core, param);
		C = res*pow(Q*XI,D);
		res = C*pow(q*XI,-D);
	} else {
		Q = q;
		res = sasfit_integrate(0.0, GSL_POSINF, MassFractExp_mpow_x_a_core, param);
	}
	norm = pow(XI,D)*exp(gsl_sf_lngamma(D/ALPHA)+log(D/ALPHA));
	return I0*res/norm;
}

scalar sasfit_ff_aggregate__exp__x_a__cut_off__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_aggregate__exp__x_a__cut_off__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

