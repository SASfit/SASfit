/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar metalog_sphere(scalar y, sasfit_param *param) {
    metalog_param *mpara;
	scalar u,fsp,qmod,Rc;
    mpara = (metalog_param *) param->moreparam;
	qmod = Q;
	Rc = Mk(10 , y ,mpara->a , 10 , param);
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
    metalog_param mp;
    gsl_function F;
    scalar ystart,yend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((ALPHA> 6.0), param, "alpha(%lg) > 6",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((N < 0.0), param, "N(%lg) < 0",N); // modify condition to your needs
	SASFIT_CHECK_COND1((BL < 0.0), param, "bl(%lg) < 0",BL); // modify condition to your needs
	SASFIT_CHECK_COND1((BU < 0.0), param, "bu(%lg) < 0",BU); // modify condition to your needs
	SASFIT_CHECK_COND2((BL == BU), param, "bl(%lg) == bu(%lf)",BL, BU); // modify condition to your needs

	Q = q;

    mp.ytrans=&ylin;
	mp.dytrans=&dylin;
    F.function = &root_metalog_f;
    F.params=param;
    param->moreparam=&mp;
    assign_metalog_par(BL, &mp,param);
	ystart = find_root_brent_metalog(&F);
	mp.x = BU;
	yend = find_root_brent_metalog(&F);
	sasfit_out("ystart=%lf, yend=%lf\n",ystart,yend);
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

