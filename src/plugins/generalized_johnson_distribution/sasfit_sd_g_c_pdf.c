/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables
#define N	param->p[0]
#define L	param->p[1]
#define U	param->p[2]
#define XI	param->p[3]
#define LAMBDA	param->p[4]
#define DELTA	param->p[5]
#define GAMMA	param->p[6]

scalar sasfit_sd_g_c_pdf(scalar x, sasfit_param * param)
{
    /*
     *   THIS IS THE PROBABILITY DISTRIBUTION FUNCTION
     */
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((x < 0.0), param, "x(%lg) < 0",x);
	SASFIT_CHECK_COND1((N < 0.0), param, "N(%lg) < 0",N); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "l(%lg) < 0",L); // modify condition to your needs
	SASFIT_CHECK_COND1((U < 0.0), param, "u(%lg) < 0",U); // modify condition to your needs
	SASFIT_CHECK_COND1((XI < 0.0), param, "xi(%lg) < 0",XI); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA < 0.0), param, "lambda(%lg) < 0",LAMBDA); // modify condition to your needs
	SASFIT_CHECK_COND1((DELTA < 0.0), param, "delta(%lg) < 0",DELTA); // modify condition to your needs
	SASFIT_CHECK_COND1((GAMMA < 0.0), param, "gamma(%lg) < 0",GAMMA); // modify condition to your needs

	// insert your code here
	return 1/(DELTA*LAMBDA*M_PI*sqrt(1 + gsl_pow_2(x - XI)/gsl_pow_2(LAMBDA))*
     (1 + gsl_pow_2(-(DELTA*GAMMA) + asinh((x - XI)/LAMBDA))/gsl_pow_2(DELTA)));
}

scalar sasfit_sd_g_c_pdf_f(scalar x, sasfit_param * param)
{
    /*
     *   THIS IS THE CUMMULITATIVE DISTRIBUTION FUNCTION
     */
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.5 + atan((-(DELTA*GAMMA) + asinh((x - XI)/LAMBDA))/DELTA)/M_PI;
}

scalar sasfit_sd_g_c_pdf_v(scalar p, sasfit_param * param, int dist)
{
    /*
     *   THIS IS THE QUANTILE DISTRIBUTION FUNCTION
     */
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return XI + LAMBDA*sinh(DELTA*(GAMMA + tan((-0.5 + p)*M_PI)));
}

