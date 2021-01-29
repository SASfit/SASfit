/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>
opo_data cone_opod;
scalar sasfit_ff_cone_opo_kernel_Re_f(scalar theta, scalar phi, sasfit_param * param) {
    cone_opod.Q[0] = cone_opod.Qmod*cos(phi)*sin(theta);
    cone_opod.Q[1] = cone_opod.Qmod*sin(phi)*sin(theta);
    cone_opod.Q[2] = cone_opod.Qmod         *cos(theta);
    opo_setQhat(&cone_opod);
    QQX = cone_opod.Qhat[0];
    QQY = cone_opod.Qhat[1];
    QQZ = cone_opod.Qhat[2];
    return (ETA_P-ETA_M)*M_PI/3.*tan(TILT)*(1-gsl_pow_3(1-H_R/tan(TILT)))*sasfit_integrate(0,H_R,&Fcone_kernel_Re,param);
}
scalar sasfit_ff_cone_opo_kernel_Im_f(scalar theta, scalar phi, sasfit_param * param) {
    cone_opod.Q[0] = cone_opod.Qmod*cos(phi)*sin(theta);
    cone_opod.Q[1] = cone_opod.Qmod*sin(phi)*sin(theta);
    cone_opod.Q[2] = cone_opod.Qmod         *cos(theta);
    opo_setQhat(&cone_opod);
    QQX = cone_opod.Qhat[0];
    QQY = cone_opod.Qhat[1];
    QQZ = cone_opod.Qhat[2];
    return (ETA_P-ETA_M)*M_PI/3.*tan(TILT)*(1-gsl_pow_3(1-H_R/tan(TILT)))*sasfit_integrate(0,H_R,&Fcone_kernel_Im,param);
}
scalar sasfit_ff_cone_opo_kernel(scalar theta, scalar phi, sasfit_param * param) {
    cone_opod.Q[0] = cone_opod.Qmod*cos(phi)*sin(theta);
    cone_opod.Q[1] = cone_opod.Qmod*sin(phi)*sin(theta);
    cone_opod.Q[2] = cone_opod.Qmod         *cos(theta);
    opo_setQhat(&cone_opod);
    return gsl_pow_2((ETA_P-ETA_M)*M_PI/3.*tan(TILT)*(1-gsl_pow_3(1-H_R/tan(TILT)))*opo_Fcone(&cone_opod));
}

scalar sasfit_ff_cone_opo_random(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A <= 0.0), param, "a(%lg) <= 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B <= 0.0), param, "b(%lg) <= 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C <= 0.0), param, "c(%lg) <= 0",C); // modify condition to your needs
	SASFIT_CHECK_COND1((H_R >= tan(TILT)&&tan(TILT)>0&&!gsl_isnan(tan(TILT))), param, "H/R >= tan(TILT) (%lg)",tan(TILT));

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(cone_opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(cone_opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(cone_opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");

	cone_opod.a = A;
	cone_opod.b = B;
	cone_opod.c = C;
	cone_opod.param = param;
    cone_opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&cone_opod,0,0,0);
    opo_init(&cone_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(cone_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be not linear independent");
    cone_opod.Qmod = q;
    return sasfit_orient_avg(&sasfit_ff_cone_opo_kernel,param);
}

scalar sasfit_ff_cone_opo_random_f(scalar q, sasfit_param * param)
{

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(cone_opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(cone_opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(cone_opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");

	cone_opod.a = A;
	cone_opod.b = B;
	cone_opod.c = C;
	cone_opod.param = param;
    opo_setConvention(&cone_opod,yaw_pitch_roll);
 //   cone_opod.Rotation.convention =Z1_X2_Y3;
    opo_setEulerAngles(&cone_opod,0,0,0);
    opo_init(&cone_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(cone_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be linear dependent");

    cone_opod.Qmod = q;
    return gsl_hypot(sasfit_orient_avg(&sasfit_ff_cone_opo_kernel_Re_f,param),sasfit_orient_avg(&sasfit_ff_cone_opo_kernel_Im_f,param));
}

scalar sasfit_ff_cone_opo_random_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return cone_opod.detDinv;
}

