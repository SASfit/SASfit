/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define LAMBDA	param->p[1]
#define ALPHA	param->p[2]
#define BETA	param->p[3]
#define DELTA	param->p[4]
#define MU	param->p[5]
#define Y0	param->p[6]

scalar sasfit_ff_generalized_hyperbolic__area_(scalar x, sasfit_param * param)
// https://en.wikipedia.org/wiki/Generalised_hyperbolic_distribution
{
    scalar gamma;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND2(gsl_pow_2(ALPHA) < gsl_pow_2(BETA), param, "alpha^2(%lg) < beta^2(%lf)",ALPHA*ALPHA,BETA*BETA);
	SASFIT_CHECK_COND1(DELTA<=0,param,"delta(%lf)<=0",DELTA); // modify condition to your needs
	SASFIT_CHECK_COND1(ALPHA<=0,param,"alpha(%lf)<=0",ALPHA); // modify condition to your needs
    gsl_set_error_handler_off();
	// insert your code here
	gamma = sqrt(gsl_pow_2(ALPHA)-gsl_pow_2(BETA));
	return Y0+A*pow(gamma/DELTA,LAMBDA)*exp(BETA*(x-MU))/(sqrt(2*M_PI)*gsl_sf_bessel_Knu(fabs(LAMBDA),DELTA*gamma))
            *gsl_sf_bessel_Knu(fabs(LAMBDA-0.5),ALPHA*gsl_hypot(DELTA,x-MU))/pow(gsl_hypot(DELTA,x-MU)/ALPHA,0.5-LAMBDA);
}

scalar sasfit_ff_generalized_hyperbolic__area__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_generalized_hyperbolic__area__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

