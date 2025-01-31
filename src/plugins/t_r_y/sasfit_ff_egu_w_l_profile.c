/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar sasfit_ff_egu_w_l_profile(scalar x, sasfit_param * param)
{
    scalar y;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((DIM > 6.0), param, "dim(%lg) > 6",DIM); // modify condition to your needs
	SASFIT_CHECK_COND1((DIM < 0.0), param, "dim(%lg) < 0",DIM); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA <= 0.0), param, "alpha(%lg) <= 0",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA <= 0.0), param, "beta(%lg) <= 0",BETA); // modify condition to your needs
	SASFIT_CHECK_COND1((GAMMA <= 0.0), param, "c(%lg) <= 0",GAMMA); // modify condition to your needs
	param->p[1]=0;
    SASFIT_CHECK_COND1((BU   <= 0.0), param, "bu(%lg) <= 0",BU);

	// insert your code here

	if (x>=BU) return 0;
	if (x<=BL) return N;
	return N*(1-sasfit_sd_egu_w_l_v(x,param,DISTRIBUTION_CUMULATIVE));
}

scalar sasfit_ff_egu_w_l_profile_f(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
	// insert your code here
	return 0;
}

scalar sasfit_ff_egu_w_l_profile_v(scalar u, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
    return 0.0;
}

