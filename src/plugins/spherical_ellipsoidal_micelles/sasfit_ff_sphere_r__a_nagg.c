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
#define ETA_SOLVC	param->p[5]
#define ALPHA	param->p[6]
#define T	param->p[7]
#define L	param->p[8]
#define LB	param->p[9]

scalar sasfit_ff_sphere_r__a_nagg(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((NAGG < 0.0), param, "N_agg(%lg) < 0",NAGG); // modify condition to your needs
	SASFIT_CHECK_COND1((V_CORE < 0.0), param, "V_core(%lg) < 0",V_CORE); // modify condition to your needs
	SASFIT_CHECK_COND1((V_BRUSH < 0.0), param, "V_brush(%lg) < 0",V_BRUSH); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA < 0.0), param, "alpha(%lg) < 0",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs
	SASFIT_CHECK_COND1((L <= 0.0), param, "L(%lg) <= 0",L); // modify condition to your needs
	SASFIT_CHECK_COND1((LB < 0.0), param, "b(%lg) < 0",LB); // modify condition to your needs

	// insert your code here

	param->kernelSelector = SPHERE_RMA_NAGG;
	return sasfit_ff_Sphere_R_ma_Profile(q,param);
}

scalar sasfit_ff_sphere_r__a_nagg_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_sphere_r__a_nagg_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

