/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar fgld_profile_PcsCyl(scalar y, sasfit_param *param) {
	scalar u,fsp,qmod,Qc, qc;
	qmod = Q;
	Qc = sasfit_sd_fgld__v(y,param,DISTRIBUTION_QUANTILE);
	qc = sasfit_sd_fgld__v(y,param,DISTRIBUTION_QUANTILE_DENS);
	// u=qmod*Qc;
	u=qmod*(Qc+BL);
	// fsp = N*(1-y)*2*M_PI*Qc*qc*gsl_sf_bessel_J0(u);
	fsp = N*(1-y)*2*M_PI*(Qc+BL)*qc*gsl_sf_bessel_J0(u);
	return fsp;
}

scalar sasfit_ff_fgld_profile_PcsCyl(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((KAPPA < 0.0), param, "kappa(%lg) < 0",KAPPA); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA <= 0.0), param, "beta(%lg) <= 0",BETA); // modify condition to your needs
    SASFIT_CHECK_COND1((BU   <= 0.0), param, "bu(%lg) <= 0",BU);

	// insert your code here
	return gsl_pow_2(sasfit_ff_fgld_profile_PcsCyl_f(q,param));
}

scalar sasfit_ff_fgld_profile_PcsCyl_f(scalar q, sasfit_param * param)
{
    scalar ystart, yend, Pcylystart;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
	// insert your code here
	if (RSCALE==0) return 0;
	Q = q*RSCALE;
	param->p[1]=gsl_max(0,BL);
	// param->p[1]=0;
//	ystart = sasfit_invert_func_v(BL,&sasfit_sd_fgld__v,DISTRIBUTION_QUANTILE,0,1,param);
	ystart = sasfit_invert_func_v(0,&sasfit_sd_fgld__v,DISTRIBUTION_QUANTILE,0,1,param);
	Pcylystart = N*(1-ystart)*M_PI*BL*BL * 2*sasfit_jinc(Q*BL);
	yend   = sasfit_invert_func_v(BU,&sasfit_sd_fgld__v,DISTRIBUTION_QUANTILE,0,1,param);
	return (sasfit_integrate(ystart,yend,&fgld_profile_PcsCyl,param)+Pcylystart)*RSCALE*RSCALE;
}

scalar sasfit_ff_fgld_profile_PcsCyl_v(scalar u, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
    return 0.0;
}

