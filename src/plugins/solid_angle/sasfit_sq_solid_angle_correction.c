/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define LAMBDA	param->p[0]
#define T0	param->p[1]

scalar sasfit_sq_solid_angle_correction(scalar q, sasfit_param * param)
{
	scalar TwoTheta;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((LAMBDA < 0.0), param, "lambda(%lg) < 0",LAMBDA); // modify condition to your needs
	SASFIT_CHECK_COND1((T0 <= 0.0), param, "T0(%lg) <= 0",T0); // modify condition to your needs

	// insert your code here
	TwoTheta = 2*asin(q*LAMBDA/(4*M_PI));
	return gsl_pow_3(cos(TwoTheta))* ((T0*log(T0)*(-1 + 1./cos(TwoTheta)))/(-T0 + pow(T0,1./cos(TwoTheta))));
}

scalar sasfit_sq_solid_angle_correction_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_solid_angle_correction_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

