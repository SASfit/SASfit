/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define Q	param->p[1]
#define LAMBDA	param->p[2]
#define DUMMY	param->p[3]
#define SHIFT	param->p[4]
#define Y0	param->p[5]

scalar sasfit_ff_q_exponential__area_(scalar x, sasfit_param * param)
// https://en.wikipedia.org/wiki/Q-exponential_distribution
{
    /*
     *   THIS IS THE PROBABILITY DISTRIBUTION FUNCTION
     */
	scalar z;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((Q >= 2.0), param, "q(%lg) >= 2",Q); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA <= 0.0), param, "lambda(%lg) <= 0",LAMBDA); // modify condition to your needs

	// insert your code here
	z=x-SHIFT;
	if (z<0) return Y0;
	if (z>=1/(LAMBDA*(1-Q)) && Q<1) return Y0;
	return Y0+A*(2-Q)*LAMBDA*eq(Q,-LAMBDA*z);
}

scalar sasfit_ff_q_exponential__area__f(scalar x, sasfit_param * param)
{
    /*
     *   THIS IS THE CUMMULITATIVE DISTRIBUTION FUNCTION
     */
	scalar z,qp;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	z=x-SHIFT;
	qp=1./(2-Q);

	return A*(1-eq(qp,-LAMBDA*z/qp));
}

scalar sasfit_ff_q_exponential__area__v(scalar p, sasfit_param * param, int dist)
{
	scalar z,qp;
    /*
     *   THIS IS THE QUANTILE DISTRIBUTION FUNCTION
     */
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	qp=1./(2-Q);
	return -lnq(Q,1-p)*qp/LAMBDA+SHIFT;
}

