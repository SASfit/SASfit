/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define ALPHA	param->p[1]
#define DELTA_ETA	param->p[2]

scalar sasfit_ff_boucher_profiles(scalar r, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	if (fabs(r)>fabs(R)) {
        return 0; 
    } else {
        return DELTA_ETA*pow((1-gsl_pow_2(r/R)),ALPHA/2.-2);
    }
}

scalar sasfit_ff_boucher_profiles_f(scalar r, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_boucher_profiles_v(scalar r, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

