/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RG_P	param->p[0]
#define RG_E	param->p[1]
#define NU	param->p[2]
#define I0	param->p[3]
#define Q	param->p[MAXPAR-1]

scalar sroggc_core(scalar mu,sasfit_param * param) 
{
	scalar RG_av;
	sasfit_param subParam;
	RG_av = sqrt(gsl_pow_2(RG_P*mu)+ gsl_pow_2(RG_E)*(1-mu*mu));
	subParam.p[0] = RG_av;
	subParam.p[1] = NU;		
	subParam.p[2] = I0;
	return sasfit_ff_generalized_gaussian_coil(Q,&subParam);
}



scalar sasfit_ff_stretched_random_oriented_generalized_gaussian_coil(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG_P < 0.0), param, "Rg_p(%lg) < 0",RG_P); // modify condition to your needs
	SASFIT_CHECK_COND1((RG_E < 0.0), param, "Rg_e(%lg) < 0",RG_E); // modify condition to your needs
	SASFIT_CHECK_COND1((NU < 0.0), param, "nu(%lg) < 0",NU); // modify condition to your needs
	SASFIT_CHECK_COND1((I0 < 0.0), param, "I0(%lg) < 0",I0); // modify condition to your needs

	// insert your code here
	Q=q;
	return sasfit_integrate(0,1,&sroggc_core,param);
}

scalar sasfit_ff_stretched_random_oriented_generalized_gaussian_coil_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_stretched_random_oriented_generalized_gaussian_coil_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

