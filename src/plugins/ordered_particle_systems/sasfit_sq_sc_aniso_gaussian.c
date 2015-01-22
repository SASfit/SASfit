/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables

scalar sasfit_sq_sc_aniso_gaussian(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((PAD < 0.0), param, "a(%lg) < 0",PAD); // modify condition to your needs

	// insert your code here
	PEAKSHAPE = GAUSSIANPEAK;
	ORDERTYPE = SC;

	QMODVAL     = q;
	ADVAL       = PAD;
	DELTAQVAL   = PDELTAQ;
	DELTAPSIVAL = PDELTAPSI;
	LAMBDAVAL   = PLAMBDA;
	ALPHAVAL    = PROLL*M_PI/180.;
	BETAVAL     = PPITCH*M_PI/180.;
	GAMMAVAL    = PYAW*M_PI/180.;
	VARPHIVAL   = sasfit_param_override_get_psi(PSI_DEG*M_PI/180.);
	VARTHETAVAL = 2*asin(QMODVAL*LAMBDAVAL/(4*M_PI));
	MAXHKLVAL   = PMAXHKL;

	set_ki(&ospParameter,param);
	set_ks(&ospParameter,param);
	set_q(&ospParameter,param);
	set_R_Nautic(&ospParameter,param);
	init_osp(&ospParameter,param);

 	return (Lattice_Factor_aniso(&ospParameter,param)-1.0)*G(&ospParameter,param)+1.0;
}

scalar sasfit_sq_sc_aniso_gaussian_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_sc_aniso_gaussian_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

	return 0.0;
}

