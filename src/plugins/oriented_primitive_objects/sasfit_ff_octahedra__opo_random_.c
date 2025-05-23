/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
opo_data ohr_opod;

scalar sasfit_ff_octahedra_opo_kernel_f(scalar theta, scalar phi, sasfit_param * param) {
    ohr_opod.Q[0] = ohr_opod.Qmod*cos(phi)*sin(theta);
    ohr_opod.Q[1] = ohr_opod.Qmod*sin(phi)*sin(theta);
    ohr_opod.Q[2] = ohr_opod.Qmod         *cos(theta);
    opo_setQhat(&ohr_opod);
    return (ETA_P-ETA_M) *ohr_opod.detDinv*opo_FOH(&ohr_opod);
}
scalar sasfit_ff_octahedra_opo_kernel(scalar theta, scalar phi, sasfit_param * param) {
    return gsl_pow_2(sasfit_ff_octahedra_opo_kernel_f(theta,phi,param));
}

scalar sasfit_ff_octahedra__opo_random_(scalar q, sasfit_param * param)
{

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q); // modify condition to your needs
	SASFIT_CHECK_COND1((A <= 0.0), param, "a(%lg) <= 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B <= 0.0), param, "b(%lg) <= 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C <= 0.0), param, "c(%lg) <= 0",C); // modify condition to your needs

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(ohr_opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(ohr_opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(ohr_opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");

	ohr_opod.a = A/M_SQRT2;
	ohr_opod.b = B*ohr_opod.a;
	ohr_opod.c = C*ohr_opod.a;
    ohr_opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&ohr_opod,0,0,0);
    opo_init(&ohr_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(ohr_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be linear dependent");

    ohr_opod.Qmod = q;
    sasfit_param_set_polar_theta(M_PI);
    sasfit_param_set_polar_phi(2*M_PI);
    return sasfit_orient_avg(&sasfit_ff_octahedra_opo_kernel,param);
}

scalar sasfit_ff_octahedra__opo_random__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q); // modify condition to your needs
	SASFIT_CHECK_COND1((A <= 0.0), param, "a(%lg) <= 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B <= 0.0), param, "b(%lg) <= 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C <= 0.0), param, "c(%lg) <= 0",C); // modify condition to your needs

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(ohr_opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(ohr_opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(ohr_opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");

	ohr_opod.a = A/M_SQRT2;
	ohr_opod.b = B*ohr_opod.a;
	ohr_opod.c = C*ohr_opod.a;
    ohr_opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&ohr_opod,0,0,0);
    opo_init(&ohr_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(ohr_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be linear dependent");

    ohr_opod.Qmod = q;
    sasfit_param_set_polar_theta(M_PI);
    sasfit_param_set_polar_phi(2*M_PI);
    return sasfit_orient_avg(&sasfit_ff_octahedra_opo_kernel_f,param);
}

scalar sasfit_ff_octahedra__opo_random__v(scalar x, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
    if (dist == 0) {
	    ohr_opod.a = x/M_SQRT2;
	} else {
	    ohr_opod.a = A/M_SQRT2;
	}
	if (dist == 4) {
	    ohr_opod.b = x*ohr_opod.a;
	} else {
	    ohr_opod.b = B*ohr_opod.a;
	}
	if (dist == 8) {
	    ohr_opod.c = x*ohr_opod.a;
	} else {
	    ohr_opod.c = C*ohr_opod.a;
	}
    ohr_opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&ohr_opod,ALPHA,BETA,GAMMA);
    opo_init(&ohr_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(ohr_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be not linear independent");

	return 4./3.*ohr_opod.detDinv;
}

