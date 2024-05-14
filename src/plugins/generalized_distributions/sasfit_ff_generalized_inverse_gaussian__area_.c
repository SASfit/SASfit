/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define THETA fabs(param->p[1])
#define ETA fabs(param->p[2])
#define P param->p[3]
#define DUMMY param->p[4]
#define Y0	param->p[5]

scalar sasfit_ff_generalized_inverse_gaussian__area_(scalar x, sasfit_param * param)
// https://en.wikipedia.org/wiki/Generalized_inverse_Gaussian_distribution
{
    scalar a,b,y;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	a = THETA/ETA;
	b = THETA*ETA;
	// insert your code here
	if (x<=0) return Y0;
	if (P>=0) {
        y=Y0+A*pow(a/b,P/2.)*pow(x,P-1)*exp(-(a*x+b/x)/2.)/(2*gsl_sf_bessel_Knu(P,sqrt(a*b)));
	} else if (round(P)==P) {
	    y=Y0+A*pow(a/b,P/2.)*pow(x,P-1)*exp(-(a*x+b/x)/2.)/(2*gsl_sf_bessel_Kn(P,sqrt(a*b)));
	} else {
	    return Y0+A*pow(a/b,P/2.)*pow(x,P-1)*exp(-(a*x+b/x)/2.)/(2*gsl_sf_bessel_Knu(fabs(P),sqrt(a*b)));
	    y=Y0+A*pow(a/b,P/2.)*pow(x,P-1)*exp(-(a*x+b/x)/2.)/(2*r8_besk(P,sqrt(a*b)));
	}
	return y;
}

scalar sasfit_ff_generalized_inverse_gaussian__area__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_generalized_inverse_gaussian__area__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

