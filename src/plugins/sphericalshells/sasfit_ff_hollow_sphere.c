/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define DR	param->p[1]
#define DUMMY	param->p[2]
#define ETA	param->p[3]

scalar sasfit_ff_hollow_sphere(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((DR < 0.0), param, "dR(%lg) < 0",DR); // modify condition to your needs

	// insert your code here
	return gsl_pow_2(sasfit_ff_hollow_sphere_f(q,param));
}

scalar sasfit_ff_hollow_sphere_f(scalar q, sasfit_param * param)
{
    scalar k1;
    sasfit_param subParam;
	sasfit_init_param( &subParam );

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
    subParam.p[0] = R;
    subParam.p[1] = DR;
    subParam.p[2] = 0;
	subParam.p[3] = ETA;
	k1 = sasfit_ff_spherical_shell_iii_f(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);
	return k1;
}

scalar sasfit_ff_hollow_sphere_v(scalar x, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return sasfit_ff_spherical_shell_iii_v(x,param,dist);
}

