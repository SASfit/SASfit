/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define I0	param->p[0]
#define RG	param->p[1]

#define F	param->p[4]


scalar sasfit_ff_benoitstar(scalar q, sasfit_param * param)
{
    scalar u,v;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((F < (2.0/3.0)), param, "f(%lg) < 2/3",F);
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG);
	// insert your code here

	u = q*q*RG*RG;
	if (u==0) return I0;
	v = u*F/(3.0*F-2.0);
	return I0*(2.0/(F*v*v)*(-1.0+v+exp(-v)+(F-1.0)*pow(1.0-exp(-v),2.0)/2.0));
}

scalar sasfit_ff_benoitstar_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_benoitstar_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

