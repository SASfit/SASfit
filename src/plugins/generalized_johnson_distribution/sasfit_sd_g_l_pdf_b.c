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

scalar sasfit_sd_g_l_pdf_b(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((x < 0.0), param, "x(%lg) < 0",x);
	SASFIT_CHECK_COND1((L < 0.0), param, "l(%lg) < 0",L); // modify condition to your needs
	SASFIT_CHECK_COND1((U < 0.0), param, "u(%lg) < 0",U); // modify condition to your needs

	// insert your code here

	if (x<=L || x>=U) return 0;
	return N*(L - U)/(2.*DELTA*LAMBDA*(L - x)*(U - x)*
     (1 + cosh(GAMMA + asinh((XI - log((L - x)/(-U + x)))/LAMBDA)/DELTA))*
     sqrt((gsl_pow_2(LAMBDA) + gsl_pow_2(XI) - 2*XI*log((L - x)/(-U + x)) +
         gsl_pow_2(log((L - x)/(-U + x))))/gsl_pow_2(LAMBDA)));
}

scalar sasfit_sd_g_l_pdf_b_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sd_g_l_pdf_b_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

