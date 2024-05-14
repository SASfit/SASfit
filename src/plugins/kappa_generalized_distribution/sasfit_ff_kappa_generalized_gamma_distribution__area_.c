/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

scalar sasfit_ff_kappa_generalized_gamma_distribution__area_(scalar x, sasfit_param * param)
{
    scalar z;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((KAPPA < 0.0), param, "kappa(%lg) < 0",KAPPA); // modify condition to your needs
	SASFIT_CHECK_COND1((KAPPA >= 1.0), param, "kappa(%lg) >= 1",KAPPA); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA <= 0.0), param, "alpha(%lg) <= 0",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA <= 0.0), param, "beta(%lg) <= 0",BETA); // modify condition to your needs
	SASFIT_CHECK_COND1((NU <= 0.0), param, "nu(%lg) <= 0",NU); // modify condition to your needs
	SASFIT_CHECK_COND1((NU >= 1./KAPPA), param, "nu(%lg) >= 1/kappa",NU); // modify condition to your needs

	// insert your code here
	z = (x-XI)/SIGMA;
	if (x<XI) return 0.0;
	if (KAPPA == 0) return A*fabs(ALPHA)*pow(BETA,NU)/gsl_sf_gamma(NU)*pow(z,ALPHA*NU-1)*exp_kappa(-BETA*pow(z,ALPHA),param);
	return A/SIGMA*(1+KAPPA*NU)*pow(2*KAPPA*BETA,NU)*ALPHA*
	        pow(z,ALPHA*NU-1)*exp_kappa(-BETA*pow(z,ALPHA),param)*
	        exp(gsl_sf_lngamma(1./(2*KAPPA)+NU/2)-gsl_sf_lngamma(1./(2*KAPPA)-NU/2)-gsl_sf_lngamma(NU));

}

scalar sasfit_ff_kappa_generalized_gamma_distribution__area__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_kappa_generalized_gamma_distribution__area__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

