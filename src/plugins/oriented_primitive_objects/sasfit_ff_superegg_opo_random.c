/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

opo_data s_egg_opod;

scalar sasfit_ff_superegg_opo_random_dr(scalar r, sasfit_param * param) {
    scalar w;
    w=pow(1-pow(r,2./EPS1),EPS1/2.);
    return 4*M_PI*r*gsl_sf_bessel_j0(QQZ*w)*w*gsl_sf_bessel_J0(gsl_hypot(QQX,QQY)*r);
}

scalar sasfit_ff_superegg_opo_kernel_f(scalar theta, scalar phi, sasfit_param * param) {
    s_egg_opod.Q[0] = s_egg_opod.Qmod*cos(phi)*sin(theta);
    s_egg_opod.Q[1] = s_egg_opod.Qmod*sin(phi)*sin(theta);
    s_egg_opod.Q[2] = s_egg_opod.Qmod         *cos(theta);
    opo_setQhat(&s_egg_opod);
    QQX = s_egg_opod.Qhat[0];
    QQY = s_egg_opod.Qhat[1];
    QQZ = s_egg_opod.Qhat[2];
    return (ETA_P-ETA_M)*s_egg_opod.detDinv*sasfit_integrate(0,1,&sasfit_ff_superegg_opo_random_dr,param);
}
scalar sasfit_ff_superegg_opo_kernel(scalar theta, scalar phi, sasfit_param * param) {
    s_egg_opod.Q[0] = s_egg_opod.Qmod*cos(phi)*sin(theta);
    s_egg_opod.Q[1] = s_egg_opod.Qmod*sin(phi)*sin(theta);
    s_egg_opod.Q[2] = s_egg_opod.Qmod         *cos(theta);
    opo_setQhat(&s_egg_opod);
    QQX = s_egg_opod.Qhat[0];
    QQY = s_egg_opod.Qhat[1];
    QQZ = s_egg_opod.Qhat[2];
    return gsl_pow_2((ETA_P-ETA_M)*s_egg_opod.detDinv*sasfit_integrate(0,1,&sasfit_ff_superegg_opo_random_dr,param));
}

scalar sasfit_ff_superegg_circ_opo_kernel_f(scalar x, sasfit_param * param) {
    s_egg_opod.Q[0] = s_egg_opod.Qmod*sqrt(fabs(1-x*x));
    s_egg_opod.Q[1] = 0;
    s_egg_opod.Q[2] = s_egg_opod.Qmod*x;
    opo_setQhat(&s_egg_opod);
    QQX = s_egg_opod.Qhat[0];
    QQY = s_egg_opod.Qhat[1];
    QQZ = s_egg_opod.Qhat[2];
    return (ETA_P-ETA_M)*s_egg_opod.detDinv*sasfit_integrate(0,1,&sasfit_ff_superegg_opo_random_dr,param);
}
scalar sasfit_ff_superegg_circ_opo_kernel(scalar x, sasfit_param * param) {
    s_egg_opod.Q[0] = s_egg_opod.Qmod*sqrt(fabs(1-x*x));
    s_egg_opod.Q[1] = 0;
    s_egg_opod.Q[2] = s_egg_opod.Qmod*x;
    opo_setQhat(&s_egg_opod);
    QQX = s_egg_opod.Qhat[0];
    QQY = s_egg_opod.Qhat[1];
    QQZ = s_egg_opod.Qhat[2];
    return gsl_pow_2((ETA_P-ETA_M)*s_egg_opod.detDinv*sasfit_integrate(0,1,&sasfit_ff_superegg_opo_random_dr,param));
}

scalar sasfit_ff_superegg_opo_random(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q); // modify condition to your needs
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B < 0.0), param, "b(%lg) < 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C < 0.0), param, "c(%lg) < 0",C); // modify condition to your needs
	SASFIT_CHECK_COND1((PP <= 0.0), param, "p(%lg) <= 0",PP); // modify condition to your needs

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(s_egg_opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(s_egg_opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(s_egg_opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");
	if (A*B*C==0) return 0;

	s_egg_opod.a = A;
	s_egg_opod.b = A*B;
	s_egg_opod.c = A*C;
	s_egg_opod.param = param;
    s_egg_opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&s_egg_opod,0,0,0);
    opo_init(&s_egg_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(s_egg_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be not linear independent");
    s_egg_opod.Qmod = q;
    if (fabs(B-1)<=sasfit_eps_get_comp() && opo_check_orthogonal(&s_egg_opod)) {
        return sasfit_integrate(0,1,sasfit_ff_superegg_circ_opo_kernel,param);
    }  else {
        return sasfit_orient_avg(&sasfit_ff_superegg_opo_kernel,param);
    }
}

scalar sasfit_ff_superegg_opo_random_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(s_egg_opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(s_egg_opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(s_egg_opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");
	if (A*B*C==0) return 0;

	s_egg_opod.a = A;
	s_egg_opod.b = A*B;
	s_egg_opod.c = A*C;
	s_egg_opod.param = param;
    s_egg_opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&s_egg_opod,ALPHA,BETA,GAMMA);
    opo_init(&s_egg_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(s_egg_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be not linear independent");

    s_egg_opod.Qmod = q;

    if (fabs(B-1)<=sasfit_eps_get_comp() && opo_check_orthogonal(&s_egg_opod)) {
        return sasfit_integrate(0,1,sasfit_ff_superegg_circ_opo_kernel_f,param);
    }  else {
        return sasfit_orient_avg(&sasfit_ff_superegg_opo_kernel_f,param);
    }
}

scalar sasfit_ff_superegg_opo_random_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
    s_egg_opod.a = A;
	s_egg_opod.b = A*B;
	s_egg_opod.c = A*C;
    s_egg_opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&s_egg_opod,ALPHA,BETA,GAMMA);
    opo_init(&s_egg_opod);
	// insert your code here
	return s_egg_opod.detDinv;
}

