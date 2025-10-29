/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define T	fabs(param->p[0])
#define T12 (T/2.)

#define ALPHA	param->p[3]

#define DELTA	param->p[7]

scalar sasfit_ff_profile_boucher_planar(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((T < 0.0), param, "T(%lg) < 0",T); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA <= 0.0), param, "alpha(%lg) <= 0",ALPHA); // modify condition to your needs


	// insert your code here
	if (fabs(x)>T12)return 0.0;
	return pow(1-gsl_pow_2(x/T12),ALPHA/2.-1);
}

scalar sasfit_ff_profile_boucher_planar_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_profile_boucher_planar_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

