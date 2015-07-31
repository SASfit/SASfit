/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

scalar sasfit_ff_tisane_tau(scalar t, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((NU < 0.0), param, "NU(%lg) < 0",NU); // modify condition to your needs

	PSI = PSI_DEG*M_PI/180.0;
	AD = 1.0;
	R=1.0;
	R_AV=R;
	TEMPERATURE = 273.0;
	ALPHA_PHASE = -GSL_SIGN(PHASE_PAR)*(asin(1./sqrt(gsl_pow_2(NU*2*M_PI*PHASE_PAR)+1.))-M_PI/2.0);
	DAMP_ALPHA = 1./sqrt(gsl_pow_2(NU*2*M_PI*PHASE_PAR)+1.);
	STATIC = 0.0;

	if (LAM_CENTER > D_LAM_LAM) {
		DELTA_T_BOTTOM = LAM_CENTER;
		DELTA_T_TOP = D_LAM_LAM;
	} else {
		DELTA_T_TOP = LAM_CENTER;
		DELTA_T_BOTTOM = D_LAM_LAM;
	}
	
	LAM_CENTER = 0;
	D_LAM_LAM = 0;
	SD = t;
	
	return i_measured_tisane(t,param);
}

scalar sasfit_ff_tisane_tau_f(scalar t, sasfit_param * param)
{
	// insert your code here
	return 0.0;
}

scalar sasfit_ff_tisane_tau_v(scalar t, sasfit_param * param, int dist)
{
	// insert your code here
	return 0.0;
}

