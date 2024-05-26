/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables
#define N	param->p[0]
#define LAMBDA	fabs(param->p[1])
#define K	param->p[2]
#define ALPHA	fabs(param->p[3])
#define BETA	fabs(param->p[4])
#define GAMMA	fabs(param->p[5])

scalar sasfit_ff_egu_w_l_profile(scalar x, sasfit_param * param)
{
    scalar pegu_w_l,z,t;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((x < 0.0), param, "x(%lg) < 0",x);
	SASFIT_CHECK_COND1((LAMBDA <= 0.0), param, "lambda(%lg) <= 0",LAMBDA); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA <= 0.0), param, "alpha(%lg) <= 0",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA <= 0.0), param, "beta(%lg) <= 0",BETA); // modify condition to your needs
	SASFIT_CHECK_COND1((GAMMA <= 0.0), param, "gamma(%lg) <= 0",GAMMA); // modify condition to your needs

	// insert your code here
	return N*(1-sasfit_sd_egu_w_l_f(x, param));
}

scalar sasfit_ff_egu_w_l_profile_f(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
}

scalar sasfit_ff_egu_w_l_profile_v(scalar y, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

