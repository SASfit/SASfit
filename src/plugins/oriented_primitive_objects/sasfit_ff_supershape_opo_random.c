/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
opo_data s_shape_opod;

scalar sasfit_ff_supershape_opo_kernel_f(scalar theta, scalar phi, sasfit_param * param) {
    s_shape_opod.Q[0] = s_shape_opod.Qmod*cos(phi)*sin(theta);
    s_shape_opod.Q[1] = s_shape_opod.Qmod*sin(phi)*sin(theta);
    s_shape_opod.Q[2] = s_shape_opod.Qmod         *cos(theta);
    opo_setQhat(&s_shape_opod);
    QQX = s_shape_opod.Qhat[0];
    QQY = s_shape_opod.Qhat[1];
    QQZ = s_shape_opod.Qhat[2];
    return (ETA_P-ETA_M)*s_shape_opod.detDinv*opo_Fsupershape(param);
}
scalar sasfit_ff_supershape_opo_kernel(scalar theta, scalar phi, sasfit_param * param) {
    s_shape_opod.Q[0] = s_shape_opod.Qmod*cos(phi)*sin(theta);
    s_shape_opod.Q[1] = s_shape_opod.Qmod*sin(phi)*sin(theta);
    s_shape_opod.Q[2] = s_shape_opod.Qmod         *cos(theta);
    opo_setQhat(&s_shape_opod);
    QQX = s_shape_opod.Qhat[0];
    QQY = s_shape_opod.Qhat[1];
    QQZ = s_shape_opod.Qhat[2];
    return gsl_pow_2((ETA_P-ETA_M)*s_shape_opod.detDinv*opo_Fsupershape(param));
}

scalar sasfit_ff_supershape_opo_random(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q); // modify condition to your needs
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B < 0.0), param, "b(%lg) < 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C < 0.0), param, "c(%lg) < 0",C); // modify condition to your needs

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(s_shape_opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(s_shape_opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(s_shape_opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");
	if (A*B*C==0) return 0;

	s_shape_opod.a = A;
	s_shape_opod.b = A*B;
	s_shape_opod.c = A*C;
    s_shape_opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&s_shape_opod,ALPHA,BETA,GAMMA);
    opo_init(&s_shape_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(s_shape_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be not linear independent");

    s_shape_opod.Qmod = q;
    sasfit_param_set_polar_theta(M_PI);
    sasfit_param_set_polar_phi(2*M_PI);
    return sasfit_orient_avg(&sasfit_ff_supershape_opo_kernel,param);
}

scalar sasfit_ff_supershape_opo_random_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(s_shape_opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(s_shape_opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(s_shape_opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");
	if (A*B*C==0) return 0;

	s_shape_opod.a = A;
	s_shape_opod.b = A*B;
	s_shape_opod.c = A*C;
    s_shape_opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&s_shape_opod,ALPHA,BETA,GAMMA);
    opo_init(&s_shape_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(s_shape_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be not linear independent");

    s_shape_opod.Qmod = q;
    return sasfit_orient_avg(&sasfit_ff_supershape_opo_kernel_f,param);
}

scalar sasfit_ff_supershape_opo_random_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

