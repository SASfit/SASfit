/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

scalar sasfit_ff_dodecahedra__opo_(scalar q, sasfit_param * param)
{
	opo_data opod;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A <= 0.0), param, "a(%lg) <= 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B <= 0.0), param, "b(%lg) <= 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C <= 0.0), param, "c(%lg) <= 0",C); // modify condition to your needs

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");

    return gsl_pow_2(sasfit_ff_dodecahedra__opo__f(q,param));
}

scalar sasfit_ff_dodecahedra__opo__f(scalar q, sasfit_param * param)
{
    scalar Qx,Qy,Qz;
    double complex FRD;
    scalar psi;
	opo_data opod;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A <= 0.0), param, "a(%lg) <= 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B <= 0.0), param, "b(%lg) <= 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C <= 0.0), param, "c(%lg) <= 0",C); // modify condition to your needs

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

    opod.Qmod = q;
    psi=sasfit_param_override_get_psi(PSI_DEG*M_PI/180.);
    opod.Q[0] = q*cos(psi);
    opod.Q[1] = q*sin(psi);
    opod.Q[2] = 0;
    opo_setQhat(&opod);

    Qx = opod.Qhat[0];
    Qy = opod.Qhat[1];
    Qz = opod.Qhat[2];
    H_R = 1-DBL_EPSILON;
    param->p[18]=45; // this is TILT
    FRD =  opo_CmplxFpyramid4(Qx,Qy, Qz,param) * cexp( I*Qz)
          +opo_CmplxFpyramid4(Qx,Qy,-Qz,param) * cexp(-I*Qz)
          +opo_CmplxFpyramid4(Qx,Qz, Qy,param) * cexp( I*Qy)
          +opo_CmplxFpyramid4(Qx,Qz,-Qy,param) * cexp(-I*Qy)
          +opo_CmplxFpyramid4(Qz,Qy, Qx,param) * cexp( I*Qx)
          +opo_CmplxFpyramid4(Qz,Qy,-Qx,param) * cexp(-I*Qx)
          +8*opo_sinc(Qx)*opo_sinc(Qy)*opo_sinc(Qz);
    return (ETA_P-ETA_M) *opod.detDinv*cabs(FRD);
}

scalar sasfit_ff_dodecahedra__opo__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
    H_R = 1-DBL_EPSILON;
    param->p[18]=45;
	return (4*H_R*(3 + H_R*opo_Cot(TILT)*(-3 + H_R*opo_Cot(TILT))))/3.*opod.detDinv;
}

