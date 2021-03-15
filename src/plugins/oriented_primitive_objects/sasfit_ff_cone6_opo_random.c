/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>
opo_data cone6_opod;
scalar sasfit_ff_cone6_opo_kernel_Re_f(scalar theta, scalar phi, sasfit_param * param) {
    cone6_opod.Q[0] = cone6_opod.Qmod*cos(phi)*sin(theta);
    cone6_opod.Q[1] = cone6_opod.Qmod*sin(phi)*sin(theta);
    cone6_opod.Q[2] = cone6_opod.Qmod         *cos(theta);
    opo_setQhat(&cone6_opod);
    QQX = cone6_opod.Qhat[0];
    QQY = cone6_opod.Qhat[1];
    QQZ = cone6_opod.Qhat[2];
    return (ETA_P-ETA_M)*cone6_opod.detDinv*sasfit_integrate(0,H_R,&Fcone6_kernel_Re,param);
}
scalar sasfit_ff_cone6_opo_kernel_Im_f(scalar theta, scalar phi, sasfit_param * param) {
    cone6_opod.Q[0] = cone6_opod.Qmod*cos(phi)*sin(theta);
    cone6_opod.Q[1] = cone6_opod.Qmod*sin(phi)*sin(theta);
    cone6_opod.Q[2] = cone6_opod.Qmod         *cos(theta);
    opo_setQhat(&cone6_opod);
    QQX = cone6_opod.Qhat[0];
    QQY = cone6_opod.Qhat[1];
    QQZ = cone6_opod.Qhat[2];
    return (ETA_P-ETA_M)*cone6_opod.detDinv*sasfit_integrate(0,H_R,&Fcone6_kernel_Im,param);
}
scalar sasfit_ff_cone6_opo_kernel(scalar theta, scalar phi, sasfit_param * param) {
    cone6_opod.Q[0] = cone6_opod.Qmod*cos(phi)*sin(theta);
    cone6_opod.Q[1] = cone6_opod.Qmod*sin(phi)*sin(theta);
    cone6_opod.Q[2] = cone6_opod.Qmod         *cos(theta);
    opo_setQhat(&cone6_opod);
    QQX = cone6_opod.Qhat[0];
    QQY = cone6_opod.Qhat[1];
    QQZ = cone6_opod.Qhat[2];
    return gsl_pow_2((ETA_P-ETA_M)*cone6_opod.detDinv*opo_Fcone6(&cone6_opod));
}

scalar sasfit_ff_cone6_opo_random(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B < 0.0), param, "b(%lg) < 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C < 0.0), param, "c(%lg) < 0",C); // modify condition to your needs
	SASFIT_CHECK_COND1((H_R > tan(TILT)&&tan(TILT)>0&&!gsl_isnan(tan(TILT))), param, "H/R > tan(TILT) (%lg)",tan(TILT));

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(cone6_opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(cone6_opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(cone6_opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");

    if (A*B*C*H_R == 0.0) return 0;

	cone6_opod.a = A;
	cone6_opod.b = B;
	cone6_opod.c = C;
	cone6_opod.param = param;
    cone6_opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&cone6_opod,0,0,0);
    opo_init(&cone6_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(cone6_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be not linear independent");
    cone6_opod.Qmod = q;
    return sasfit_orient_avg(&sasfit_ff_cone6_opo_kernel,param);
}

scalar sasfit_ff_cone6_opo_random_f(scalar q, sasfit_param * param)
{

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(cone6_opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(cone6_opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(cone6_opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");

    if (A*B*C*H_R == 0.0) return 0;

	cone6_opod.a = A;
	cone6_opod.b = B;
	cone6_opod.c = C;
	cone6_opod.param = param;
    opo_setConvention(&cone6_opod,yaw_pitch_roll);
    opo_setEulerAngles(&cone6_opod,0,0,0);
    opo_init(&cone6_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(cone6_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be linear dependent");

    cone6_opod.Qmod = q;
    return gsl_hypot(sasfit_orient_avg(&sasfit_ff_cone6_opo_kernel_Re_f,param),sasfit_orient_avg(&sasfit_ff_cone6_opo_kernel_Im_f,param));
}

scalar sasfit_ff_cone6_opo_random_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return cone6_opod.detDinv;
}

