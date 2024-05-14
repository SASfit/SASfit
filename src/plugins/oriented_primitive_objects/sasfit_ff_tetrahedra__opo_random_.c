/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

opo_data tetrahedron_opod;
scalar sasfit_ff_tetrahedron_opo_kernel_Re_f(scalar theta, scalar phi, sasfit_param * param) {
    tetrahedron_opod.Q[0] = tetrahedron_opod.Qmod*cos(phi)*sin(theta);
    tetrahedron_opod.Q[1] = tetrahedron_opod.Qmod*sin(phi)*sin(theta);
    tetrahedron_opod.Q[2] = tetrahedron_opod.Qmod         *cos(theta);
    opo_setQhat(&tetrahedron_opod);
    QQX = tetrahedron_opod.Qhat[0];
    QQY = tetrahedron_opod.Qhat[1];
    QQZ = tetrahedron_opod.Qhat[2];
    return (ETA_P-ETA_M)*tetrahedron_opod.detDinv*Ftetrahedron_kernel_Re(&tetrahedron_opod);
}
scalar sasfit_ff_tetrahedron_opo_kernel_Im_f(scalar theta, scalar phi, sasfit_param * param) {
    tetrahedron_opod.Q[0] = tetrahedron_opod.Qmod*cos(phi)*sin(theta);
    tetrahedron_opod.Q[1] = tetrahedron_opod.Qmod*sin(phi)*sin(theta);
    tetrahedron_opod.Q[2] = tetrahedron_opod.Qmod         *cos(theta);
    opo_setQhat(&tetrahedron_opod);
    QQX = tetrahedron_opod.Qhat[0];
    QQY = tetrahedron_opod.Qhat[1];
    QQZ = tetrahedron_opod.Qhat[2];
    return (ETA_P-ETA_M)*tetrahedron_opod.detDinv*Ftetrahedron_kernel_Im(&tetrahedron_opod);
}
scalar sasfit_ff_tetrahedron_opo_kernel(scalar theta, scalar phi, sasfit_param * param) {
    tetrahedron_opod.Q[0] = tetrahedron_opod.Qmod*cos(phi)*sin(theta);
    tetrahedron_opod.Q[1] = tetrahedron_opod.Qmod*sin(phi)*sin(theta);
    tetrahedron_opod.Q[2] = tetrahedron_opod.Qmod         *cos(theta);
    opo_setQhat(&tetrahedron_opod);
    QQX = tetrahedron_opod.Qhat[0];
    QQY = tetrahedron_opod.Qhat[1];
    QQZ = tetrahedron_opod.Qhat[2];
    return gsl_pow_2((ETA_P-ETA_M)*tetrahedron_opod.detDinv*opo_Ftetrahedron(&tetrahedron_opod));
}
scalar sasfit_ff_tetrahedra__opo_random_(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B < 0.0), param, "b(%lg) < 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C < 0.0), param, "c(%lg) < 0",C); // modify condition to your needs
	SASFIT_CHECK_COND1((H_R > tan(TILT)/sqrt(3)&&tan(TILT)>0&&!gsl_isnan(tan(TILT))), param, "H/R > tan(TILT)/sqrt(3) (%lg)",tan(TILT)/sqrt(3));

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(tetrahedron_opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(tetrahedron_opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(tetrahedron_opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");

    if (A*B*C*H_R == 0.0) return 0;

	tetrahedron_opod.a = A;
	tetrahedron_opod.b = B;
	tetrahedron_opod.c = C;
	tetrahedron_opod.param = param;
    tetrahedron_opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&tetrahedron_opod,0,0,0);
    opo_init(&tetrahedron_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(tetrahedron_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be not linear independent");
    tetrahedron_opod.Qmod = q;
    sasfit_param_set_polar_theta(M_PI);
    sasfit_param_set_polar_phi(2*M_PI);
    return sasfit_orient_avg(&sasfit_ff_tetrahedron_opo_kernel,param);
}

scalar sasfit_ff_tetrahedra__opo_random__f(scalar q, sasfit_param * param)
{

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(tetrahedron_opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(tetrahedron_opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(tetrahedron_opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");

    if (A*B*C*H_R == 0.0) return 0;

	tetrahedron_opod.a = A;
	tetrahedron_opod.b = B;
	tetrahedron_opod.c = C;
	tetrahedron_opod.param = param;
    opo_setConvention(&tetrahedron_opod,yaw_pitch_roll);
    opo_setEulerAngles(&tetrahedron_opod,0,0,0);
    opo_init(&tetrahedron_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(tetrahedron_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be linear dependent");

    tetrahedron_opod.Qmod = q;
    sasfit_param_set_polar_theta(M_PI);
    sasfit_param_set_polar_phi(2*M_PI);
    return gsl_hypot(sasfit_orient_avg(&sasfit_ff_tetrahedron_opo_kernel_Re_f,param),sasfit_orient_avg(&sasfit_ff_tetrahedron_opo_kernel_Im_f,param));
}

scalar sasfit_ff_tetrahedra__opo_random__v(scalar q, sasfit_param * param, int dist)
{
    scalar TanAlpha;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	TanAlpha=2*sqrt(2);
	return 8.0/3.0*TanAlpha*(gsl_pow_3(1)-gsl_pow_3(1-sqrt(3)*H_R/TanAlpha))*tetrahedron_opod.detDinv;
}

