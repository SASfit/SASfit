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
#define SIGMA_D param->p[5]

scalar bilayergauss(scalar nu,sasfit_param * param)
{
	scalar u_out, u_core, M, Pprime, R, Fout, Fcore;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

	u_core = gsl_pow_2(Q*SIGMA_CORE);
	u_out  = gsl_pow_2(Q*SIGMA_OUT);

	R = 0.5*D*nu;

	Fout  = sqrt(2.*M_PI)*SIGMA_OUT *B_OUT  *exp(-0.5*u_out) *cos(Q*R);
	Fcore = sqrt(2.*M_PI)*SIGMA_CORE*B_CORE *exp(-0.5*u_core);

	return gsl_pow_int(Fcore+2.0*Fout,lround(P));
}

scalar LogNormBiLayerGauss(scalar nu,sasfit_param * param)
{
    sasfit_param subParam;
    subParam.p[0] = 1.0;
    subParam.p[1] = SIGMA_D;
	subParam.p[2] = 1.0;
	subParam.p[3] = 1.0;
	return sasfit_sd_LogNorm(nu, &subParam)*bilayergauss(nu,param);
}
scalar sasfit_ff_pcs_bilayergauss(scalar q, sasfit_param * param)
{
    scalar nuStart, nuEnd;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);

	// insert your code here
    P=2;
    Q=q;
    if (SIGMA_D==0) {
        return bilayergauss(1,param);
    } else {
        find_LogNorm_int_range(2,1,SIGMA_D,&nuStart,&nuEnd,param);
        return sasfit_integrate(nuStart,nuEnd,&LogNormBiLayerGauss,param);
    }
	return gsl_pow_2(sasfit_ff_pcs_bilayergauss_f(q,param));
}

scalar sasfit_ff_pcs_bilayergauss_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_pcs_bilayergauss_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

