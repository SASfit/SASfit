/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R1	param->p[0]
#define R2	param->p[1]
#define ETA_0	param->p[2]
#define ETA_1	param->p[3]
#define ETA_2	param->p[4]

scalar sasfit_ff_two_attached_spheres(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R1 < 0.0), param, "R1(%lg) < 0",R1); // modify condition to your needs
	SASFIT_CHECK_COND1((R2 < 0.0), param, "R2(%lg) < 0",R2); // modify condition to your needs

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_two_attached_spheres_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_two_attached_spheres_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

