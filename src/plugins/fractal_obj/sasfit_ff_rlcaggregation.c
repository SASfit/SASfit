/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]
#define D	param->p[1]
#define I0	param->p[2]

scalar sasfit_ff_rlcaggregation(scalar q, sasfit_param * param)
{
	scalar C[5], SQ;
	int i;

	SASFIT_ASSERT_PTR(param);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG < 0.0), param, "RG < 0",RG);
	SASFIT_CHECK_COND1((D <= 0.0), param, "D(%lg) <= 0",D);

	C[0] = 1.0;
	C[1] = 8./3./D;
	C[2] = 3.13;
	C[3] = -2.58;
	C[4] = 0.95;
	SQ = 0.0;

	for (i=0; i < 5 ;i++)
	{
		SQ = SQ+C[i]*pow(q*RG,2.0*i);
	}

	SQ = pow(SQ, -D/8.);
	if (RG == 0)
	{
		return I0;
	} else {
		return I0*SQ;
	}
}

scalar sasfit_ff_rlcaggregation_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_rlcaggregation_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

