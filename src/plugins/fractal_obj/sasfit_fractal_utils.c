/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

scalar sphere_f(scalar q, scalar R, scalar Deta) {
	scalar x,beta;
	x=q*R;
	beta=4./3.*M_PI*gsl_pow_3(R)*Deta;
//	return beta*gsl_sf_hyperg_0F1(2.5,-0.25*x*x);
	if (fabs(x) < 1e-6)	{
		return beta*(1-gsl_pow_2(x)/10.+gsl_pow_4(x)/280.0-gsl_pow_6(x)/15120.+gsl_pow_8(x)/1330560.);
	} else {
		return beta*3*gsl_sf_bessel_j1(x)/x;
	}

}
