/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar egu_w_l_sphere(scalar y, sasfit_param *param) {
	scalar u,fsp,qmod,Rc;
	qmod = Q;
	Rc = sasfit_sd_egu_w_l_v(y, param, DISTRIBUTION_QUANTILE);
	u=qmod*Rc;
	if (fabs(u)<1e-6) {
		fsp = 1 - gsl_pow_2(u)/10. + gsl_pow_4(u)/280. - gsl_pow_6(u)/15120. + gsl_pow_8(u)/1.33056e6
				- gsl_pow_int(u,10)/1.729728e8 + gsl_pow_int(u,12)/3.1135104e10;
	} else {
		fsp = 3.0*(sin(u)-u*cos(u))/gsl_pow_3(u);
	}
	return N*pow(Rc,6-ALPHA)*gsl_pow_2(4./3.*M_PI* fsp);
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

scalar sasfit_ff_egu_w_l_sphere_f(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
	// insert your code here
	return 0;
}

scalar sasfit_ff_egu_w_l_sphere_v(scalar u, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
    return 0.0;
}

