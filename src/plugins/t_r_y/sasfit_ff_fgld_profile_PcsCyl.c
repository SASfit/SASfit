/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar fgld_profile_PcsCyl(scalar y, sasfit_param *param) {
	scalar u,fsp,Qc, qc, yused;
	if (y<YSTART) {
        yused = YSTART;
	} else {
	    yused = y;
	}
	Qc = sasfit_sd_fgld__v(yused,param,DISTRIBUTION_QUANTILE);
	qc = sasfit_sd_fgld__v(yused,param,DISTRIBUTION_QUANTILE_DENS);
	u=Q*Qc;
	fsp = N*(1-yused)*2*M_PI*Qc*qc*gsl_sf_bessel_J0(u);
	return fsp;
}

scalar sasfit_ff_fgld_profile_PcsCyl(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((KAPPA < 0.0), param, "kappa(%lg) < 0",KAPPA); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA <= 0.0), param, "beta(%lg) <= 0",BETA); // modify condition to your needs
    SASFIT_CHECK_COND1((BU   <= 0.0), param, "bu(%lg) <= 0",BU);
    SASFIT_CHECK_COND1((BL     < 0.0), param, "bl(%lg) < 0",BL);
    SASFIT_CHECK_COND1((RSCALE < 0.0), param, "r(%lg) < 0",RSCALE);

	// insert your code here
	return gsl_pow_2(sasfit_ff_fgld_profile_PcsCyl_f(q,param));
}

scalar sasfit_ff_fgld_profile_PcsCyl_f(scalar q, sasfit_param * param)
{
    scalar yend, Pcscyl0start;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
	// insert your code here
	if (RSCALE==0) return 0;
	Q = q*RSCALE;
 	YSTART = sasfit_invert_func_v(BL,&sasfit_sd_fgld__v,DISTRIBUTION_QUANTILE,0,1,param);
	yend   = sasfit_invert_func_v(BU,&sasfit_sd_fgld__v,DISTRIBUTION_QUANTILE,0,1,param);
	Pcscyl0start = N*(1-YSTART)*2*M_PI*gsl_pow_2(BL*RSCALE)*sasfit_jinc(Q*BL);
	return sasfit_integrate(YSTART,yend,&fgld_profile_PcsCyl,param)*RSCALE*RSCALE+Pcscyl0start;
}

scalar sasfit_ff_fgld_profile_PcsCyl_v(scalar u, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
    return 0.0;
}

