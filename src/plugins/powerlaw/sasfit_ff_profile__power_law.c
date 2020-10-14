/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define ALPHA	param->p[1]
#define DETA	param->p[2]

scalar sasfit_ff_profile__power_law(scalar r, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA <= -3.0), param, "alpha(%lg) <= -3",ALPHA); // modify condition to your needs

	// insert your code here
	if (fabs(r)<fabs(R) && fabs(R)>0) {
        return DETA*(1-pow(fabs(r/R),ALPHA));
	} else if (r==0 && R==0) {
	    return DETA;
	} else {
	    return 0;
	}
}

scalar sasfit_ff_profile__power_law_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_profile__power_law_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

