/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define NAGG	param->p[0]
#define V_CORE	param->p[1]
#define V_BRUSH	param->p[2]
#define ETA_CORE	param->p[3]
#define ETA_BRUSH	param->p[4]
#define ETA_SOLV	param->p[5]
#define XSOLV_CORE	param->p[6]
#define RG	param->p[7]
#define D	param->p[8]
#define H	param->p[9]

scalar sasfit_ff_cyl_chains_rw__nagg(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((NAGG < 0.0), param, "Nagg(%lg) < 0",NAGG); // modify condition to your needs
	SASFIT_CHECK_COND1((V_CORE < 0.0), param, "V_core(%lg) < 0",V_CORE); // modify condition to your needs
	SASFIT_CHECK_COND1((V_BRUSH < 0.0), param, "V_brush(%lg) < 0",V_BRUSH); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA_CORE < 0.0), param, "eta_core(%lg) < 0",ETA_CORE); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA_BRUSH < 0.0), param, "eta_brush(%lg) < 0",ETA_BRUSH); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA_SOLV < 0.0), param, "eta_solv(%lg) < 0",ETA_SOLV); // modify condition to your needs
	SASFIT_CHECK_COND1((XSOLV_CORE < 0.0), param, "xsolv_core(%lg) < 0",XSOLV_CORE); // modify condition to your needs
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((D < 0.0), param, "d(%lg) < 0",D); // modify condition to your needs
	SASFIT_CHECK_COND1((H < 0.0), param, "H(%lg) < 0",H); // modify condition to your needs

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_cyl_chains_rw__nagg_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_cyl_chains_rw__nagg_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

