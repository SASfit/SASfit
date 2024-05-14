/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define I0	param->p[0]
#define A	param->p[1]
#define RA	param->p[2]

scalar sasfit_ff_extended_guinier_law(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q <= 0.0), param, "q(%lg) <= 0",q);
	SASFIT_CHECK_COND1((A  < 0.0), param, "a(%lg) < 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((A  >= 3.0), param, "a(%lg) >= 3",A); // modify condition to your needs
	SASFIT_CHECK_COND1((RA < 0.0), param, "Ra(%lg) < 0",RA); // modify condition to your needs


	SASFIT_ASSERT_PTR(param);

	return ((A == 0.0) ? I0*exp(-RA*RA*q*q/3.0): A*M_PI*pow(q,-A)*I0*exp(-RA*RA*q*q/(3.0-A)) );
}

scalar sasfit_ff_extended_guinier_law_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_extended_guinier_law_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

