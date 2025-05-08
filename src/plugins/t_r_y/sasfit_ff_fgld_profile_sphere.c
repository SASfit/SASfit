/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar fgld_profile_sphere(scalar y, sasfit_param *param) {
	scalar u,fsp,qmod,Qc, qc, yused;
	if (y<YSTART) {
        yused = YSTART;
        return N;
	} else {
	    yused = y;
	}
	Qc = sasfit_sd_fgld__v(y,param,DISTRIBUTION_QUANTILE);
	qc = sasfit_sd_fgld__v(y,param,DISTRIBUTION_QUANTILE_DENS);
	u=Q*Qc;
	fsp = N*(1-yused)*4*M_PI*Qc*Qc*qc*gsl_sf_bessel_j0(u);
	return fsp;
}

scalar fgld_profile_void(scalar y, void *para) {
	return sasfit_ff_fgld_profile(y,(sasfit_param *) para);
}

scalar sasfit_ff_fgld_profile_sphere(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((KAPPA < 0.0), param, "kappa(%lg) < 0",KAPPA); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA <= 0.0), param, "beta(%lg) <= 0",BETA); // modify condition to your needs
    SASFIT_CHECK_COND1((BU   <= 0.0), param, "bu(%lg) <= 0",BU);

	// insert your code here
	return gsl_pow_2(sasfit_ff_fgld_profile_sphere_f(q,param));
}

scalar sasfit_ff_fgld_profile_sphere_f(scalar q, sasfit_param * param)
{
    scalar yend,Pcsshp0start;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
	// insert your code here
	if (RSCALE==0) return 0;
	Q = q*RSCALE;
	Pcsshp0start = N*(1-YSTART)*2*M_PI*gsl_pow_2(BL*RSCALE)*sasfit_jinc(Q*BL);
	YSTART = sasfit_invert_func_v(BL,&sasfit_sd_fgld__v,DISTRIBUTION_QUANTILE,0,1,param);
	yend   = sasfit_invert_func_v(BU,&sasfit_sd_fgld__v,DISTRIBUTION_QUANTILE,0,1,param);
	//return sasfit_clipped_sinc_quad(0,yend,&fgld_profile_void,Q,(void *) param);*gsl_pow_3(RSCALE);
	Pcsshp0start= N*(1-YSTART)*sasfit_sphere_fc(q,BL*RSCALE)*4./3.*M_PI*gsl_pow_2(BL*RSCALE);
	return sasfit_integrate(YSTART,yend,&fgld_profile_sphere,param)*gsl_pow_3(RSCALE)+Pcsshp0start;
}

scalar sasfit_ff_fgld_profile_sphere_v(scalar u, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
    return 0.0;
}

