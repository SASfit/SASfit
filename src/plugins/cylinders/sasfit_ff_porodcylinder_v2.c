/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define DUMMY	param->p[1]
#define NU	param->p[2]
#define ETA	param->p[3]

scalar sasfit_ff_porodcylinder_v2(scalar q, sasfit_param * param)
{
    scalar tmp, FC, LC;
	sasfit_param subParam;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((NU < 0.0), param, "nu(%lg) < 0",NU); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs

	// insert your code here
    if (R == 0.0) return 0.0;
	if (NU == 0.0) return 0.0;
	subParam.p[0] = R;
    subParam.p[1] = DUMMY;
    subParam.p[2] =	NU*R;
    subParam.p[3] = ETA;

	if (2.0/NU > 1.25)
	{
		return sasfit_ff_flatcylinder(q, &subParam);
	}
	else if (2.0/NU < 0.75)
	{
		return sasfit_ff_longcylinder(q, &subParam);
	} else
	{
		LC = sasfit_ff_longcylinder(q, &subParam);
		FC = sasfit_ff_flatcylinder(q, &subParam);
		tmp = (2.0/NU-0.75)*2.0;
		return tmp*FC+(1.0-tmp)*LC;
	}
}

scalar sasfit_ff_porodcylinder_v2_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_porodcylinder_v2_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return M_PI*gsl_pow_2(R)*NU*R;
}

