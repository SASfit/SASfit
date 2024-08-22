/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define P0	param->p[0]
#define P1	param->p[1]
#define P2	param->p[2]
#define P3	param->p[3]
#define P4	param->p[4]

scalar sasfit_ff_polynom(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return P0 + P1*q + P2*gsl_pow_int(q,2) + P3*gsl_pow_int(q,3) + P4*gsl_pow_int(q,4);
}

scalar sasfit_ff_polynom_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_polynom_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

