/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
opo_data s_toroid_opod;

scalar sasfit_ff_supertoroid_helix_opo_random(scalar q, sasfit_param * param)
{
	scalar sum;
    scalar psi;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q); // modify condition to your needs
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B < 0.0), param, "b(%lg) < 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C < 0.0), param, "c(%lg) < 0",C); // modify condition to your needs

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(s_toroid_opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(s_toroid_opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(s_toroid_opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");
	if (A*B*C==0) return 0;

	s_toroid_opod.a = A;
	s_toroid_opod.b = A*B;
	s_toroid_opod.c = A*C;
    s_toroid_opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&s_toroid_opod,ALPHA,BETA,GAMMA);
    opo_init(&s_toroid_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(s_toroid_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be linear dependent");

    s_toroid_opod.Qmod = q;
    psi=sasfit_param_override_get_psi(PSI_DEG*M_PI/180.);
    s_toroid_opod.Q[0] = q*cos(psi);
    s_toroid_opod.Q[1] = q*sin(psi);
    s_toroid_opod.Q[2] = 0;
	s_toroid_opod.param=param;
    opo_setQhat(&s_toroid_opod);
    QQX = s_toroid_opod.Qhat[0];
    QQY = s_toroid_opod.Qhat[1];
    QQZ = s_toroid_opod.Qhat[2];
    sum = opo_Fsuper_toroid_helix(param);
//    sasfit_out("detDinv=%lg,FFss=%lg\n",s_toroid_opod.detDinv,sum);
//    sasfit_out("fm=%lg falpha=%lg n1=%lg n2=%lg n3=%lg a=%lg b=%lg\n",FMM,FALPHA,FN1,FN2,FN3,FAA,FBB);
//    sasfit_out("sm=%lg salpha=%lg N1=%lg N2=%lg N3=%lg A=%lg B=%lg\n",SMM,SALPHA,SN1,SN2,SN3,SAA,SBB);
	return gsl_pow_2((ETA_P-ETA_M)*s_toroid_opod.detDinv*sum);
}

scalar sasfit_ff_supertoroid_helix_opo_random_f(scalar q, sasfit_param * param)
{
	scalar sum;
    scalar psi;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(s_toroid_opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(s_toroid_opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(s_toroid_opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");
	if (A*B*C==0) return 0;

	s_toroid_opod.a = A;
	s_toroid_opod.b = A*B;
	s_toroid_opod.c = A*C;
    s_toroid_opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&s_toroid_opod,ALPHA,BETA,GAMMA);
    opo_init(&s_toroid_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(s_toroid_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be linear dependent");

    s_toroid_opod.Qmod = q;
    psi=sasfit_param_override_get_psi(PSI_DEG*M_PI/180.);
    s_toroid_opod.Q[0] = q*cos(psi);
    s_toroid_opod.Q[1] = q*sin(psi);
    s_toroid_opod.Q[2] = 0;
	s_toroid_opod.param=param;
    opo_setQhat(&s_toroid_opod);
    QQX = s_toroid_opod.Qhat[0];
    QQY = s_toroid_opod.Qhat[1];
    QQZ = s_toroid_opod.Qhat[2];
	return (ETA_P-ETA_M)*s_toroid_opod.detDinv*opo_Fsuper_toroid_helix(param);
}

scalar sasfit_ff_supertoroid_helix_opo_random_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

