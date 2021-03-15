/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

opo_data pyramid4_opod;
scalar sasfit_ff_pyramid4_opo_kernel_Re_f(scalar theta, scalar phi, sasfit_param * param) {
    pyramid4_opod.Q[0] = pyramid4_opod.Qmod*cos(phi)*sin(theta);
    pyramid4_opod.Q[1] = pyramid4_opod.Qmod*sin(phi)*sin(theta);
    pyramid4_opod.Q[2] = pyramid4_opod.Qmod         *cos(theta);
    opo_setQhat(&pyramid4_opod);
    QQX = pyramid4_opod.Qhat[0];
    QQY = pyramid4_opod.Qhat[1];
    QQZ = pyramid4_opod.Qhat[2];
    return (ETA_P-ETA_M)*pyramid4_opod.detDinv*opo_Fpyramid4_Re(&pyramid4_opod);
}
scalar sasfit_ff_pyramid4_opo_kernel_Im_f(scalar theta, scalar phi, sasfit_param * param) {
    pyramid4_opod.Q[0] = pyramid4_opod.Qmod*cos(phi)*sin(theta);
    pyramid4_opod.Q[1] = pyramid4_opod.Qmod*sin(phi)*sin(theta);
    pyramid4_opod.Q[2] = pyramid4_opod.Qmod         *cos(theta);
    opo_setQhat(&pyramid4_opod);
    QQX = pyramid4_opod.Qhat[0];
    QQY = pyramid4_opod.Qhat[1];
    QQZ = pyramid4_opod.Qhat[2];
    return (ETA_P-ETA_M)*pyramid4_opod.detDinv*opo_Fpyramid4_Im(&pyramid4_opod);
}
scalar sasfit_ff_pyramid4_opo_kernel(scalar theta, scalar phi, sasfit_param * param) {
    pyramid4_opod.Q[0] = pyramid4_opod.Qmod*cos(phi)*sin(theta);
    pyramid4_opod.Q[1] = pyramid4_opod.Qmod*sin(phi)*sin(theta);
    pyramid4_opod.Q[2] = pyramid4_opod.Qmod         *cos(theta);
    opo_setQhat(&pyramid4_opod);
    QQX = pyramid4_opod.Qhat[0];
    QQY = pyramid4_opod.Qhat[1];
    QQZ = pyramid4_opod.Qhat[2];
    return gsl_pow_2((ETA_P-ETA_M)*pyramid4_opod.detDinv*opo_Fpyramid4(&pyramid4_opod));
}

scalar sasfit_ff_pyramid4_opo_random(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A <= 0.0), param, "a(%lg) <= 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B <= 0.0), param, "b(%lg) <= 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C <= 0.0), param, "c(%lg) <= 0",C); // modify condition to your needs
	SASFIT_CHECK_COND1((H_R >= tan(TILT)&&tan(TILT)>0&&!gsl_isnan(tan(TILT))), param, "H/R >= tan(TILT) (%lg)",tan(TILT));

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(pyramid4_opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(pyramid4_opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(pyramid4_opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");

	pyramid4_opod.a = A;
	pyramid4_opod.b = B;
	pyramid4_opod.c = C;
	pyramid4_opod.param = param;
    pyramid4_opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&pyramid4_opod,0,0,0);
    opo_init(&pyramid4_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(pyramid4_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be not linear independent");
    pyramid4_opod.Qmod = q;
    return sasfit_orient_avg(&sasfit_ff_pyramid4_opo_kernel,param);
}

scalar sasfit_ff_pyramid4_opo_random_f(scalar q, sasfit_param * param)
{

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(pyramid4_opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(pyramid4_opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(pyramid4_opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");

	pyramid4_opod.a = A;
	pyramid4_opod.b = B;
	pyramid4_opod.c = C;
	pyramid4_opod.param = param;
    opo_setConvention(&pyramid4_opod,yaw_pitch_roll);
 //   pyramid4_opod.Rotation.convention =Z1_X2_Y3;
    opo_setEulerAngles(&pyramid4_opod,0,0,0);
    opo_init(&pyramid4_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(pyramid4_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be linear dependent");

    pyramid4_opod.Qmod = q;
    return gsl_hypot(sasfit_orient_avg(&sasfit_ff_pyramid4_opo_kernel_Re_f,param),sasfit_orient_avg(&sasfit_ff_pyramid4_opo_kernel_Im_f,param));
}

scalar sasfit_ff_pyramid4_opo_random_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return pyramid4_opod.detDinv;
}

