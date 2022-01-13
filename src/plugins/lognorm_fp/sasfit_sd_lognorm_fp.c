/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables
#define FP	param->p[0]
#define SIGMA	param->p[1]
#define P	param->p[2]
#define MU	param->p[3]
#define LENGTH2	param->p[4]
#define LENGTH3	param->p[5]
#define SHAPE	param->p[6]

scalar sasfit_sd_lognorm_fp(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((FP < 0.0), param, "fp(%lg) < 0",FP); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA < 0.0), param, "sigma(%lg) < 0",SIGMA); // modify condition to your needs
	SASFIT_CHECK_COND1((P < 0.0), param, "p(%lg) < 0",P); // modify condition to your needs
	SASFIT_CHECK_COND1((MU < 0.0), param, "mu(%lg) < 0",MU); // modify condition to your needs
	SASFIT_CHECK_COND1((LENGTH2 < 0.0), param, "length2(%lg) < 0",LENGTH2); // modify condition to your needs
	SASFIT_CHECK_COND1((LENGTH3 < 0.0), param, "length3(%lg) < 0",LENGTH3); // modify condition to your needs
	SASFIT_CHECK_COND1((SHAPE < 0.0), param, "shape(%lg) < 0",SHAPE); // modify condition to your needs

	// insert your code here
	return 0.0;
}

scalar sasfit_sd_lognorm_fp_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sd_lognorm_fp_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

