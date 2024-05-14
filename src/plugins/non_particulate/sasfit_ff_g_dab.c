/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define XI	param->p[0]
#define H	param->p[1]
#define DUMMY2	param->p[2]
#define ETA	param->p[3]

scalar sasfit_ff_g_dab(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((XI < 0.0), param, "xi(%lg) < 0",XI); // modify condition to your needs
	SASFIT_CHECK_COND1((H <= -1.5), param, "H(%lg) <= -1.5",H);

	// insert your code here
	return gsl_pow_2(gsl_pow_3(2*XI)*gsl_sf_poch(H,1.5)*ETA)*gsl_pow_3(M_PI)/pow(1+gsl_pow_2(q*XI),1.5+H);
	return gsl_pow_2(gsl_pow_3(2*XI)*exp(gsl_sf_lngamma(1.5+H)-gsl_sf_lngamma(H))*ETA)*gsl_pow_3(M_PI)/pow(1+gsl_pow_2(q*XI),1.5+H);
}

scalar sasfit_ff_g_dab_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_g_dab_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

