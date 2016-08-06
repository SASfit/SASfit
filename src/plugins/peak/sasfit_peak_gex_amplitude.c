/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   8th of May 2016
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define AMPLITUDE	param->p[0]
#define XMODE	param->p[1]
#define X0	    param->p[2]
#define BETA	param->p[3]
#define LAMBDA	param->p[4]
#define BACKGR	param->p[5]

scalar sasfit_peak_gex_amplitude(scalar x, sasfit_param * param)
{	scalar c,camp,gamma;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
	SASFIT_CHECK_COND1((X0 >= XMODE), param, "X0>=XMODE, X0-XMODE",X0-XMODE); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA == 0.0), param, "beta(%lg) == 0",BETA); // modify condition to your needs
	SASFIT_CHECK_COND1(((LAMBDA+2)/BETA <= 0.0), param, "(lambda+2)/beta (%lg) < 0 ",(LAMBDA+2)/BETA); // modify condition to your needs
 
	// insert your code here
	if (x>=X0) {
        gamma = (XMODE - X0)*pow((LAMBDA+1.0)/BETA,-1.0/BETA);
        SASFIT_CHECK_COND1((gamma == 0.0), param, "gamma(%lg) == 0",gamma); // modify condition to your needs
        c=log(fabs(BETA)/gamma)-sasfit_gammaln((LAMBDA+2.0)/BETA)+(LAMBDA+1.0)*log((x-X0)/gamma)-pow((x-X0)/gamma,BETA);
        camp=log(fabs(BETA)/gamma)-sasfit_gammaln((LAMBDA+2.0)/BETA)+(LAMBDA+1.0)*log((XMODE-X0)/gamma)-pow((XMODE-X0)/gamma,BETA);
		return AMPLITUDE*exp(c-camp)+BACKGR;
	} else {
		return BACKGR;
	}
}

scalar sasfit_peak_gex_amplitude_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_peak_gex_amplitude_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

