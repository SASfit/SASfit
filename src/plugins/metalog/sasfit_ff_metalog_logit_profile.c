/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar sasfit_ff_metalog_logit_profile(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	if (x<=0) return N;
	if (x>=BU) return 0;

    param->p[1]=0;

 	return N*(1-sasfit_sd_metalog_logit_v(x,param,DISTRIBUTION_CUMULATIVE));
}

scalar sasfit_ff_metalog_logit_profile_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);

	return 0;
}

scalar sasfit_ff_metalog_logit_profile_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

