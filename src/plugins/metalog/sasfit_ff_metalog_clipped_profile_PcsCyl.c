/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar metalog_profile_PcsCyl(scalar y, sasfit_param *param) {
	scalar u,fsp,qmod,Qc, qc;
    qmod = Q;
	Qc = Mk(10 , y ,&param->p[4] , 10, param);
	qc = mk(10 , y ,&param->p[4] , 10, param);
	u=qmod*Qc;
	fsp = N*(1-y)*2*M_PI*Qc*qc*gsl_sf_bessel_J0(u);
	return fsp;
}
scalar sasfit_ff_metalog_clipped_profile_PcsCyl(scalar q, sasfit_param * param)
{
    return gsl_pow_2(sasfit_ff_metalog_clipped_profile_PcsCyl_f(q,param));
}

scalar sasfit_ff_metalog_clipped_profile_PcsCyl_f(scalar q, sasfit_param * param)
{
    scalar ystart,yend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	Q = q;
	param->p[1]=0;
	param->p[3]=0;
	ystart = sasfit_invert_func_v(BL,&sasfit_sd_metalog_clipped_v,DISTRIBUTION_QUANTILE,0,1,param);
	yend   = sasfit_invert_func_v(BU,&sasfit_sd_metalog_clipped_v,DISTRIBUTION_QUANTILE,0,1,param);

	return sasfit_integrate(ystart,yend,&metalog_profile_PcsCyl,param);
}

scalar sasfit_ff_metalog_clipped_profile_PcsCyl_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

