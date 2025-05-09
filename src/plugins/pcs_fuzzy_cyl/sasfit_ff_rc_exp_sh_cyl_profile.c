/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RC	        param->p[0]
#define T	        param->p[1]
#define ALPHA	    param->p[2]
#define SIGMA_S     param->p[3]
#define ETAC	    param->p[4]
#define ETASHIN	    param->p[5]
#define ETASHOUT	param->p[6]
#define ETASOLV 	param->p[7]

scalar sasfit_ff_rc_exp_sh_cyl_profile(scalar r, sasfit_param * param)
{
    scalar x;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((RC < 0.0), param, "RC(%lg) < 0",RC); // modify condition to your needs
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs

	// insert your code here

	scalar etaexp;
	if (fabs(r)>RC+T) return ETASOLV;

    if (fabs(r)>RC+T) {
        x = 1;
    } else {
        x = (r-RC)/T;
    }

	if (r<RC) return ETAC;

//    x = (r-RC)/T;
	if (ALPHA < 0)
	{
		return (ETASHIN +(ETASHOUT-ETASHIN )*x*exp((1-x)*ALPHA))/(1+exp((r-RC-T)/SIGMA_S));
	} else {
		return (ETASHOUT+(ETASHIN -ETASHOUT)*(1-x)*exp(-x*ALPHA))/(1+exp((r-RC-T)/SIGMA_S));
	}
}


scalar sasfit_ff_rc_exp_sh_cyl_profile_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0;
}

scalar sasfit_ff_rc_exp_sh_cyl_profile_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0;
}

