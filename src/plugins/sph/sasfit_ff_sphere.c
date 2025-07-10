/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define ETA	param->p[1]

scalar sasfit_ff_sphere(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs

	// insert your code here
	return gsl_pow_2(sasfit_ff_sphere_f(q, param));
}

scalar sasfit_ff_sphere_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
    scalar qr;
	qr=q*R;
	if (qr == 0.0) {
		return 4./3. * M_PI * gsl_pow_3(R)*ETA;
	} else {
		return 4./3. * M_PI * gsl_pow_3(R)*ETA * 3.0*(sin(qr)-qr*cos(qr))/gsl_pow_3(qr);
	}
}

scalar sasfit_ff_sphere_v(scalar x, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 4./3. * M_PI * gsl_pow_3(x);
}

