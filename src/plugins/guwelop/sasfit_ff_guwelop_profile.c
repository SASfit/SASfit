/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

scalar sasfit_ff_guwelop_profile(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((x < 0.0), param, "x(%lg) < 0",x);
	SASFIT_CHECK_COND1((LAMBDA < 0.0), param, "lambda(%lg) < 0",LAMBDA); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA < 0.0), param, "alpha(%lg) < 0",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA < 0.0), param, "beta(%lg) < 0",BETA); // modify condition to your needs
	SASFIT_CHECK_COND1((GAMMA < 0.0), param, "gamma(%lg) < 0",GAMMA); // modify condition to your needs

	// insert your code here
	if (x>BU) return 0;
	return N*(1-sasfit_sd_p_guwelop_f(x, param));
}

scalar sasfit_ff_guwelop_profile_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_guwelop_profile_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

