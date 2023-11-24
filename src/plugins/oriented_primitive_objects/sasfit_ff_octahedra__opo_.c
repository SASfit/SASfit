/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

opo_data oh_opod;
scalar sasfit_ff_octahedra__opo_(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A <= 0.0), param, "a(%lg) <= 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B <= 0.0), param, "b(%lg) <= 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C <= 0.0), param, "c(%lg) <= 0",C); // modify condition to your needs

	// insert your code here

	return gsl_pow_2(sasfit_ff_octahedra__opo__f(q,param));
}

scalar sasfit_ff_octahedra__opo__f(scalar q, sasfit_param * param)
{
    scalar psi;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A <= 0.0), param, "a(%lg) <= 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B <= 0.0), param, "b(%lg) <= 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C <= 0.0), param, "c(%lg) <= 0",C); // modify condition to your needs

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(oh_opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(oh_opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(oh_opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");

	oh_opod.a = A/M_SQRT2;
	oh_opod.b = B*oh_opod.a;
	oh_opod.c = C*oh_opod.a;
    oh_opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&oh_opod,ALPHA,BETA,GAMMA);
    opo_init(&oh_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(oh_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be not linear independent");

    oh_opod.Qmod = q;
    psi=sasfit_param_override_get_psi(PSI_DEG*M_PI/180.);
    oh_opod.Q[0] = q*cos(psi);
    oh_opod.Q[1] = q*sin(psi);
    oh_opod.Q[2] = 0;
    opo_setQhat(&oh_opod);
	// insert your code here

	return (ETA_P-ETA_M) *oh_opod.detDinv*opo_FOH(&oh_opod);
}

scalar sasfit_ff_octahedra__opo__v(scalar x, sasfit_param * param, int dist)
{
    if (dist == 0) {
	    oh_opod.a = x/M_SQRT2;
	} else {
	    oh_opod.a = A/M_SQRT2;
	}
	if (dist == 4) {
	    oh_opod.b = x*oh_opod.a;
	} else {
	    oh_opod.b = B*oh_opod.a;
	}
	if (dist == 8) {
	    oh_opod.c = x*oh_opod.a;
	} else {
	    oh_opod.c = C*oh_opod.a;
	}
    oh_opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&oh_opod,ALPHA,BETA,GAMMA);
    opo_init(&oh_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(oh_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be not linear independent");

	return 4./3.*oh_opod.detDinv;
}

