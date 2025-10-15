/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define ETA	param->p[3]

scalar sasfit_ff_sphere(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs

	// insert your code here
	return gsl_pow_2(sasfit_ff_sphere_f(q,param));
}

scalar sasfit_ff_sphere_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	if (q*R<1e-4) {
		return ETA*4.0/3.0*M_PI*R*R*R*(1 - gsl_pow_2(q*R)/10. + gsl_pow_4(q*R)/280. - gsl_pow_6(q*R)/15120.);
	} else {
		return ETA*4.0*M_PI*(sin(q*R) - q*R*cos(q*R))/gsl_pow_3(q);
	}
}

scalar sasfit_ff_sphere_v(scalar x, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	if ( dist != 0 ) x = R;

    return 4.0/3.0 * M_PI * gsl_pow_3(x);
}

