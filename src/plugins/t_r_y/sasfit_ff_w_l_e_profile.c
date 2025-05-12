/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar sasfit_ff_w_l_e_profile(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((x < 0.0), param, "x(%lg) < 0",x);
	SASFIT_CHECK_COND1((THETA < 0.0), param, "theta(%lg) < 0",THETA); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA < 0.0), param, "alpha(%lg) < 0",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA < 0.0), param, "beta(%lg) < 0",BETA); // modify condition to your needs

	// insert your code here

    if (x>BU) {
        return 0;
	}
	if (x<=BL) {
	    YSTART = sasfit_invert_func_v(BL,&sasfit_sd_w_l_e__v,DISTRIBUTION_QUANTILE,0,1,param);
        return N*(1-YSTART);
	} else {
        return N-sasfit_sd_w_l_e__v(x,param,DISTRIBUTION_CUMULATIVE);
	}
}

scalar sasfit_ff_w_l_e_profile_f(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return sasfit_sd_w_l_e__v(x,param,DISTRIBUTION_CUMULATIVE);;
}

scalar sasfit_ff_w_l_e_profile_v(scalar u, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

    return 0.0;
}

