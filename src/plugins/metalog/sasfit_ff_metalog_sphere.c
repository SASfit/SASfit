/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar metalog_sphere(scalar y, sasfit_param *param) {
	scalar u,fsp,qmod,Rc;
	qmod = Q;
	Rc = Mk(10 , y ,&param->p[4] , 10 , param);
	u=qmod*Rc;
	if (fabs(u)<1e-6) {
		fsp = 1 - gsl_pow_2(u)/10. + gsl_pow_4(u)/280. - gsl_pow_6(u)/15120. + gsl_pow_8(u)/1.33056e6
				- gsl_pow_int(u,10)/1.729728e8 + gsl_pow_int(u,12)/3.1135104e10;
	} else {
		fsp = 3.0*(sin(u)-u*cos(u))/gsl_pow_3(u);
	}
	return N*pow(Rc,6-ALPHA)*gsl_pow_2(4./3.*M_PI* fsp);
}
scalar sasfit_ff_metalog_sphere(scalar q, sasfit_param * param)
{
    scalar ystart,yend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((ALPHA> 6.0), param, "alpha(%lg) > 6",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND2((BL == BU), param, "bl(%lg) == bu(%lf)",BL, BU); // modify condition to your needs

	Q = q;

	ystart = sasfit_invert_func_v(BL,&sasfit_sd_metalog_clipped_v,DISTRIBUTION_QUANTILE,0,1,param);
	yend   = sasfit_invert_func_v(BU,&sasfit_sd_metalog_clipped_v,DISTRIBUTION_QUANTILE,0,1,param);
	return sasfit_integrate(ystart,yend,&metalog_sphere,param);
}

scalar sasfit_ff_metalog_sphere_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_metalog_sphere_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

