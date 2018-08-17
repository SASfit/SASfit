/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define I0	param->p[0]
#define RG	param->p[1]

#define F	param->p[4]

scalar sasfit_ff_polydispersestar(scalar q, sasfit_param * param)
{
    scalar u2;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((F < 0.0), param, "f(%lg) < 0",F); // modify condition to your needs
	// insert your code here

	u2 = q*q*RG*RG;
	if (u2  == 0.0) return I0;
	return I0*(1.+u2/(3.*F))/pow(1.+u2*(1.+F)/(6.*F),2.);
}

scalar sasfit_ff_polydispersestar_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_polydispersestar_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

