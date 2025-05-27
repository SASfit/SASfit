/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define TILT0	param->p[0]
#define DELTA_TILT	param->p[1]
#define D001	param->p[2]

scalar lambda(scalar rot, scalar tilt) {
    return
        (12721
           - 611.74127*tilt
           - 12.44417*gsl_pow_2(tilt)
           - 0.12411*gsl_pow_3(tilt)
           + 0.00583*gsl_pow_4(tilt))/rot
        + 0.0122
        + 3.61e-4*tilt
        + 3.14e-4*gsl_pow_2(tilt)
        + 3.05e-5*gsl_pow_3(tilt)
        + 9.32e-7*gsl_pow_4(tilt) ;
}

scalar sasfit_ff_tiltsans1(scalar rot, sasfit_param * param)
{
    scalar lam,q,tilt,d;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((rot < 0.0), param, "rot(%lg) < 0",rot);
	SASFIT_CHECK_COND1((D001 < 0), param, "d001(%lg) < 0",D001); // modify condition to your needs

	// insert your code here
	tilt=-TILT0-DELTA_TILT;
	d=D001;
	lam = lambda(rot,tilt);
	return 2*asin(lam/(2*d))*180/M_PI;
}

scalar sasfit_ff_tiltsans1_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_tiltsans1_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

