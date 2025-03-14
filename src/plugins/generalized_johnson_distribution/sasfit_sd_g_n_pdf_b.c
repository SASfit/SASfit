/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
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

scalar sasfit_sd_g_n_pdf_b(scalar x, sasfit_param * param)
{
    /*
     *   THIS IS THE PROBABILITY DISTRIBUTION FUNCTION
     */
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((x < 0.0), param, "x(%lg) < 0",x);
	SASFIT_CHECK_COND1((L < 0.0), param, "l(%lg) < 0",L); // modify condition to your needs
	SASFIT_CHECK_COND1((U < 0.0), param, "u(%lg) < 0",U); // modify condition to your needs

	// insert your code here

	if (x<=L || x>=U) return 0;
	return -N*(exp(-gsl_pow_2(DELTA*GAMMA + asinh((XI - sqrt(2)*sasfit_erfinv((L + U - 2*x)/(L - U)))/
             LAMBDA))/(2.*gsl_pow_2(DELTA)) + gsl_pow_2(sasfit_erfinv((L + U - 2*x)/(L - U))))/
     (DELTA*LAMBDA*(L - U)*sqrt(1 + gsl_pow_2(XI - sqrt(2)*sasfit_erfinv((L + U - 2*x)/(L - U)))/
          gsl_pow_2(LAMBDA))));
}

scalar sasfit_sd_g_n_pdf_b_f(scalar x, sasfit_param * param)
{
    /*
     *   THIS IS THE CUMMULITATIVE DISTRIBUTION FUNCTION
     */
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sd_g_n_pdf_b_v(scalar q, sasfit_param * param, int dist)
{
    /*
     *   THIS IS THE QUANTILE DISTRIBUTION FUNCTION
     */
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

