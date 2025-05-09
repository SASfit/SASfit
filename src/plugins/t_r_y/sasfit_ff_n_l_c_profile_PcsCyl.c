/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar n_l_c_profile_PcsCyl(scalar x, sasfit_param *param) {
	scalar u,fsp,qmod,y;
	qmod = Q;
	y = sasfit_sd_n_l_c__v(x,param,DISTRIBUTION_CUMULATIVE);
	u=qmod*x;
	fsp = N*(1-y)*2*M_PI*x*gsl_sf_bessel_J0(u);
	return fsp;
}

scalar sasfit_ff_n_l_c_profile_PcsCyl(scalar q, sasfit_param * param)
{
    scalar tmp;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((Q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((THETA < 0.0), param, "theta(%lg) < 0",THETA); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA < 0.0), param, "sigma(%lg) < 0",SIGMA); // modify condition to your needs
	param->p[1]=0;
    SASFIT_CHECK_COND1((BU   <= 0.0), param, "bu(%lg) <= 0",BU);

	// insert your code here
	return gsl_pow_2(sasfit_ff_n_l_c_profile_PcsCyl_f(q,param));
}

scalar sasfit_ff_n_l_c_profile_PcsCyl_f(scalar q, sasfit_param * param)
{
    SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((Q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((THETA < 0.0), param, "theta(%lg) < 0",THETA); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA < 0.0), param, "sigma(%lg) < 0",SIGMA); // modify condition to your needs
	param->p[1]=0;
    SASFIT_CHECK_COND1((BU   <= 0.0), param, "bu(%lg) <= 0",BU);

    // insert your code here
	Q = q;

	return sasfit_integrate(0,BU,&n_l_c_profile_PcsCyl,param);
}

scalar sasfit_ff_n_l_c_profile_PcsCyl_v(scalar u, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
    return 0;
}

