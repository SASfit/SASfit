/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar metalogLog_sphere(scalar y, sasfit_param *param) {
    metalog_param *mpara;
	scalar u,fsp,qmod,Rc;
    mpara = (metalog_param *) param->moreparam;
	qmod = Q;
	Rc = MLog(10 , y ,mpara->a , 10 , param);
	u=qmod*Rc;
	if (fabs(u)<1e-6) {
		fsp = 1 - gsl_pow_2(u)/10. + gsl_pow_4(u)/280. - gsl_pow_6(u)/15120. + gsl_pow_8(u)/1.33056e6
				- gsl_pow_int(u,10)/1.729728e8 + gsl_pow_int(u,12)/3.1135104e10;
	} else {
		fsp = 3.0*(sin(u)-u*cos(u))/gsl_pow_3(u);
	}
	return N*pow(Rc,6-ALPHA)*gsl_pow_2(4./3.*M_PI* fsp);
}
scalar sasfit_ff_metaloglog_sphere(scalar q, sasfit_param * param)
{
    metalog_param mp;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((ALPHA > 6.0), param, "alpha(%lg) > 6",ALPHA); // modify condition to your needs

	Q = q;
    param->moreparam=&mp;
    assign_metalog_par(0, &mp,param);
	return sasfit_integrate(0,1,&metalogLog_sphere,param);
}

scalar sasfit_ff_metaloglog_sphere_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_metaloglog_sphere_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

