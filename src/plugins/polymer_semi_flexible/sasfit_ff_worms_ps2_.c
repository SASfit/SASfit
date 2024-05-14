/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define LB	param->p[0]
#define L	param->p[1]

#define I0	param->p[3]

scalar sasfit_ff_worms_ps2_(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((LB <= 0.0), param, "lb(%lg) <= 0",LB); // modify condition to your needs
	SASFIT_CHECK_COND1((L <= 0.0), param, "L(%lg) <= 0",L); // modify condition to your needs

	// insert your code here
	if (q==0) return I0;
	return I0/(L*L)*sasfit_sq_p__q___worm_ps2_(q,param);
}

scalar sasfit_ff_worms_ps2__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_worms_ps2__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

