/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar sasfit_sd_w_l_e_(scalar x, sasfit_param * param)
{
    scalar tmp;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((x < 0.0), param, "x(%lg) < 0",x);
	SASFIT_CHECK_COND1((THETA < 0.0), param, "theta(%lg) < 0",THETA); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA < 0.0), param, "alpha(%lg) < 0",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA < 0.0), param, "beta(%lg) < 0",BETA); // modify condition to your needs

	// insert your code here
	if (x<0) return 0.0;
	tmp = -BETA*log(1-Lindley_F_R(x,THETA));
	return N*ALPHA*BETA*THETA*THETA*(1+x)/(1+THETA+THETA*x)
	       *pow(tmp,ALPHA-1)*exp(-pow(tmp,ALPHA));
}

scalar sasfit_sd_w_l_e__f(scalar x, sasfit_param * param)
{
    scalar tmp;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

	if (x<0) return 0.0;
	tmp = -BETA*log(1-Lindley_F_R(x,THETA));
	return 1-exp(-pow(tmp,ALPHA));
}

scalar sasfit_sd_w_l_e__v(scalar y, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

	// case 0: quantile distribution function
	// case 1: not yet defined (mode?, median?, ....)
	switch (dist) {
        case 0: if (y<=0) return 0;
                if (y>=1) y = 1-sasfit_eps_get_nriq();
                return -(1+THETA)/THETA-1/THETA*gsl_sf_lambert_Wm1(-(THETA+1)*exp(-((THETA+1)+pow(-log(1-y),1/ALPHA)/BETA)));
        default: sasfit_err("parameter distr=%d not defined",dist);
    }
}

