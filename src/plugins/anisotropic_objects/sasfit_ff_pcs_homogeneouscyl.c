/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define SIGMA_T	param->p[1]

scalar sasfit_ff_pcs_homogeneouscyl(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "r(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA_T < 0.0), param, "sigma_t(%lg) < 0",SIGMA_T); // modify condition to your needs

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_pcs_homogeneouscyl_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_pcs_homogeneouscyl_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

