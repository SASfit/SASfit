/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define Q	param->p[1]
#define BETA	fabs(param->p[2])
#define DUMMY	param->p[3]
#define SHIFT	param->p[4]
#define Y0	param->p[5]

scalar sasfit_ff_q_gaussian__area_(scalar x, sasfit_param * param)
{
    // https://en.wikipedia.org/wiki/Q-Gaussian_distribution
    /*
     *   THIS IS THE PROBABILITY DISTRIBUTION FUNCTION
     */
    scalar z;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((Q >= 3.0), param, "q(%lg) >= 3",Q); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA == 0.0), param, "beta(%lg) == 0",BETA); // modify condition to your needs

	// insert your code here
	z = x-SHIFT;
	if (Q<1 && fabs(z)>1./sqrt(BETA*(1.-Q))) return Y0;
    return Y0+A*sqrt(BETA)/Cq(Q)*eq(Q,-BETA*z*z);
}

scalar sasfit_ff_q_gaussian__area__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_q_gaussian__area__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

