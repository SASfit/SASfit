/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define XI	param->p[0]
#define H	param->p[1]
#define ETA	param->p[2]

scalar sasfit_ff_dab_2d(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((XI < 0.0), param, "xi(%lg) < 0",XI); // modify condition to your needs
	SASFIT_CHECK_COND1((H < -1.0), param, "H(%lg) <= -1",H); // modify condition to your needs

	// insert your code here
	//return gsl_pow_2(XI*XI*2*M_PI*ETA)/pow(1+gsl_pow_2(q*XI),1.5);
	return gsl_pow_2(ETA)/pow(1+gsl_pow_2(q*XI),1+H);
}

scalar sasfit_ff_dab_2d_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_dab_2d_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

