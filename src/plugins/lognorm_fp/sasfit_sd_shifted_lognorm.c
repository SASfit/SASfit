/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables
#define N	param->p[0]
#define S	param->p[1]
#define X0	param->p[2]
#define MU	param->p[3]

scalar sasfit_sd_shifted_lognorm(scalar x, sasfit_param * param)
{
    scalar c,LNf;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND2((MU < 0.0), param, "mu(%lg) < x0",MU,X0); // modify condition to your needs

	// insert your code here
	if (x <= 0.0) return 0.0;

	c = 1.0/sqrt(M_PI*2.0)/S;
	LNf = N*c/(x-X0) * exp(-0.5*pow(log((x-X0)/MU)/S,2.0));

	return LNf;
}

scalar sasfit_sd_shifted_lognorm_f(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	if (x<=X0) return 0;
	return N*0.5*(1+gsl_sf_erf(log((x-X0)/MU)/(sqrt(2)*S)));
}

scalar sasfit_sd_shifted_lognorm_v(scalar y, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	 return (X0+ MU*exp(sqrt(2)*S*sasfit_erfinv(2*y-1)))/N;
}

