/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar sasfit_ff_n_l_c_profile(scalar x, sasfit_param * param)
{
    scalar tmp;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((x < 0.0), param, "x(%lg) < 0",x);
	SASFIT_CHECK_COND1((THETA < 0.0), param, "theta(%lg) < 0",THETA); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA < 0.0), param, "sigma(%lg) < 0",SIGMA); // modify condition to your needs

	// insert your code here

    if (x>BU) {
        return 0;
	}
	if (x<=BL) {
	    YSTART = sasfit_invert_func_v(BL,&sasfit_sd_n_l_c__v,DISTRIBUTION_QUANTILE,0,1,param);
        return N*(1-YSTART);
	} else {
        return N-sasfit_sd_n_l_c__v(x,param,DISTRIBUTION_CUMULATIVE);
	}
}

scalar sasfit_ff_n_l_c_profile_f(scalar x, sasfit_param * param)
{
    scalar tmp;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
    return 0;
}

scalar sasfit_ff_n_l_c_profile_v(scalar u, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

    return 0;
}

