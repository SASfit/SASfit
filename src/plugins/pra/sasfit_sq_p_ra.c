/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define B	param->p[0]
#define F	param->p[1]
#define NW	param->p[2]

scalar sasfit_sq_p_ra(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((B < 0.0), param, "b(%lg) < 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((F < 0.0), param, "f(%lg) < 0",F); // modify condition to your needs
	SASFIT_CHECK_COND1((NW < 0.0), param, "nw(%lg) < 0",NW); // modify condition to your needs

	// insert your code here
	return (F*NW-(NW-1)*(1-gsl_sf_bessel_j0(q*B)))/(F*NW+(F-1)*NW*(NW-1)*(1-gsl_sf_bessel_j0(q*B)));
}

scalar sasfit_sq_p_ra_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_p_ra_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

