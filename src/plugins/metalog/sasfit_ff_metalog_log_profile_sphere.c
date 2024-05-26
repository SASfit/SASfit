/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar metalog_log_profile_sphere(scalar y, sasfit_param *param) {
    metalog_param *mpara;
	scalar u,fsp,qmod,Qc, qc;
    mpara = (metalog_param *) param->moreparam;
	qmod = Q;
	Qc = MLog(10 , y ,mpara->a , 10 , param);
	qc = mLog(10 , y ,mpara->a , 10 , param);
	u=qmod*Qc;
	fsp = N*(1-y)*4*M_PI*Qc*Qc*qc*gsl_sf_bessel_j0(u);
	return fsp;
}
scalar sasfit_ff_metalog_log_profile_sphere(scalar q, sasfit_param * param)
{
    return gsl_pow_2(sasfit_ff_metalog_log_profile_sphere_f(q,param));
}

scalar sasfit_ff_metalog_log_profile_sphere_f(scalar q, sasfit_param * param)
{
	metalog_param mp;
    gsl_function F;
    scalar ystart,yend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	Q = q;
	param->p[1]=0;

    F.function = &root_metalog_Log_f;
    F.params=param;
    param->moreparam=&mp;
    assign_metalog_par(BU, &mp,param);
	yend = find_root_f_metalog(&F);

	return sasfit_integrate(0,yend,&metalog_log_profile_sphere,param);
}

scalar sasfit_ff_metalog_log_profile_sphere_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

