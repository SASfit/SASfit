/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar w_l_e_profile_PcsPlanar(scalar x, sasfit_param *param) {
	scalar u,fsp,qmod,y;
	qmod = Q;
	y = sasfit_sd_w_l_e__v(x,param,DISTRIBUTION_CUMULATIVE);
	u=qmod*x;
	fsp = N*(1-y)*2*cos(u);
	return fsp;
}

scalar sasfit_ff_w_l_e_profile_PcsPlanar(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((THETA < 0.0), param, "theta(%lg) < 0",THETA); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA < 0.0), param, "alpha(%lg) < 0",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA < 0.0), param, "beta(%lg) < 0",BETA); // modify condition to your needs
	param->p[1]=0;
    SASFIT_CHECK_COND1((BU   <= 0.0), param, "bu(%lg) <= 0",BU);
	// insert your code here

	return gsl_pow_2(sasfit_ff_w_l_e_profile_PcsPlanar_f(q,param));
}

scalar sasfit_ff_w_l_e_profile_PcsPlanar_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
	param->p[1]=0;
    SASFIT_CHECK_COND1((BU   <= 0.0), param, "bu(%lg) <= 0",BU);

	// insert your code here
	if (RSCALE==0) return 0;
	Q = q*RSCALE;

	return sasfit_integrate(0,BU,&w_l_e_profile_PcsPlanar,param);
}

scalar sasfit_ff_w_l_e_profile_PcsPlanar_v(scalar u, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

    return 0.0;
}

