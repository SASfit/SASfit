/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define DUMMY	param->p[1]
#define L	param->p[2]
#define ETA	param->p[3]

scalar sasfit_ff_porodcylinder(scalar q, sasfit_param * param)
{
    scalar tmp, FC, LC;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs

	// insert your code here
    if (R == 0.0) return 0.0;
	if (L == 0.0) return 0.0;

	if (2.0*R/L > 1.25)
	{
		return sasfit_ff_flatcylinder(q, param);
	}
	else if (2.0*R/L < 0.75)
	{
		return sasfit_ff_longcylinder(q, param);
	} else
	{
		LC = sasfit_ff_longcylinder(q, param);
		FC = sasfit_ff_flatcylinder(q, param);
		tmp = (2.0*R/L-0.75)*2.0;
		return tmp*FC+(1.0-tmp)*LC;
	}
}

scalar sasfit_ff_porodcylinder_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_porodcylinder_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return M_PI*gsl_pow_2(R)*L;
}

