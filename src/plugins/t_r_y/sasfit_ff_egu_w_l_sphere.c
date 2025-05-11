/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar egu_w_l_sphere(scalar y, sasfit_param *param) {
	scalar fsp,Rc;
	Rc = sasfit_sd_egu_w_l_v(y, param, DISTRIBUTION_QUANTILE);
	fsp=sasfit_sphere_fc(Q,Rc);
	return N*pow(Rc,6-DIM)*gsl_pow_2(4./3.*M_PI* fsp);
}

scalar sasfit_ff_egu_w_l_sphere(scalar q, sasfit_param * param)
{
    scalar ystart, yend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((DIM > 6.0), param, "dim(%lg) > 6",DIM); // modify condition to your needs
	SASFIT_CHECK_COND1((DIM < 0.0), param, "dim(%lg) < 0",DIM); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA <= 0.0), param, "alpha(%lg) <= 0",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA <= 0.0), param, "beta(%lg) <= 0",BETA); // modify condition to your needs
	SASFIT_CHECK_COND1((GAMMA <= 0.0), param, "c(%lg) <= 0",GAMMA); // modify condition to your needs


    param->p[1]=0;
    SASFIT_CHECK_COND1((BU   <= 0.0), param, "bu(%lg) <= 0",BU);

	// insert your code here

	Q = q;
	ystart = 0;
	yend   = sasfit_sd_egu_w_l_v(BU,param,DISTRIBUTION_CUMULATIVE);
	return sasfit_integrate(ystart,yend,&egu_w_l_sphere,param);
}

scalar sasfit_ff_egu_w_l_sphere_f(scalar q, sasfit_param * param)
{
    SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	return 0;
}

scalar sasfit_ff_egu_w_l_sphere_v(scalar u, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
    return 0;
}

