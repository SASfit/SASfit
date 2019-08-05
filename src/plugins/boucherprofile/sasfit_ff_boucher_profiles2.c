/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define ALPHA	param->p[1]
#define DELTA_ETA	param->p[2]

scalar sasfit_ff_boucher_profiles2(scalar r, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((ALPHA <= 2.0), param, "alpha(%lg) <=2",ALPHA); // modify condition to your needs


	// insert your code here
	if (fabs(r)>fabs(R)) {
        return 0; 
    } else {
        return DELTA_ETA*pow((1-gsl_pow_2(r/R)),ALPHA/2.-2)*4./3.*M_PI/
           (gsl_pow_3(sqrt(M_PI))*gsl_sf_gamma(ALPHA/2.-1)/gsl_sf_gamma(ALPHA/2.+0.5));
    }
}

scalar sasfit_ff_boucher_profiles2_f(scalar r, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_boucher_profiles2_v(scalar r, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

