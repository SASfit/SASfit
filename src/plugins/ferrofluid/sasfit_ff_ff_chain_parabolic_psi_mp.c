/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

scalar sasfit_ff_ff_chain_parabolic_psi_mp(scalar q, sasfit_param * param)
{
	scalar nn;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R_CORE < 0.0), param, "R_core(%lg) < 0",R_CORE); // modify condition to your needs
	SASFIT_CHECK_COND1((SNAGG < 0.0), param, "nagg(%lg) < 0",SNAGG); // modify condition to your needs
	SASFIT_CHECK_COND1((VBRUSH < 0.0), param, "Vbrush(%lg) < 0",VBRUSH); // modify condition to your needs
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((fabs(POL) > 1.0), param, "|pol|(%lg) > 1",POL); // modify condition to your needs

	// insert your code here

	RW_SAW = 2.0;

	RADAVG = 0.0;
	PEP    = 0.0;

	if ((R_TOT-T_SHELL) > 0) {
		R_CORE=R_TOT-T_SHELL;
		T_SH = T_SHELL;
	} else {
		R_CORE = 0.0;
		T_SH = R_TOT;
	}

	if (R_TOT == 0.0) return 0.0;

	ALPHA_SH = 2.0/(3*M_PI);
	nn = 4*M_PI*gsl_pow_2(R_TOT)/SIGMA;
	I = nn*N*gsl_pow_3(A)/(4*M_PI);

	nn = SNAGG*4*M_PI*gsl_pow_2(R_TOT);
	I = VBRUSH * nn /(4*M_PI);
	C=ALPHA_SH/(P*gsl_pow_2(A*N));

//	sasfit_out("q:%lf\n",q);
	LAMBDA = find_LAMBDA(param);
//	sasfit_out("LAMBDA: %f   zmax: %f\n",LAMBDA,sqrt(LAMBDA/C));

	PSI = sasfit_param_override_get_psi(PSIDEG*M_PI/180.);
	return	FFmicelle_mp(q,param);

}

scalar sasfit_ff_ff_chain_parabolic_psi_mp_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_ff_chain_parabolic_psi_mp_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return V(R_CORE+T_SH+2.*sqrt(5./3.)*RG,param);
}

