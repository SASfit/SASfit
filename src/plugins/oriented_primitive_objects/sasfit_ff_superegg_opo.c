/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

scalar sasfit_ff_superegg_opo_dr(scalar r, sasfit_param * param) {
    scalar w;
    w=pow(1-pow(r,2./EPS1),EPS1/2.);
    return 4*M_PI*r*gsl_sf_bessel_j0(QQZ*w)*w*gsl_sf_bessel_J0(gsl_hypot(QQX,QQY)*r);
}

scalar sasfit_ff_superegg_opo(scalar q, sasfit_param * param)
{
	opo_data opod;
    scalar psi;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B < 0.0), param, "b(%lg) < 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C < 0.0), param, "c(%lg) < 0",C); // modify condition to your needs
	SASFIT_CHECK_COND1((EPS1 <= 0.0), param, "eps1(%lg) <= 0",EPS1); // modify condition to your needs

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");
	if (A*B*C==0) return 0;

	opod.a = A;
	opod.b = A*B;
	opod.c = A*C;
    opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&opod,ALPHA,BETA,GAMMA);
    opo_init(&opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be not linear independent");

    opod.Qmod = q;
    psi=sasfit_param_override_get_psi(PSI_DEG*M_PI/180.);
    opod.Q[0] = q*cos(psi);
    opod.Q[1] = q*sin(psi);
    opod.Q[2] = 0;
	opod.param=param;
    opo_setQhat(&opod);
    QQX = opod.Qhat[0];
    QQY = opod.Qhat[1];
    QQZ = opod.Qhat[2];
	return gsl_pow_2((ETA_P-ETA_M)*opod.detDinv*sasfit_integrate(0,1,&sasfit_ff_superegg_opo_dr,param));
}

scalar sasfit_ff_superegg_opo_f(scalar q, sasfit_param * param)
{
	opo_data opod;
    scalar psi;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");
	if (A*B*C==0) return 0;

	opod.a = A;
	opod.b = A*B;
	opod.c = A*C;
    opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&opod,ALPHA,BETA,GAMMA);
    opo_init(&opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be not linear independent");

    opod.Qmod = q;
    psi=sasfit_param_override_get_psi(PSI_DEG*M_PI/180.);
    opod.Q[0] = q*cos(psi);
    opod.Q[1] = q*sin(psi);
    opod.Q[2] = 0;
	opod.param=param;
    opo_setQhat(&opod);
    QQX = opod.Qhat[0];
    QQY = opod.Qhat[1];
    QQZ = opod.Qhat[2];
	return (ETA_P-ETA_M)*opod.detDinv*sasfit_integrate(0,1,&sasfit_ff_superegg_opo_dr,param);
}

scalar sasfit_ff_superegg_opo_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

