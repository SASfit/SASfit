/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define DUMMY1	param->p[1]
#define DUMMY2	param->p[2]
#define ALPHA	param->p[3]
#define DUMMY4	param->p[4]
#define DUMMY5	param->p[5]
#define DUMMY6	param->p[6]
#define DETA	param->p[7]

scalar sasfit_ff_profile_boucher_cyl(scalar r, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA <= 2.0), param, "alpha(%lg) <= 2",ALPHA); // modify condition to your needs

	// insert your code here
	if (fabs(r)>R) return 0;
	return DETA*pow(1-gsl_pow_2(abs(r)/R),ALPHA*0.5-2);
}

scalar sasfit_ff_profile_boucher_cyl_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0;
}

scalar sasfit_ff_profile_boucher_cyl_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0;
}

