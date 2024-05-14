/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]
#define MW	param->p[1]
#define MN	param->p[2]
#define I0	param->p[3]

scalar sasfit_ff_gauspoly(scalar q, sasfit_param * param)
{
    scalar u,w,k, na;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((MW < 0.0), param, "Mw(%lg) < 0",MW); // modify condition to your needs
	SASFIT_CHECK_COND1((MN < 0.0), param, "Mn(%lg) < 0",MN); // modify condition to your needs

	// insert your code here
	na = 6.0221415e23;
	u = gsl_pow_2(q*RG);
	w = MW/MN-1.0;
	k = u/(1.0 + 2.0*w);

	if ((u < 1e-6) && (w < 1e-6)) {
        return I0*(1-u/3.+1/12.*u*u*(1+w-2*w*w));
	}
	if ((u < 1e-6) ) {
        return I0*(1-u/3.+gsl_pow_3(u)*(1+3*w)*(1+4*w)/(60.*gsl_pow_2(1+2*w))+gsl_pow_2(u)*(1+3*w)/(12+24*w));
	}
	if ((w < 1e-6) ) {
        return I0*((24 + w*(72 + 12*u*(4 + u) + 24*w + u*(48 + u*(36 + u*(16 + 3*u)))*w) -
                    24*exp(u)*(1 + w*(3 + w) + u*(-1 + (-1 + w)*w)))/(12.*exp(u)*u*u));
	}
	return I0*2.0*(pow(1+w*k,-1.0/w)+k-1.0)/((1+w)*k*k);
}

scalar sasfit_ff_gauspoly_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_gauspoly_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

