/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>
#include "complex.h"

// define shortcuts for local parameters/variables

opo_data dh_opod;

scalar sasfit_ff_rhombic_dodecahedra_opo_kernel_f(scalar theta, scalar phi, sasfit_param * param) {
    double complex FRD;
    scalar Qx,Qy,Qz;
    dh_opod.Q[0] = dh_opod.Qmod*cos(phi)*sin(theta);
    dh_opod.Q[1] = dh_opod.Qmod*sin(phi)*sin(theta);
    dh_opod.Q[2] = dh_opod.Qmod         *cos(theta);
    opo_setQhat(&dh_opod);
    Qx = dh_opod.Qhat[0];
    Qy = dh_opod.Qhat[1];
    Qz = dh_opod.Qhat[2];
    H_R = 1- DBL_EPSILON;
    param->p[18]=45; // this is TILT
    FRD =  opo_CmplxFpyramid4(Qx,Qy, Qz,param) * cexp( I*Qz)
          +opo_CmplxFpyramid4(Qx,Qy,-Qz,param) * cexp(-I*Qz)
          +opo_CmplxFpyramid4(Qx,Qz, Qy,param) * cexp( I*Qy)
          +opo_CmplxFpyramid4(Qx,Qz,-Qy,param) * cexp(-I*Qy)
          +opo_CmplxFpyramid4(Qz,Qy, Qx,param) * cexp( I*Qx)
          +opo_CmplxFpyramid4(Qz,Qy,-Qx,param) * cexp(-I*Qx)
          +8*opo_sinc(Qx)*opo_sinc(Qy)*opo_sinc(Qz);
    return (ETA_P-ETA_M) *dh_opod.detDinv*cabs(FRD);
}

scalar sasfit_ff_rhombic_dodecahedra_opo_kernel(scalar theta, scalar phi, sasfit_param * param) {
    return gsl_pow_2(sasfit_ff_rhombic_dodecahedra_opo_kernel_f(theta,phi,param));
}

scalar sasfit_ff_rhombic_dodecahedra__opo_random_(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q); // modify condition to your needs
	SASFIT_CHECK_COND1((A <= 0.0), param, "a(%lg) <= 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B <= 0.0), param, "b(%lg) <= 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C <= 0.0), param, "c(%lg) <= 0",C); // modify condition to your needs

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(dh_opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(dh_opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(dh_opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");

	dh_opod.a = A;
	dh_opod.b = B;
	dh_opod.c = C;
    dh_opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&dh_opod,0,0,0);
    opo_init(&dh_opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(dh_opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be linear dependent");

    dh_opod.Qmod = q;
    sasfit_param_set_polar_theta(M_PI);
    sasfit_param_set_polar_phi(2*M_PI);
    return sasfit_orient_avg(&sasfit_ff_rhombic_dodecahedra_opo_kernel,param);
}

scalar sasfit_ff_rhombic_dodecahedra__opo_random__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_rhombic_dodecahedra__opo_random__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

