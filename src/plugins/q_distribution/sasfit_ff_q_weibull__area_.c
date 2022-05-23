/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define Q	param->p[1]
#define LAMBDA	fabs(param->p[2])
#define KAPPA	fabs(param->p[3])
#define SHIFT	param->p[4]
#define Y0	param->p[5]

scalar sasfit_ff_q_weibull__area_(scalar x, sasfit_param * param)
{
    // https://en.wikipedia.org/wiki/Q-Weibull_distribution
    /*
     *   THIS IS THE PROBABILITY DISTRIBUTION FUNCTION
     */
    scalar z;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((Q >= 2.0), param, "q(%lg) >= 2",Q); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA <= 0.0), param, "lambda(%lg) <= 0",LAMBDA); // modify condition to your needs
	SASFIT_CHECK_COND1((KAPPA <= 0.0), param, "kappa(%lg) <= 0",KAPPA); // modify condition to your needs

	// insert your code here
	z = x-SHIFT;
	if (Q>= 1) {
        if (z<=0) {
            return Y0;
        } else {
            return Y0+A*(2-Q)*KAPPA/LAMBDA*pow(z/LAMBDA,KAPPA-1)*eq(Q,-pow(z/LAMBDA,KAPPA));
        }
	} else {
	    if (z<=0 || z>=LAMBDA/pow(1-Q,1./KAPPA)) {
            return Y0;
        } else {
            return Y0+A*(2-Q)*KAPPA/LAMBDA*pow(z/LAMBDA,KAPPA-1)*eq(Q,-pow(z/LAMBDA,KAPPA));
        }
	}
}

scalar sasfit_ff_q_weibull__area__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_q_weibull__area__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

