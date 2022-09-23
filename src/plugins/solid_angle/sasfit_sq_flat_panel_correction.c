/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define LAMBDA	param->p[0]
#define T0SAMPLE	param->p[1]
#define T0FLAT	param->p[2]

scalar sasfit_sq_flat_panel_correction(scalar q, sasfit_param * param)
{
	scalar TwoTheta;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((LAMBDA < 0.0), param, "lambda(%lg) < 0",LAMBDA); // modify condition to your needs
	SASFIT_CHECK_COND1((T0SAMPLE < 0.0), param, "T0sample(%lg) < 0",T0SAMPLE); // modify condition to your needs
	SASFIT_CHECK_COND1((T0FLAT < 0.0), param, "T0flat(%lg) < 0",T0FLAT); // modify condition to your needs

	// insert your code here
	TwoTheta = 2*asin(q*LAMBDA/(4*M_PI));
	return (T0FLAT/pow(T0FLAT,0.5+0.5/cos(TwoTheta)))/(T0SAMPLE/pow(T0SAMPLE,0.5+0.5/cos(TwoTheta)));
}

scalar sasfit_sq_flat_panel_correction_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_flat_panel_correction_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

