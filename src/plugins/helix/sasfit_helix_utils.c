/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */


#include "include/private.h"
#include <sasfit_error_ff.h>

scalar sinc(scalar x) {
	if (fabs(x)<1e-6) {
		return 1-gsl_pow_2(x)/6.0+gsl_pow_4(x)/120.-gsl_pow_6(x)/5040.+gsl_pow_8(x)/362880.;
	} else {
		return sin(x)/x;
	}
}

scalar thinrod_helix(scalar q, scalar L)
{
	scalar u,u2;
	scalar P;
	u = q*L;
	u2 = u/2.0;
	if (u==0.0) {
		P = L*L;
	} else {
		P =L*L*(2.0*gsl_sf_Si(u)/u-gsl_pow_2(sin(u2)/u2) );
	}

	return P;
}