/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

scalar sasfit_ff_cone_opo(scalar q, sasfit_param * param)
{
    scalar psi;
	opo_data opod;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A <= 0.0), param, "a(%lg) <= 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B <= 0.0), param, "b(%lg) <= 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C <= 0.0), param, "c(%lg) <= 0",C); // modify condition to your needs
	SASFIT_CHECK_COND1((H_R >= tan(TILT)&&tan(TILT)>0&&!gsl_isnan(tan(TILT))), param, "H/R >= tan(TILT) (%lg)",tan(TILT));

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");

	opod.a = A;
	opod.b = B;
	opod.c = C;
	opod.param = param;
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
	return gsl_pow_2((ETA_P-ETA_M) *opo_Fcone(&opod));
}

scalar sasfit_ff_cone_opo_f(scalar q, sasfit_param * param)
{
    scalar psi;
	opo_data opod;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");

	opod.a = A;
	opod.b = B;
	opod.c = C;
	opod.param = param;
    opo_setConvention(&opod,yaw_pitch_roll);
 //   opod.Rotation.convention =Z1_X2_Y3;
    opo_setEulerAngles(&opod,ALPHA,BETA,GAMMA);
    opo_init(&opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be linear dependent");

    opod.Qmod = q;
    psi=sasfit_param_override_get_psi(PSI_DEG*M_PI/180.);
    opod.Q[0] = q*cos(psi);
    opod.Q[1] = q*sin(psi);
    opod.Q[2] = 0;
    opo_setQhat(&opod);
	return (ETA_P-ETA_M) *opo_Fcone(&opod);
}

scalar sasfit_ff_cone_opo_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

