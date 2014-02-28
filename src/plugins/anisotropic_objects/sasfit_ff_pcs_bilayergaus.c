/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

#define SIGMA_CORE fabs(param->p[0])
#define B_CORE param->p[1]
#define SIGMA_OUT fabs(param->p[2])
#define B_OUT param->p[3]
#define D	param->p[4]

scalar sasfit_ff_pcs_bilayergaus(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);

	// insert your code here

	return gsl_pow_2(sasfit_ff_pcs_bilayergaus_f(q,param));
}

scalar sasfit_ff_pcs_bilayergaus_f(scalar q, sasfit_param * param)
{
	scalar u_out, u_core, M, Pprime, R, Fout, Fcore;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

	u_core = gsl_pow_2(q*SIGMA_CORE);
	u_out  = gsl_pow_2(q*SIGMA_OUT);

	R = 0.5*D;

//	M = 2.0*sqrt(2.*M_PI)*SIGMA_OUT *B_OUT+sqrt(2.*M_PI)*SIGMA_CORE*B_CORE;
//
//	if (M == 0.0) {
//		M = 1.0;
//        sasfit_param_set_err(param, DBGINFO("BiLayerGauss is divergent"));
//	}


	Fout  = sqrt(2.*M_PI)*SIGMA_OUT *B_OUT  *exp(-0.5*u_out) *cos(q*R);
	Fcore = sqrt(2.*M_PI)*SIGMA_CORE*B_CORE *exp(-0.5*u_core);

	return Fcore+2.0*Fout;
}

scalar sasfit_ff_pcs_bilayergaus_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

