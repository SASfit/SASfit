/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define I0	param->p[0]
#define XI	param->p[1]
#define Q0	param->p[2]
#define M	param->p[3]
#define P	param->p[4]

scalar sasfit_ff_broad_peak(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((I0 < 0.0), param, "I0(%lg) < 0",I0); // modify condition to your needs
	SASFIT_CHECK_COND1((XI < 0.0), param, "xi(%lg) < 0",XI); // modify condition to your needs
	SASFIT_CHECK_COND1((Q0 < 0.0), param, "q0(%lg) < 0",Q0); // modify condition to your needs
	SASFIT_CHECK_COND1((M < 0.0), param, "m(%lg) < 0",M); // modify condition to your needs
	SASFIT_CHECK_COND1((P < 0.0), param, "p(%lg) < 0",P); // modify condition to your needs

	// insert your code here
	return I0/pow(1.0+pow(fabs(q-Q0)*XI,M),P);
}

scalar sasfit_ff_broad_peak_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_broad_peak_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

