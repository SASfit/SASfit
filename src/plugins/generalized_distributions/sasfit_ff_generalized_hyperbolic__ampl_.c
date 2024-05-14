/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define SIGMA	param->p[1]
#define XI	param->p[2]
#define DUMMY	param->p[3]
#define SHIFT	param->p[4]
#define Y0	param->p[5]

scalar sasfit_ff_generalized_hyperbolic__ampl_(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A < 0.0), param, "A(%lg) < 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA < 0.0), param, "sigma(%lg) < 0",SIGMA); // modify condition to your needs
	SASFIT_CHECK_COND1((XI < 0.0), param, "xi(%lg) < 0",XI); // modify condition to your needs
	SASFIT_CHECK_COND1((DUMMY < 0.0), param, "dummy(%lg) < 0",DUMMY); // modify condition to your needs
	SASFIT_CHECK_COND1((SHIFT < 0.0), param, "shift(%lg) < 0",SHIFT); // modify condition to your needs
	SASFIT_CHECK_COND1((Y0 < 0.0), param, "y0(%lg) < 0",Y0); // modify condition to your needs

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_generalized_hyperbolic__ampl__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_generalized_hyperbolic__ampl__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

