/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define XI	param->p[0]
#define DUMMY1	param->p[1]
#define DUMMY2	param->p[2]
#define ETA	param->p[3]

scalar sasfit_ff_debye_anderson_brumberger(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((XI < 0.0), param, "xi(%lg) < 0",XI); // modify condition to your needs

	// insert your code here
	return gsl_pow_2(ETA*gsl_pow_3(XI*2)*M_PI)/gsl_pow_2(gsl_pow_2(q*XI)+1.0);
}

scalar sasfit_ff_debye_anderson_brumberger_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_debye_anderson_brumberger_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

