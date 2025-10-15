/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define NU	param->p[1]
#define MU	param->p[2]
#define ETA	param->p[3]

scalar sasfit_ff_spherical_shell_ii(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((NU < 0.0), param, "nu(%lg) < 0",NU); // modify condition to your needs
	SASFIT_CHECK_COND1((NU >= 1.0), param, "nu(%lg) >= 1",NU); // modify condition to your needs

	// insert your code here
	return gsl_pow_2(sasfit_ff_spherical_shell_ii_f(q,param));
}

scalar sasfit_ff_spherical_shell_ii_f(scalar q, sasfit_param * param)
{
    scalar k1, k2;
	sasfit_param subParam;
	sasfit_init_param( &subParam );

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

	k1 = sasfit_ff_sphere_f(q, param);

	subParam.p[0] = NU*R;
	subParam.p[3] = ETA*(1 - MU);
	k2 = sasfit_ff_sphere_f(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);
	return k1-k2;
}

scalar sasfit_ff_spherical_shell_ii_v(scalar x, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
    if ( dist != 0 ) x = R;

    return 4.0/3.0 * M_PI * gsl_pow_3(x);
}

