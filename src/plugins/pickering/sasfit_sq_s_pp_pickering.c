/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include <gsl/gsl_randist.h>
#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define R_O	param->p[0]
#define R_P	param->p[1]
#define DELTA	param->p[2]
#define N	param->p[3]


scalar sasfit_sq_s_pp_pickering(scalar q, sasfit_param * param)
{
    scalar result,u;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R_O < 0.0), param, "R_o(%lg) < 0",R_O); // modify condition to your needs
	SASFIT_CHECK_COND1((R_P < 0.0), param, "R_p(%lg) < 0",R_P); // modify condition to your needs
	SASFIT_CHECK_COND1((DELTA < 0.0), param, "delta(%lg) < 0",DELTA); // modify condition to your needs
	SASFIT_CHECK_COND1((N < 1.0), param, "n(%lg) < 1",N); // modify condition to your needs

	// insert your code here
	u = q*(R_O+DELTA*R_P);
	result = gsl_pow_2(gsl_sf_bessel_j0(u));
    return result*(lround(N)-1.0) + 1.0;
}

scalar sasfit_sq_s_pp_pickering_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_s_pp_pickering_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

