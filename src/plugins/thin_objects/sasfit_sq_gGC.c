/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

#define RG	param->p[0]
#define NU	param->p[1]
#define I0	param->p[2]

scalar sasfit_sq_gGC(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((L <= 0.0), param, "L(%lg) < 0",L); // modify condition to your needs
	SASFIT_CHECK_COND1((LB <= 0.0), param, "lb(%lg) < 0",LB); // modify condition to your needs

	// insert your code here

	I0 = 1;
	return sasfit_ff_generalized_gaussian_coil(q,param);
}

scalar sasfit_sq_gGC_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0;
}

scalar sasfit_sq_gGC_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

