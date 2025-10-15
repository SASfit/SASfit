/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R1	param->p[0]
#define R2	param->p[1]
#define MU	param->p[2]
#define ETA	param->p[3]

scalar sasfit_ff_spherical_shell_i(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R1 < 0.0), param, "R1(%lg) < 0",R1); // modify condition to your needs
	SASFIT_CHECK_COND1((R2 < 0.0), param, "R2(%lg) < 0",R2); // modify condition to your needs

	// insert your code here
	return gsl_pow_2(sasfit_ff_spherical_shell_i_f(q,param));
}

scalar sasfit_ff_spherical_shell_i_f(scalar q, sasfit_param * param)
{
	scalar k1, k2;
	sasfit_param subParam;
	sasfit_init_param( &subParam );

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

	k1 = sasfit_ff_sphere_f(q, param);

	subParam.p[0] = R2;
	subParam.p[3] = ETA*(1 - MU);
	k2 = sasfit_ff_sphere_f(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);
	return k1-k2;
}

scalar sasfit_ff_spherical_shell_i_v(scalar x, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

    if ( dist != 0 ) x = R1;

    return 4.0/3.0 * M_PI * gsl_pow_3(x);
}

