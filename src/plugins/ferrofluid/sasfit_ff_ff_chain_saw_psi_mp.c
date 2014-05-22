/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

scalar sasfit_ff_ff_chain_saw_psi_mp(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q <= 0.0), param, "q(%lg) <= 0",q);
	SASFIT_CHECK_COND1((R_CORE < 0.0), param, "R_core(%lg) < 0",R_CORE); // modify condition to your needs
	SASFIT_CHECK_COND1((SNAGG < 0.0), param, "nagg(%lg) < 0",SNAGG); // modify condition to your needs
	SASFIT_CHECK_COND1((VBRUSH < 0.0), param, "Vbrush(%lg) < 0",VBRUSH); // modify condition to your needs
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((T_BRUSH_CONST < 0.0), param, "t_brush_const(%lg) < 0",T_BRUSH_CONST); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA_BRUSH_GAUSSIAN < 0.0), param, "sigma_brush_Gaussian(%lg) < 0",SIGMA_BRUSH_GAUSSIAN); // modify condition to your needs
	SASFIT_CHECK_COND1((fabs(POL) > 1.0), param, "|pol|(%lg) > 1",POL); // modify condition to your needs
	SASFIT_CHECK_COND1((L_B < 0.0), param, "L_b(%lg) < 0",L_B); // modify condition to your needs

	// insert your code here

	RW_SAW = 0.0;
	RADAVG = 0.0;

	R_CORE = 0.0;
	if ((R_TOT-T_SHELL) > 0) {
		R_CORE=R_TOT-T_SHELL;
		T_SH = T_SHELL;
	} else {
		R_CORE = 0.0;
		T_SH = R_TOT;
	}

	PSI = sasfit_param_override_get_psi(PSIDEG*M_PI/180.);
	return	FFmicelle_mp(q,param);
}

scalar sasfit_ff_ff_chain_saw_psi_mp_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_ff_chain_saw_psi_mp_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

