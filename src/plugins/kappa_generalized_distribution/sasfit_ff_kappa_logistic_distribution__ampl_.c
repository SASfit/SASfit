/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

scalar sasfit_ff_kappa_logistic_distribution__ampl_(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

    scalar z,mode,Norm;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((KAPPA < 0.0), param, "kappa(%lg) < 0",KAPPA); // modify condition to your needs
	SASFIT_CHECK_COND1((KAPPA >= 1.0), param, "kappa(%lg) >= 1",KAPPA); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA <= 0.0), param, "alpha(%lg) <= 0",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA <= 0.0), param, "beta(%lg) <= 0",BETA); // modify condition to your needs
	SASFIT_CHECK_COND1((NU <= 0.0), param, "nu(%lg) <= 0",NU); // modify condition to your needs
	SASFIT_CHECK_COND1((NU >= 1./KAPPA), param, "nu(%lg) >= 1/kappa",NU); // modify condition to your needs
	SASFIT_CHECK_COND1((NU-1/ALPHA > 1./KAPPA), param, "(nu-1/alpha)(%lg) > 1/kappa",NU-1/ALPHA); // modify condition to your needs


	// insert your code here

	z = (x-XI)/SIGMA;
	if (x<=XI) return 0.0;
	mode = mode_kappa(param);
	Norm = sasfit_ff_kappa_logistic_distribution__area_(mode,param);
//	sasfit_out("mode:%lf, Norm:%lf\n",mode,Norm);
	return A*sasfit_ff_kappa_logistic_distribution__area_(x,param)/Norm;
}

scalar sasfit_ff_kappa_logistic_distribution__ampl__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_kappa_logistic_distribution__ampl__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

