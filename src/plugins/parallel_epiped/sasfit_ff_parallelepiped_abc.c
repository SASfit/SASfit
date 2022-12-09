/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include <sasfit.h>

#include <sasfit_common.h>
#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define NU	param->p[3]

scalar sasfit_ff_parallelepiped_abc(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B < 0.0), param, "b(%lg) < 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C < 0.0), param, "c(%lg) < 0",C); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs

	Q=q;
	NUA = A;
	NUB = B;
	NUC = C;
	// insert your code here
	return sasfit_orient_avg(&KernelSphAvg_P,param);
}

scalar sasfit_ff_parallelepiped_abc_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	sasfit_err("sasfit_ff_parallelepiped_abc_f not implemented yet\n");
	return 0.0;
}

scalar sasfit_ff_parallelepiped_abc_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return A*B*C;
}

