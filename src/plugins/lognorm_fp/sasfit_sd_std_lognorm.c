/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables
#define N	param->p[0]
#define S	param->p[1]
#define P	param->p[2]
#define MU	param->p[3]

scalar sasfit_sd_std_lognorm(scalar x, sasfit_param * param)
{
	scalar c,LNf;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((MU <= 0.0), param, "mu(%lg) <= 0",MU); // modify condition to your needs

	// insert your code here

	if (x <= 0.0) return 0.0;

	c = 1.0/sqrt(M_PI*2.0)/S/pow(MU,1.0-P) / exp(0.5*pow(S*(1.0-P),2.0));
	LNf = N*c* pow(x,-P) * exp(-0.5*pow(log(x/MU)/S,2.0));

	return LNf;
}

scalar sasfit_sd_std_lognorm_f(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	if (x<=0) return 0;
	return N*0.5*gsl_sf_erfc(((1-P)*S*S-log(x/MU))/(sqrt(2)*S));
}

scalar sasfit_sd_std_lognorm_v(scalar y, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return MU*exp(S*S*(1-P)-sqrt(2)*S*sasfit_erfinv(1-2*y))/N;
}

