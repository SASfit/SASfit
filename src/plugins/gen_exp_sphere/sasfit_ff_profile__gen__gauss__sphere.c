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
#define BETA	(3+ALPHA)

scalar sasfit_ff_profile__gen__gauss__sphere(scalar r, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA <= -3.0), param, "alpha(%lg) <= -3",ALPHA); // modify condition to your needs
	// insert your code here
	
	if (ALPHA==0) return DETA*pow(2,-((1+ALPHA)/2.))*pow(BETA,BETA/2)
                *exp(-gsl_pow_2(r/RG)*BETA/2.)
                /gsl_sf_gamma(BETA/2);
    if (ALPHA < 10) {
        return DETA*pow(2,-((1+ALPHA)/2.))*pow(BETA,BETA/2)
                *exp(-gsl_pow_2(r/RG)*BETA/2.)
                *pow(fabs(r/RG),ALPHA)
                /gsl_sf_gamma(BETA/2);
    } else {
        return DETA*pow(2,-((1+ALPHA)/2.))
                *exp(-gsl_pow_2(r/RG)*BETA/2.
				     -gsl_sf_lngamma(BETA/2)
					 +BETA/2*log(BETA)
					 +ALPHA*log(fabs(r/RG))
					);
	}
}

scalar sasfit_ff_profile__gen__gauss__sphere_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_profile__gen__gauss__sphere_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

