/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar sasfit_sd_n_l_c_(scalar x, sasfit_param * param)
{
    scalar tmp;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((x < 0.0), param, "x(%lg) < 0",x);
	SASFIT_CHECK_COND1((THETA < 0.0), param, "theta(%lg) < 0",THETA); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA < 0.0), param, "sigma(%lg) < 0",SIGMA); // modify condition to your needs

	// insert your code here
	tmp=(tan(M_PI*(Lindley_F_R(x,THETA)-0.5))-MU)/SIGMA;
	return N*sqrt(M_PI_2)*THETA*THETA/(SIGMA*(1+THETA))*(1+x)
        *exp(-THETA*x)/gsl_pow_2(sin(M_PI*(1-Lindley_F_R(x,THETA))))
        *exp(-gsl_pow_2(tmp/2));
}

scalar sasfit_sd_n_l_c__f(scalar x, sasfit_param * param)
{
    scalar tmp;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	tmp=(tan(M_PI*(Lindley_F_R(x,THETA)-0.5))-MU)/SIGMA;
	return 0.5*(1+gsl_sf_erf(tmp/sqrt(2)));
}

scalar sasfit_sd_n_l_c__v(scalar y, sasfit_param * param, int dist)
{
    scalar QN;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

	// case 0: quantile distribution function
	// case 1: not yet defined (mode?, median?, ....)

	switch (dist) {
        case 0: if (y<=0) return 0;
                if (y>=1) y = 1-sasfit_eps_get_nriq();
                QN = MU+SIGMA*sqrt(2)*sasfit_erfinv(2*y-1);
                return KWm1(0.5-atan( QN )/M_PI,THETA);
        default: sasfit_err("parameter distr=%d not defined",dist);
    }
}

