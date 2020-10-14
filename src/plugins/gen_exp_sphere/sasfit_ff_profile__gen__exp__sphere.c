/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]
#define ALPHA	param->p[1]
#define DETA	param->p[2]
#define BETA    ((3+ALPHA)*(4+ALPHA))

scalar sasfit_ff_profile__gen__exp__sphere(scalar r, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA <= -3.0), param, "alpha(%lg) <= -3",ALPHA); // modify condition to your needs
	// insert your code here

	if (ALPHA==0) return DETA*pow(BETA,((3+ALPHA)/2.))
                *exp(-fabs(r/RG)*sqrt(BETA))
                /gsl_sf_gamma(3+ALPHA);
    if (ALPHA < 10) {
        return DETA*pow(BETA,((3+ALPHA)/2.))
                *exp(-fabs(r/RG)*sqrt(BETA))
                *pow(fabs(r/RG),ALPHA)
                /gsl_sf_gamma(3+ALPHA);
    } else {
        return DETA*exp(
                  log(BETA)*((3+ALPHA)/2.)
                +(-fabs(r/RG)*sqrt(BETA))
                +log(fabs(r/RG))*(ALPHA)
                -gsl_sf_lngamma(3+ALPHA));
    }
	return DETA*(pow((3 + BETA)*(4 + BETA),(3 + BETA)/2.)*pow(fabs(r)/RG,BETA))/
   (exp((fabs(r)*sqrt(gsl_sf_gamma(5 + BETA)/gsl_sf_gamma(3 + BETA)))/RG)*gsl_sf_gamma(3 + BETA));
}

scalar sasfit_ff_profile__gen__exp__sphere_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_profile__gen__exp__sphere_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

