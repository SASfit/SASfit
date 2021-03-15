/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

#define R_CORE	param->p[0]
#define N_AGG	param->p[1]
#define V_BRUSH	param->p[2]
#define ETA_CORE	param->p[3]
#define ETA_BRUSH	param->p[4]
#define ETA_SOLVC	param->p[5]
#define XSOLV_CORE	param->p[6]
#define RG	param->p[7]
#define D	param->p[8]
#define DUMMY	param->p[9]

scalar sasfit_ff_sphere_chains_rw_(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R_CORE < 0.0), param, "R_core(%lg) < 0",R_CORE); // modify condition to your needs
	SASFIT_CHECK_COND1((N_AGG <= 0.0), param, "N_AGG(%lg) <= 0",N_AGG); // modify condition to your needs
	SASFIT_CHECK_COND1((V_BRUSH < 0.0), param, "V_brush(%lg) < 0",V_BRUSH); // modify condition to your needs
	SASFIT_CHECK_COND1((XSOLV_CORE < 0.0), param, "xsolv_core(%lg) < 0",XSOLV_CORE); // modify condition to your needs
	SASFIT_CHECK_COND1((XSOLV_CORE >= 1.0), param, "xsolv_core(%lg) >= 1",XSOLV_CORE); // modify condition to your needs
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((D < 0.0), param, "d(%lg) < 0",D); // modify condition to your needs

	// insert your code here

	param->kernelSelector = SPHERE_RWBRUSH;
	return sasfit_ff_Sphere_RWbrush(q,param);
}

scalar sasfit_ff_sphere_chains_rw__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_sphere_chains_rw__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

