/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar fgld_profile_sphere(scalar y, sasfit_param *param) {
	scalar u,fsp,qmod,Qc, qc;
	qmod = Q;
	Qc = sasfit_sd_fgld__v(y,param,DISTRIBUTION_QUANTILE);
	qc = sasfit_sd_fgld__v(y,param,DISTRIBUTION_QUANTILE_DENS);
	u=qmod*Qc;
	if (y<YSTART) y = YSTART;
	fsp = N*(1-y)*4*M_PI*Qc*Qc*qc*gsl_sf_bessel_j0(u);
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
    scalar yend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
	// insert your code here
	Q = q*RSCALE;
	YSTART = sasfit_invert_func_v(BL,&sasfit_sd_fgld__v,DISTRIBUTION_QUANTILE,0,1,param);
	yend   = sasfit_invert_func_v(BU,&sasfit_sd_fgld__v,DISTRIBUTION_QUANTILE,0,1,param);
	//return sasfit_clipped_sinc_quad(0,yend,&fgld_profile_void,Q,(void *) param);*gsl_pow_3(RSCALE);
	return sasfit_integrate(0,yend,&fgld_profile_sphere,param)*gsl_pow_3(RSCALE);
}

scalar sasfit_ff_fgld_profile_sphere_v(scalar u, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
    return 0.0;
}

