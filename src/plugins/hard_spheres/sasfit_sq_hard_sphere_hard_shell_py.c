/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables

#define R	param->p[0]
#define ETA	param->p[1]
#define DELTA_R	param->p[2]

scalar sasfit_sq_hard_sphere_hard_shell_py(scalar q, sasfit_param * param)
{
	sasfit_param subParam;

	// insert your code here

    sasfit_init_param( &subParam );
	subParam.p[0] = R+DELTA_R;
    subParam.p[1] = ETA*gsl_pow_3(1+DELTA_R/R);
    if (!gsl_finite(subParam.p[1])) subParam.p[1] = ETA;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA >= 1.0), param, "eta(%lg) >= 1",ETA);
	SASFIT_CHECK_COND1((DELTA_R < 0.0), param, "Delta_R(%lg) < 0",DELTA_R); // modify condition to your needs

	// insert your code here

	return sasfit_sq_hard_sphere__py_(q,&subParam);
}

scalar sasfit_sq_hard_sphere_hard_shell_py_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_hard_sphere_hard_shell_py_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

