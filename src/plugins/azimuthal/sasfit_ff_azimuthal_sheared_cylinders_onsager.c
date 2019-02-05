/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>


#define PSI_DEG	param->p[6]

// define shortcuts for local parameters/variables


scalar sasfit_ff_azimuthal_sheared_cylinders_onsager(scalar psi, sasfit_param * param)
{
	scalar qmod;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
	qmod = PSI_DEG;
	PSI_DEG = psi;
	return sasfit_ff_sheared_cylinders__onsager_(qmod,param);
}

scalar sasfit_ff_azimuthal_sheared_cylinders_onsager_f(scalar psi, sasfit_param * param)
{
	scalar qmod;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
	qmod = PSI_DEG;
	PSI_DEG = psi;
	return sasfit_ff_sheared_cylinders__onsager__f(qmod,param);
}

scalar sasfit_ff_azimuthal_sheared_cylinders_onsager_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

