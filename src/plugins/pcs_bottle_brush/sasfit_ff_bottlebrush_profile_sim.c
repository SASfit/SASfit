/*
 * Author(s) of this file:
 *   <Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define SIGMA_ETA	param->p[0]
#define R1	param->p[1]
#define X1	param->p[2]
#define R2	param->p[3]
#define X2	param->p[4]
#define Q       param->p[MAXPAR-1]

scalar sasfit_ff_bottlebrush_profile_sim(scalar r, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((SIGMA_ETA < 0.0), param, "sigma_eta(%lg) < 0",SIGMA_ETA); // modify condition to your needs
	SASFIT_CHECK_COND1((R1 < 0.0), param, "r1(%lg) < 0",R1); // modify condition to your needs
	SASFIT_CHECK_COND1((X1 < 0.0), param, "x1(%lg) < 0",X1); // modify condition to your needs
	SASFIT_CHECK_COND1((R2 < 0.0), param, "r2(%lg) < 0",R2); // modify condition to your needs
	SASFIT_CHECK_COND1((X2 < 0.0), param, "x2(%lg) < 0",X2); // modify condition to your needs

	// insert your code here
	return SIGMA_ETA/(1+pow(fabs(r)/R1,X1))*exp(-pow(fabs(r)/R2,X2));
}

scalar sasfit_ff_bottlebrush_profile_sim_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0;
}

scalar sasfit_ff_bottlebrush_profile_sim_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

