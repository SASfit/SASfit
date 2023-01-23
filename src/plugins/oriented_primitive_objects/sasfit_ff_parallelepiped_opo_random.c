/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>
opo_data p_epiped_opod;

scalar sasfit_ff_parallelepiped_opo_kernel_f(scalar theta, scalar phi, sasfit_param * param) {
    p_epiped_opod.Q[0] = p_epiped_opod.Qmod*cos(phi)*sin(theta);
    p_epiped_opod.Q[1] = p_epiped_opod.Qmod*sin(phi)*sin(theta);
    p_epiped_opod.Q[2] = p_epiped_opod.Qmod         *cos(theta);
    opo_setQhat(&p_epiped_opod);
    return (ETA_P-ETA_M) *opo_Fp(&p_epiped_opod);
}
scalar sasfit_ff_parallelepiped_opo_kernel(scalar theta, scalar phi, sasfit_param * param) {
    p_epiped_opod.Q[0] = p_epiped_opod.Qmod*cos(phi)*sin(theta);
    p_epiped_opod.Q[1] = p_epiped_opod.Qmod*sin(phi)*sin(theta);
    p_epiped_opod.Q[2] = p_epiped_opod.Qmod         *cos(theta);
    opo_setQhat(&p_epiped_opod);
    return gsl_pow_2((ETA_P-ETA_M) *opo_Fp(&p_epiped_opod));
}

scalar sasfit_ff_parallelepiped_opo_random(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q); // modify condition to your needs
	SASFIT_CHECK_COND1((A <= 0.0), param, "a(%lg) <= 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B <= 0.0), param, "b(%lg) <= 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C <= 0.0), param, "c(%lg) <= 0",C); // modify condition to your needs


	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(p_epiped_opod.ea,EA_X,EA_Y,EA_Z),0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(p_epiped_opod.eb,EB_X,EB_Y,EB_Z),0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(p_epiped_opod.ec,EC_X,EC_Y,EC_Z),0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");

	p_epiped_opod.a = A;
	p_epiped_opod.b = A*B;
	p_epiped_opod.c = A*C;
    p_epiped_opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&p_epiped_opod,0,0,0);
    opo_init(&p_epiped_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(p_epiped_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be linear dependent");

    p_epiped_opod.Qmod = q;
    sasfit_param_set_polar_theta(M_PI_2);
    sasfit_param_set_polar_phi(M_PI_2);
    return sasfit_orient_avg(&sasfit_ff_parallelepiped_opo_kernel,param);
}

scalar sasfit_ff_parallelepiped_opo_random_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(p_epiped_opod.ea,EA_X,EA_Y,EA_Z),0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(p_epiped_opod.eb,EB_X,EB_Y,EB_Z),0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(p_epiped_opod.ec,EC_X,EC_Y,EC_Z),0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");

	p_epiped_opod.a = A;
	p_epiped_opod.b = B;
	p_epiped_opod.c = C;
    p_epiped_opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&p_epiped_opod,0,0,0);
    opo_init(&p_epiped_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(p_epiped_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be not linear independent");

    p_epiped_opod.Qmod = q;
    sasfit_param_set_polar_theta(M_PI_2);
    sasfit_param_set_polar_phi(M_PI_2);
    return sasfit_orient_avg(&sasfit_ff_parallelepiped_opo_kernel_f,param);
}

scalar sasfit_ff_parallelepiped_opo_random_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 8.*p_epiped_opod.detDinv;
}

