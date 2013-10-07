/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define T	param->p[0]
#define SIGMA_T	param->p[1]
#define D	param->p[2]
#define SIGMA_D	param->p[3]
#define DUMMY	param->p[4]
#define DUMMY	param->p[5]
#define SIGMA_OUT	param->p[6]
#define B_OUT	param->p[7]
#define SIGMA_CORE	param->p[8]
#define B_CORE	param->p[9]

scalar sasfit_ff_disc_sd_bilayergauss(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA_T < 0.0), param, "sigma_t(%lg) < 0",SIGMA_T); // modify condition to your needs
	SASFIT_CHECK_COND1((D < 0.0), param, "D(%lg) < 0",D); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA_D < 0.0), param, "sigma_D(%lg) < 0",SIGMA_D); // modify condition to your needs
	SASFIT_CHECK_COND1((DUMMY < 0.0), param, "dummy(%lg) < 0",DUMMY); // modify condition to your needs
	SASFIT_CHECK_COND1((DUMMY < 0.0), param, "dummy(%lg) < 0",DUMMY); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA_OUT < 0.0), param, "sigma_out(%lg) < 0",SIGMA_OUT); // modify condition to your needs
	SASFIT_CHECK_COND1((B_OUT < 0.0), param, "b_out(%lg) < 0",B_OUT); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA_CORE < 0.0), param, "sigma_core(%lg) < 0",SIGMA_CORE); // modify condition to your needs
	SASFIT_CHECK_COND1((B_CORE < 0.0), param, "b_core(%lg) < 0",B_CORE); // modify condition to your needs

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_disc_sd_bilayergauss_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_disc_sd_bilayergauss_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

