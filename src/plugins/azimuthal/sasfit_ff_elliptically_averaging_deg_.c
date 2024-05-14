/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define B	param->p[1]
#define C	param->p[2]
#define N	param->p[3]
#define DELTA	(param->p[4]*M_PI/180.)


scalar sasfit_ff_elliptically_averaged_deg_(scalar psi, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param);
	// assert pointer param is valid
	SASFIT_CHECK_COND1((A == 0.0), param, "A(%lg) == 0",A);
	SASFIT_CHECK_COND1((B == 0.0), param, "B(%lg) == 0",B);
	SASFIT_CHECK_COND1((N == 0.0), param, "N(%lg) == 0",N);
	// insert your code here
	return pow(gsl_hypot(cos(psi*M_PI/180.-DELTA)/A,sin(psi*M_PI/180.-DELTA)/B),-N)+C;
}

scalar sasfit_ff_elliptically_averaged_deg__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_elliptically_averaged_deg__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

