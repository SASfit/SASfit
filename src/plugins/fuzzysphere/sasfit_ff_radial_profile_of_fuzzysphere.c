/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define SIGMA	param->p[1]
#define ETA_SPH	param->p[2]
#define ETA_SOL	param->p[3]

scalar sasfit_ff_radial_profile_of_fuzzysphere(scalar r, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	if (fabs(SIGMA)==0.0) {
		if (fabs(r) <= fabs(R) ) {
			return ETA_SPH;
		} else {
			return ETA_SOL;
		}
	} else {
		if (r==0.0) {
			return (ETA_SPH-ETA_SOL)*
				(gsl_sf_erf(R/(sqrt(2.0)*SIGMA))
				-exp(-(gsl_pow_2(R/SIGMA)/2.0)) * sqrt(2.0/M_PI)*R/SIGMA
				) + ETA_SOL;
		} else {
			return (ETA_SPH-ETA_SOL)*
			     ( ( exp(-0.5*gsl_pow_2((fabs(r)+fabs(R))/fabs(SIGMA)))
				    -exp(-0.5*gsl_pow_2((fabs(r)-fabs(R))/fabs(SIGMA)))
				   )*SIGMA/(sqrt(2.0*M_PI)*fabs(r))
				+  0.5*gsl_sf_erf((fabs(r)+fabs(R))/(sqrt(2.0)*fabs(SIGMA)))
				-  0.5*gsl_sf_erf((fabs(r)-fabs(R))/(sqrt(2.0)*fabs(SIGMA)))
				 )
			   + ETA_SOL;
		}
	}
}

scalar sasfit_ff_radial_profile_of_fuzzysphere_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_radial_profile_of_fuzzysphere_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

