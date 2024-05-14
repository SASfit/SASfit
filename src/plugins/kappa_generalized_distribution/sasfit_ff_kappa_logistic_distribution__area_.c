/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

scalar sasfit_ff_kappa_logistic_distribution__area_(scalar x, sasfit_param * param)
{
    scalar z;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((KAPPA < 0.0), param, "kappa(%lg) < 0",KAPPA); // modify condition to your needs
	SASFIT_CHECK_COND1((KAPPA >= 1.0), param, "kappa(%lg) >= 1",KAPPA); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA <= 0.0), param, "alpha(%lg) <= 0",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA <= 0.0), param, "beta(%lg) <= 0",BETA); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA <= 0.0), param, "lambda(%lg) <= 0",LAMBDA); // modify condition to your needs

	// insert your code here
	z = (x-XI)/SIGMA;
	if (x<XI) return 0.0;

	return A/SIGMA  * LAMBDA*ALPHA*BETA*pow(z,ALPHA-1)/sqrt(1+gsl_pow_2(KAPPA*BETA*pow(z,ALPHA)))
                    * exp_kappa(-BETA*pow(z,ALPHA),param)
                    / gsl_pow_2(1+(LAMBDA-1)*exp_kappa(-BETA*pow(z,ALPHA),param));

}

scalar sasfit_ff_kappa_logistic_distribution__area__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_kappa_logistic_distribution__area__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

