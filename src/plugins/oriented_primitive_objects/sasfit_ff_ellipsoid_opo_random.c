/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
opo_data ell_opod;

scalar sasfit_ff_ellipsoid_opo_kernel_f(scalar theta, scalar phi, sasfit_param * param) {
    ell_opod.Q[0] = ell_opod.Qmod*cos(phi)*sin(theta);
    ell_opod.Q[1] = ell_opod.Qmod*sin(phi)*sin(theta);
    ell_opod.Q[2] = ell_opod.Qmod         *cos(theta);
    opo_setQhat(&ell_opod);
    return (ETA_P-ETA_M) *opo_Fe(&ell_opod);
}
scalar sasfit_ff_ellipsoid_opo_kernel(scalar theta, scalar phi, sasfit_param * param) {
    ell_opod.Q[0] = ell_opod.Qmod*cos(phi)*sin(theta);
    ell_opod.Q[1] = ell_opod.Qmod*sin(phi)*sin(theta);
    ell_opod.Q[2] = ell_opod.Qmod         *cos(theta);
    opo_setQhat(&ell_opod);
    return gsl_pow_2((ETA_P-ETA_M) *opo_Fe(&ell_opod));
}

scalar sasfit_ff_ellipsoid_opo_random(scalar q, sasfit_param * param)
{

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q); // modify condition to your needs
	SASFIT_CHECK_COND1((A <= 0.0), param, "a(%lg) <= 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B <= 0.0), param, "b(%lg) <= 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C <= 0.0), param, "c(%lg) <= 0",C); // modify condition to your needs

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(ell_opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(ell_opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(ell_opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");

	ell_opod.a = A;
	ell_opod.b = B;
	ell_opod.c = C;
    ell_opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&ell_opod,0,0,0);
    opo_init(&ell_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(ell_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be linear dependent");

    ell_opod.Qmod = q;
    sasfit_param_set_polar_theta(M_PI);
    sasfit_param_set_polar_phi(2*M_PI);
    return sasfit_orient_avg(&sasfit_ff_ellipsoid_opo_kernel,param);
}

scalar sasfit_ff_ellipsoid_opo_random_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(ell_opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(ell_opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(ell_opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");

	ell_opod.a = A;
	ell_opod.b = B;
	ell_opod.c = C;
    ell_opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&ell_opod,ALPHA,BETA,GAMMA);
    opo_init(&ell_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(ell_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be not linear independent");

    ell_opod.Qmod = q;
    sasfit_param_set_polar_theta(M_PI);
    sasfit_param_set_polar_phi(2*M_PI);
    return sasfit_orient_avg(&sasfit_ff_ellipsoid_opo_kernel,param);
}

scalar sasfit_ff_ellipsoid_opo_random_v(scalar q, sasfit_param * param, int dist)
{
    opo_data ell_opod;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
    ell_opod.a = A;
	ell_opod.b = B;
	ell_opod.c = C;
    ell_opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&ell_opod,ALPHA,BETA,GAMMA);
    opo_init(&ell_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(ell_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be not linear independent");

	return 4./3.*ell_opod.detDinv;
}

