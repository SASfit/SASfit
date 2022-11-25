/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R_CORE		fabs(param->p[0])
#define DR			fabs(param->p[1])
#define ETA_CORE	param->p[2]
#define ETA_SHELL	param->p[3]
#define X_IN_SOL	param->p[4]
#define X_OUT_SOL	param->p[5]
#define ETA_SOL		param->p[6]


scalar sasfit_ff_radial_profile_of_linshell(scalar r, sasfit_param * param)
{
	scalar m,b,eta_sh_in, eta_sh_out;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

	eta_sh_in = (1- X_IN_SOL)*ETA_SHELL+ X_IN_SOL*ETA_SOL;
	eta_sh_out= (1-X_OUT_SOL)*ETA_SHELL+X_OUT_SOL*ETA_SOL;
	m=1e100;
	if (DR !=0) {
		m=(eta_sh_out-eta_sh_in)/DR;
	}
	b=-m*R_CORE+eta_sh_in;
	if (fabs(r)<=R_CORE) {
		return ETA_CORE;
	} else if ((fabs(r)>R_CORE) && (fabs(r)<R_CORE+DR)) {
		return m*fabs(r)+b;
	} else {
		return ETA_SOL;
	}
}

scalar sasfit_ff_radial_profile_of_linshell_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_radial_profile_of_linshell_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

