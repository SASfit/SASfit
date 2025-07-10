/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define DR	param->p[1]
#define CONTRAST	param->p[2]

scalar sasfit_ff_hollowsphere(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((DR < 0.0), param, "DR(%lg) < 0",DR); // modify condition to your needs
	SASFIT_CHECK_COND1((CONTRAST < 0.0), param, "Contrast(%lg) < 0",CONTRAST); // modify condition to your needs

	// insert your code here
	return gsl_pow_2(sasfit_ff_hollowsphere_f(q, param));
}

scalar sasfit_ff_hollowsphere_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
    scalar FF;
	sasfit_param subParam;
	sasfit_init_param( &subParam );
	subParam.p[0] = R+DR;
    subParam.p[1] = CONTRAST;
    FF = sasfit_ff_sphere_f(q, &subParam);
	subParam.p[0] = R;
	FF=FF-sasfit_ff_sphere_f(q, &subParam);
	// insert your code here
	return FF;
}

scalar sasfit_ff_hollowsphere_v(scalar x, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return sasfit_ff_sphere_v(x,param,dist);
}

