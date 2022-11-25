/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

scalar sasfit_ff_langevin_static_up_down(scalar t, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((NU < 0.0), param, "NU(%lg) < 0",NU); // modify condition to your needs

	PSI = PSI_DEG*M_PI/180.0;
	AD = 1.0;
	R=1.0;
	R_AV=R;
	TEMPERATURE = 273.0;
	DAMP_ALPHA = 1.0;
	STATIC = 1.0;
	
	P_POL = -fabs(P_POL);
	EPSILON_FLIPPER = 1.0;

	T_PLUS = 0.0;
	T_MINUS = 1.0;

	return i_measured(t,param);
}

scalar sasfit_ff_langevin_static_up_down_f(scalar t, sasfit_param * param)
{
	// insert your code here
	return 0.0;
}

scalar sasfit_ff_langevin_static_up_down_v(scalar t, sasfit_param * param, int dist)
{
	// insert your code here
	return 0.0;
}

