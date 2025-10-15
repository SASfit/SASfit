/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define T	param->p[0]
#define SIGMA_T	fabs(param->p[1])

#define ALPHA	param->p[3]

#define DETA	param->p[7]

scalar sasfit_ff_boucher_planar(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA <= -1.0), param, "alpha(%lg) <= -1",ALPHA); // modify condition to your needs

	// insert your code here
	return DETA*sqrt(M_PI)*T*gsl_sf_gamma(1+ALPHA)/gsl_sf_gamma(1.5+ALPHA)*gsl_sf_hyperg_0F1(1.5+ALPHA,-0.25*gsl_pow_2(q*T));
}

scalar sasfit_ff_boucher_planar_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return DETA*sqrt(M_PI)*T*gsl_sf_gamma(1+ALPHA)/gsl_sf_gamma(1.5+ALPHA)*gsl_sf_hyperg_0F1(1.5+ALPHA,-0.25*gsl_pow_2(q*T));
;
}

scalar sasfit_ff_boucher_planar_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

