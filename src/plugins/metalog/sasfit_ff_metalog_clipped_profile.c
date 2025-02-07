/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar sasfit_ff_metalog_clipped_profile(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	if (x>=BU || N==0) return 0;
    return N*(1-sasfit_sd_metalog_clipped_v(x,param,DISTRIBUTION_CUMULATIVE)/N);
}

scalar sasfit_ff_metalog_clipped_profile_f(scalar q, sasfit_param * param)
{
	return 0;
}

scalar sasfit_ff_metalog_clipped_profile_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

