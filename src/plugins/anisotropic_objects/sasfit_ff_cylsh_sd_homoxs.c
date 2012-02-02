/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define T	param->p[0]
#define SIGMA_T	param->p[1]
#define R	param->p[2]
#define SIGMA_R	param->p[3]
#define H	param->p[4]
#define SIGMA_H	param->p[5]
#define ETA_L	param->p[6]
#define ETA_SOL	param->p[7]

scalar sasfit_ff_cylsh_sd_homoxs(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA_T < 0.0), param, "sigma_t(%lg) < 0",SIGMA_T); // modify condition to your needs
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA_R < 0.0), param, "sigma_R(%lg) < 0",SIGMA_R); // modify condition to your needs
	SASFIT_CHECK_COND1((H < 0.0), param, "H(%lg) < 0",H); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA_H < 0.0), param, "sigma_H(%lg) < 0",SIGMA_H); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA_L < 0.0), param, "eta_l(%lg) < 0",ETA_L); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA_SOL < 0.0), param, "eta_sol(%lg) < 0",ETA_SOL); // modify condition to your needs

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_cylsh_sd_homoxs_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_cylsh_sd_homoxs_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

