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
#define ALPHA		param->p[6]
#define ETA_SOL		param->p[7]

scalar expprof(scalar x, sasfit_param * param) {
	scalar eta_sh_in, eta_sh_out;

	eta_sh_in	= X_IN_SOL	*ETA_SOL + (1-X_IN_SOL)*ETA_SHELL;
	eta_sh_out	= X_OUT_SOL	*ETA_SOL + (1-X_OUT_SOL)*ETA_SHELL;

	if (ALPHA<0) {
		return eta_sh_in+(eta_sh_out-eta_sh_in)*x*exp((1-x)*ALPHA);
	} else {
		return eta_sh_out+(eta_sh_in-eta_sh_out)*(1-x)*exp(-x*ALPHA);
	}
}
scalar sasfit_ff_radial_profile_of_expshell(scalar r, sasfit_param * param)
{
	scalar x;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	x = (fabs(r)-R_CORE)/DR;
	if (fabs(r)<R_CORE) {
		return ETA_CORE;
	}  else if ((fabs(r)<(R_CORE+DR)) && fabs(r)>=R_CORE){
		return expprof(x,param);
	} else {
		return ETA_SOL;
	}
}

scalar sasfit_ff_radial_profile_of_expshell_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_radial_profile_of_expshell_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

