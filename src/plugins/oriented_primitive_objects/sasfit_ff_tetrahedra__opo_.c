/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables


scalar sasfit_ff_tetrahedra__opo_(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) <= 0",q);
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) <= 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B < 0.0), param, "b(%lg) <= 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C < 0.0), param, "c(%lg) <= 0",C); // modify condition to your needs
	SASFIT_CHECK_COND1((H_R > tan(TILT)/sqrt(3)&&tan(TILT)>0&&!gsl_isnan(tan(TILT))), param, "H/R > tan(TILT)/sqrt(3) (%lg)",tan(TILT));

    if (A*B*C*H_R == 0.0) return 0;

	// insert your code here

	return gsl_pow_2(sasfit_ff_tetrahedra__opo__f(q,param));
}

scalar sasfit_ff_tetrahedra__opo__f(scalar q, sasfit_param * param)
{
    scalar psi;
	opo_data opod;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B < 0.0), param, "b(%lg) < 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C < 0.0), param, "c(%lg) < 0",C); // modify condition to your needs
	SASFIT_CHECK_COND1((H_R > tan(TILT)/sqrt(3)&&tan(TILT)>0&&!gsl_isnan(tan(TILT))), param, "H/R > tan(TILT)/sqrt(3) (%lg)",tan(TILT));

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");

    if (A*B*C*H_R == 0.0) return 0;

	opod.a = A;
	opod.b = B;
	opod.c = C;
    opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&opod,ALPHA,BETA,GAMMA);
    opo_init(&opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be not linear independent");

    opod.Qmod = q;
    psi=sasfit_param_override_get_psi(PSI_DEG*M_PI/180.);
    opod.Q[0] = q*cos(psi);
    opod.Q[1] = q*sin(psi);
    opod.Q[2] = 0;
    opo_setQhat(&opod);
    opod.param = param;
	// insert your code here
	return (ETA_P-ETA_M) *opod.detDinv*opo_Ftetrahedron(&opod);
}

scalar sasfit_ff_tetrahedra__opo__v(scalar q, sasfit_param * param, int dist)
{
	opo_data opod;
	scalar TanAlpha;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
    opod.a = A;
	opod.b = B;
	opod.c = C;
    opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&opod,ALPHA,BETA,GAMMA);
    opo_init(&opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be not linear independent");
    TanAlpha=2*sqrt(2);
	return 8./3.*TanAlpha*(gsl_pow_3(1)-gsl_pow_3(1-sqrt(3)*H_R/TanAlpha))*opod.detDinv;
}

