/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]
#define D	param->p[1]
#define I0	param->p[2]

scalar sasfit_ff_massfractexp(scalar q, sasfit_param * param)
{
	scalar xi;

	SASFIT_ASSERT_PTR(param);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((D <= 1.0), param, "D(%lg) <= 1",D);
	SASFIT_CHECK_COND1((RG < 0.0), param, "RG(%lg) < 0",RG);

	xi = sqrt(2.0*RG*RG/(D*(D+1.0)));

    if (q*xi == 0) return I0;
	return I0*(sin((D-1.0)*atan(q*xi))) / ((D-1.)*q*xi*pow(1.0+gsl_pow_2(q*xi),(D-1.0)/2.0));

}

scalar sasfit_ff_massfractexp_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_massfractexp_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

