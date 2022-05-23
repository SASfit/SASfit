/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

scalar eq(scalar q, scalar x) {
	if (q==1) {
		return exp(x);
	} else if (q!=1 && (1.+(1.-q)*x)>0) {
		return pow(1.+(1.-q)*x,1./(1.-q));
	} else {
		return 0;
	}
}

scalar Cq(scalar q) {
	if (q==1.) {
		return sqrt(M_PI);
	} else if (q < 1.) {
		return 2.*sqrt(M_PI)*gsl_sf_gamma(1./(1.-q))/((3.-q)*sqrt(1.-q)*gsl_sf_gamma((3.-q)/(2.*(1.-q))));
	} else if (1.<q && q<3.){
		return sqrt(M_PI)*gsl_sf_gamma((3.-q)/(2.*(q-1.)))/(sqrt(q-1.)*gsl_sf_gamma(1./(q-1.)));
	} else {
	    return GSL_NAN;
	}
}

scalar lnq(scalar q, scalar x) {
	if (q==1 && x >0) {
		return log(x);
	} else if (q!=0 && x>0) {
		return (pow(x,1-q)-1)/(1-q);
	} else {
		return GSL_NAN;
	}
}
