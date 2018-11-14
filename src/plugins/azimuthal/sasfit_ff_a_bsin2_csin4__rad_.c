/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define B	param->p[1]
#define C	param->p[2]
#define DELTA2	param->p[3]
#define DELTA4	param->p[4]

scalar sasfit_ff_a_bsin2_csin4__rad_(scalar psi, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return A+B*gsl_pow_2(sin(psi-DELTA2))+C*gsl_pow_4(sin(psi-DELTA4));;
}

scalar sasfit_ff_a_bsin2_csin4__rad__f(scalar psi, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_a_bsin2_csin4__rad__v(scalar psi, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

