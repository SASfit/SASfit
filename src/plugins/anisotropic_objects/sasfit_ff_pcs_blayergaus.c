/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define D	param->p[0]
#define W_D param->p[1]
#define SIGMA_CORE param->p[2]
#define B_CORE param->p[3]
#define SIGMA_OUT param->p[4]
#define B_OUT param->p[5]

scalar sasfit_ff_pcs_blayergaus(scalar q, sasfit_param * param)
{
	scalar u_out, u_core, M, Pprime, R, Fout, Fcore;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((D < 0.0), param, "d(%lg) < 0",D); // modify condition to your needs

	// insert your code here	u_out  = q*q*sigma_out *sigma_out ;
	
	u_core = q*q*SIGMA_CORE*SIGMA_CORE;

	R = 0.5*D;

	M = 2.0*sqrt(2.*M_PI)*SIGMA_OUT *B_OUT+sqrt(2.*M_PI)*SIGMA_CORE*B_CORE;


	if (M == 0.0) 
	{
		M = 1.0;
                sasfit_param_set_err(param, DBGINFO("BiLayerGauss is divergent"));
	}


	Fout  = sqrt(2.*M_PI)*SIGMA_OUT *B_OUT  *exp(-0.5*u_out) *cos(q*R);
	Fcore = sqrt(2.*M_PI)*SIGMA_CORE*B_CORE *exp(-0.5*u_core);

	return gsl_pow_2(Fcore+2.0*Fout);
}

scalar sasfit_ff_pcs_blayergaus_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_pcs_blayergaus_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

