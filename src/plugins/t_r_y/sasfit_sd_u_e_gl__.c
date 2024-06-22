/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar sasfit_sd_u_e_gl__(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((N < 0.0), param, "N(%lg) < 0",N); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA3 <= 0.0), param, "lambda3(%lg) <= 0",LAMBDA3); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA4 <= 0.0), param, "lambda4(%lg) <= 0",LAMBDA4); // modify condition to your needs

	// insert your code here
	return N*0.5*THETA*exp(-THETA*x)*(LAMBDA3*pow(1-exp(-THETA*x),LAMBDA3-1)+LAMBDA4*pow(exp(-THETA*x),LAMBDA4-1));
}

scalar sasfit_sd_u_e_gl___f(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return N*0.5*(1+pow(1-exp(-THETA*x),LAMBDA3)-pow(exp(-THETA*x),LAMBDA4));
}

scalar sasfit_sd_u_e_gl___v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

