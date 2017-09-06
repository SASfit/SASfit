/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define SIGMA	param->p[1]
#define ETA_SPH	param->p[2]
#define ETA_SOL	param->p[3]

scalar sasfit_ff_fuzzysphere(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);

	// insert your code here
	return gsl_pow_2(sasfit_ff_fuzzysphere_f(q,param));
}

scalar sasfit_ff_fuzzysphere_f(scalar q, sasfit_param * param)
{
	scalar u,v;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
    SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	
	u = fabs(q*R);
	v=fabs(q*SIGMA);
	// insert your code here
	if (u==0) {
		return sasfit_ff_fuzzysphere_v(R,param,1)
			* exp(-0.5*gsl_pow_2(v))
			* (ETA_SPH-ETA_SOL);
	} else {
		return sasfit_ff_fuzzysphere_v(R,param,1)
			* 3*(sin(u)-u*cos(u))/gsl_pow_3(u)
			* exp(-0.5*gsl_pow_2(v))
			* (ETA_SPH-ETA_SOL);
	}
}

scalar sasfit_ff_fuzzysphere_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 4./3.*M_PI*gsl_pow_3(fabs(R));
}

