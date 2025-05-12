/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar w_l_e_sphere(scalar y, sasfit_param *param) {
	scalar fsp,Rc;
	Rc = sasfit_sd_w_l_e__v(y, param, DISTRIBUTION_QUANTILE);
	fsp=sasfit_sphere_fc(Q,Rc);
	return N*pow(Rc,6-DIM)*gsl_pow_2(4./3.*M_PI* fsp);
}

scalar sasfit_ff_w_l_e_sphere(scalar q, sasfit_param * param)
{
    scalar ystart, yend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((THETA < 0.0), param, "theta(%lg) < 0",THETA); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA < 0.0), param, "alpha(%lg) < 0",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA < 0.0), param, "beta(%lg) < 0",BETA); // modify condition to your needs

	// insert your code here

	Q = q;

	ystart = sasfit_invert_func_v(BL,&sasfit_sd_w_l_e__v,DISTRIBUTION_QUANTILE,0,1,param);
	yend   = sasfit_invert_func_v(BU,&sasfit_sd_n_l_c__v,DISTRIBUTION_QUANTILE,0,1,param);
	return sasfit_integrate(ystart,yend,&w_l_e_sphere,param);
}

scalar sasfit_ff_w_l_e_sphere_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0;
}

scalar sasfit_ff_w_l_e_sphere_v(scalar u, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

    return 0.0;
}

