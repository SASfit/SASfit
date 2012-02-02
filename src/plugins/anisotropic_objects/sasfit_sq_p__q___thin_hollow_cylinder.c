/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define H	param->p[1]
#define SIGMA_R	param->p[2]
#define SIGMA_H	param->p[3]

scalar sasfit_sq_p__q___thin_hollow_cylinder(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((H < 0.0), param, "H(%lg) < 0",H); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA_R < 0.0), param, "sigma_R(%lg) < 0",SIGMA_R); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA_H < 0.0), param, "sigma_H(%lg) < 0",SIGMA_H); // modify condition to your needs

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_p__q___thin_hollow_cylinder_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_p__q___thin_hollow_cylinder_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

