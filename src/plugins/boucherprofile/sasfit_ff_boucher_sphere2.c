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

scalar sasfit_ff_boucher_sphere2(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
 //   SASFIT_CHECK_COND1((ALPHA <= 2.0), param, "alpha(%lg) <=2",ALPHA);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs

	// insert your code here
	return gsl_pow_2(sasfit_ff_boucher_sphere2_f(q,param));
}

scalar sasfit_ff_boucher_sphere2_f(scalar q, sasfit_param * param)
{
    scalar beta;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
 //   beta = DELTA_ETA*gsl_pow_3(R)*sqrt(M_PI)*M_PI*exp(gsl_sf_lngamma(ALPHA/2.-1)-gsl_sf_lngamma(ALPHA/2.+0.5));

//   beta = DELTA_ETA*gsl_pow_3(R*sqrt(M_PI))*gsl_sf_gamma(ALPHA/2.-1)/gsl_sf_gamma(ALPHA/2.+0.5);
   beta  = DELTA_ETA*gsl_pow_3(R)*4./3.*M_PI;
    if (q*R == 0) return beta;
	return beta*gsl_sf_hyperg_0F1(0.5*(ALPHA+1),-gsl_pow_2(q*R/2.));
}

scalar sasfit_ff_boucher_sphere2_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 4./3. * M_PI*gsl_pow_3(R);
}

