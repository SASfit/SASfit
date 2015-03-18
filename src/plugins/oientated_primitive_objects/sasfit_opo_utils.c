/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

void opo_setConvention (opo_data *opod, opo_Euler_convention_t convention)
{
    opo_rot *opoparam;
    opoparam = &opod->Rotation;
    opoparam->convention = convention;
}

void opo_setEulerAngles(opo_data *opod, double alpha, double beta,double gamma)
{
    opo_rot *opoparam;
    opoparam = &opod->Rotation;
    opoparam->EulerAngles[0] = alpha;  // [0, 2 pi]
    opoparam->EulerAngles[1] = beta;   // [0, pi]
    opoparam->EulerAngles[2] = gamma;  // [0, 2 pi]
}

void opo_setRotationMatrix(opo_data *opod) {
    double s[3], c[3];
    int i;
    opo_rot *opoparam;
    opoparam = &opod->Rotation;

    for (i=0;i<3;i++) {
        s[i] = sin(opoparam->EulerAngles[i]);
        c[i] = cos(opoparam->EulerAngles[i]);
    }
    switch (opoparam->convention) {
// proper Euler Angles:
        case X1_Z2_X3 : {
                        opoparam->RotationMatrix[0][0] = c[1];
                        opoparam->RotationMatrix[1][0] = c[0]*s[1];
                        opoparam->RotationMatrix[2][0] = s[0]*s[1];

                        opoparam->RotationMatrix[0][1] = -c[2]*s[1];
                        opoparam->RotationMatrix[1][1] =  c[0]*c[1]*c[2]-s[0]*s[2];
                        opoparam->RotationMatrix[2][1] =  c[0]*s[2]     +c[1]*c[2]*s[0];

                        opoparam->RotationMatrix[0][2] =  s[1]*s[2];
                        opoparam->RotationMatrix[1][2] = -c[2]*s[0]-c[0]*c[1]*s[2];
                        opoparam->RotationMatrix[2][2] =  c[0]*c[2]-c[1]*s[0]*s[2];
                        break;
                        }
        case X1_Y2_X3 : {
                        opoparam->RotationMatrix[0][0] =  c[1];
                        opoparam->RotationMatrix[1][0] =  s[0]*s[1];
                        opoparam->RotationMatrix[2][0] = -c[0]*s[1];

                        opoparam->RotationMatrix[0][1] =  s[1]*s[2];
                        opoparam->RotationMatrix[1][1] =  c[0]*c[2]-c[1]*s[0]*s[2];
                        opoparam->RotationMatrix[2][1] =  c[2]*s[0]+c[0]*c[1]*s[2];

                        opoparam->RotationMatrix[0][2] =  c[2]*s[1];
                        opoparam->RotationMatrix[1][2] = -c[0]*s[2]     -c[1]*c[2]*s[0];
                        opoparam->RotationMatrix[2][2] =  c[0]*c[1]*c[2]-s[0]*s[2];
                        break;
                        }
        case Y1_X2_Y3 : {
                        opoparam->RotationMatrix[0][0] =  c[0]*c[2]-c[1]*s[0]*s[2];
                        opoparam->RotationMatrix[1][0] =  s[1]*s[2];
                        opoparam->RotationMatrix[2][0] = -c[2]*s[0]-c[0]*c[1]*s[2];

                        opoparam->RotationMatrix[0][1] =  s[0]*s[1];
                        opoparam->RotationMatrix[1][1] =  c[1];
                        opoparam->RotationMatrix[2][1] =  c[0]*s[1];

                        opoparam->RotationMatrix[0][2] =  c[0]*s[2]+c[1]*c[2]*s[0];
                        opoparam->RotationMatrix[1][2] = -c[2]*s[1];
                        opoparam->RotationMatrix[2][2] =  c[0]*c[1]*c[2]-s[0]*s[2];
                        break;
                        }
        case Y1_Z2_Y3 : {
                        opoparam->RotationMatrix[0][0] =  c[0]*c[1]*c[2]-s[0]*s[2];
                        opoparam->RotationMatrix[1][0] =  c[2]*s[1];
                        opoparam->RotationMatrix[2][0] = -c[0]*s[2]-c[1]*c[2]*s[0];

                        opoparam->RotationMatrix[0][1] = -c[0]*s[1];
                        opoparam->RotationMatrix[1][1] =  c[1];
                        opoparam->RotationMatrix[2][1] =  s[0]*s[1];

                        opoparam->RotationMatrix[0][2] =  c[2]*s[0]+c[0]*c[1]*s[2];
                        opoparam->RotationMatrix[1][2] =  s[1]*s[2];
                        opoparam->RotationMatrix[2][2] =  c[0]*c[2]-c[1]*s[0]*s[2];
                        break;
                        }
        case Z1_X2_Z3 : { // x-convention
                        opoparam->RotationMatrix[0][0] = c[0]*c[2]-c[1]*s[0]*s[2];
                        opoparam->RotationMatrix[1][0] = c[2]*s[0]+c[0]*c[1]*s[2];
                        opoparam->RotationMatrix[2][0] = s[1]*s[2];

                        opoparam->RotationMatrix[0][1] = -c[0]*s[2]     -c[1]*c[2]*s[0];
                        opoparam->RotationMatrix[1][1] =  c[0]*c[1]*c[2]-s[0]*s[2];
                        opoparam->RotationMatrix[2][1] =  c[2]*s[1];

                        opoparam->RotationMatrix[0][2] =  s[0]*s[1];
                        opoparam->RotationMatrix[1][2] = -c[0]*s[1];
                        opoparam->RotationMatrix[2][2] =  c[2];
                        break;
                        }
        case Z1_Y2_Z3 : {
                        opoparam->RotationMatrix[0][0] =  c[0]*c[1]*c[2]-s[0]*s[2];
                        opoparam->RotationMatrix[1][0] =  c[0]*s[2]     +c[1]*c[2]*s[0];
                        opoparam->RotationMatrix[2][0] = -c[2]*s[1];

                        opoparam->RotationMatrix[0][1] = -c[2]*s[0]-c[0]*c[1]*s[2];
                        opoparam->RotationMatrix[1][1] =  c[0]*c[2]-c[1]*s[0]*s[2];
                        opoparam->RotationMatrix[2][1] =  s[1]*s[2];

                        opoparam->RotationMatrix[0][2] = c[0]*s[1];
                        opoparam->RotationMatrix[1][2] = s[0]*s[1];
                        opoparam->RotationMatrix[2][2] = c[1];
                        break;
                        }
// Tait-Bryan angles:
        case X1_Z2_Y3 : {
                        opoparam->RotationMatrix[0][0] =  c[1]*c[2];
                        opoparam->RotationMatrix[1][0] =  s[0]*s[2]     +c[0]*c[2]*s[1];
                        opoparam->RotationMatrix[2][0] =  c[2]*s[0]*s[1]-c[0]*s[2];

                        opoparam->RotationMatrix[0][1] = -s[1];
                        opoparam->RotationMatrix[1][1] =  c[0]*c[1];
                        opoparam->RotationMatrix[2][1] =  c[1]*s[0];

                        opoparam->RotationMatrix[0][2] = c[1]*s[2];
                        opoparam->RotationMatrix[1][2] = c[0]*s[1]*s[2]-c[2]*s[0];
                        opoparam->RotationMatrix[2][2] = c[0]*c[2]     +s[0]*s[1]*s[2];
                        break;
                        }
        case X1_Y2_Z3 : {
                        opoparam->RotationMatrix[0][0] =  c[1]*c[2];
                        opoparam->RotationMatrix[1][0] =  c[0]*s[2]+c[2]*s[0]*s[1];
                        opoparam->RotationMatrix[2][0] =  s[0]*s[2]-c[0]*c[2]*s[1];

                        opoparam->RotationMatrix[0][1] = -c[1]*s[2];
                        opoparam->RotationMatrix[1][1] =  c[0]*c[2]-s[0]*s[1]*s[2];
                        opoparam->RotationMatrix[2][1] =  c[2]*s[0]+c[0]*s[1]*s[2];

                        opoparam->RotationMatrix[0][2] =  s[1];
                        opoparam->RotationMatrix[1][2] = -c[1]*s[0];
                        opoparam->RotationMatrix[2][2] =  c[0]*c[1];
                        break;
                        }
        case Y1_X2_Z3 : {
                        opoparam->RotationMatrix[0][0] =  c[0]*c[2]     +s[0]*s[1]*s[2];
                        opoparam->RotationMatrix[1][0] =  c[1]*s[2];
                        opoparam->RotationMatrix[2][0] =  c[0]*s[1]*s[2]-c[2]*s[1];

                        opoparam->RotationMatrix[0][1] =  c[2]*s[0]*s[1]-c[0]*s[2];
                        opoparam->RotationMatrix[1][1] =  c[1]*c[2];
                        opoparam->RotationMatrix[2][1] =  c[0]*c[2]*s[1]+s[0]*s[2];

                        opoparam->RotationMatrix[0][2] =  c[1]*s[0];
                        opoparam->RotationMatrix[1][2] = -s[1];
                        opoparam->RotationMatrix[2][2] =  c[0]*c[1];
                        break;
                        }
        case Y1_Z2_X3 : {
                        opoparam->RotationMatrix[0][0] =  c[0]*c[1];
                        opoparam->RotationMatrix[1][0] =  s[1];
                        opoparam->RotationMatrix[2][0] = -c[1]*s[0];

                        opoparam->RotationMatrix[0][1] = s[0]*s[2]-c[0]*c[2]*s[1];
                        opoparam->RotationMatrix[1][1] = c[1]*c[2];
                        opoparam->RotationMatrix[2][1] = c[0]*s[2]+c[2]*s[0]*s[1];

                        opoparam->RotationMatrix[0][2] =  c[2]*s[0]+c[0]*s[1]*s[2];
                        opoparam->RotationMatrix[1][2] = -c[1]*s[2];
                        opoparam->RotationMatrix[2][2] =  c[0]*c[2]-s[0]*s[1]*s[2];
                        break;
                        }
        case Z1_Y2_X3 : {
                        opoparam->RotationMatrix[0][0] = c[0]*c[1];
                        opoparam->RotationMatrix[1][0] = c[1]*s[0];
                        opoparam->RotationMatrix[2][0] = -s[1];

                        opoparam->RotationMatrix[0][1] = c[0]*s[1]*s[2]-c[2]*s[0];
                        opoparam->RotationMatrix[1][1] = c[0]*c[2]     +s[0]*s[1]*s[2];
                        opoparam->RotationMatrix[2][1] = c[1]*s[2];

                        opoparam->RotationMatrix[0][2] = s[0]*s[2]     +c[0]*c[2]*s[1];
                        opoparam->RotationMatrix[1][2] = c[2]*s[0]*s[1]-c[0]*s[2];
                        opoparam->RotationMatrix[2][2] = c[1]*c[2];
                        break;
                        }
        case Z1_X2_Y3 : {
                        opoparam->RotationMatrix[0][0] =  c[0]*c[2]-s[0]*s[1]*s[2];
                        opoparam->RotationMatrix[1][0] =  c[2]*s[0]+c[0]*s[1]*s[2];
                        opoparam->RotationMatrix[2][0] = -c[1]*s[2];

                        opoparam->RotationMatrix[0][1] = -c[1]*s[0];
                        opoparam->RotationMatrix[1][1] =  c[0]*c[1];
                        opoparam->RotationMatrix[2][1] =  s[1];

                        opoparam->RotationMatrix[0][2] = c[0]*s[2]+c[2]*s[0]*s[1];
                        opoparam->RotationMatrix[1][2] = s[0]*s[2]-c[0]*c[2]*s[1];
                        opoparam->RotationMatrix[2][2] = c[1]*c[2];
                        break;
                        }
        default:        {// x-convention
                        opoparam->RotationMatrix[0][0] = c[0]*c[2]-c[1]*s[0]*s[2];
                        opoparam->RotationMatrix[1][0] = c[2]*s[0]+c[0]*c[1]*s[2];
                        opoparam->RotationMatrix[2][0] = s[1]*s[2];

                        opoparam->RotationMatrix[0][1] = -c[0]*s[2]     -c[1]*c[2]*s[0];
                        opoparam->RotationMatrix[1][1] =  c[0]*c[1]*c[2]-s[0]*s[2];
                        opoparam->RotationMatrix[2][1] =  c[2]*s[1];

                        opoparam->RotationMatrix[0][2] =  s[0]*s[1];
                        opoparam->RotationMatrix[1][2] = -c[0]*s[1];
                        opoparam->RotationMatrix[2][2] =  c[2];
                        }
    }
}

int opo_set_e(double *e, double ex, double ey, double ez) {
    scalar emod;
    emod = sqrt(ex*ex+ey*ey+ez*ez);
    if (emod == 0) return 0;
    e[0]=ex/emod;
    e[1]=ey/emod;
    e[2]=ez/emod;
    return 1;
}

void opo_setDinv(opo_data *opod) {
    opod->Dinv[0][0] =opod->a*opod->ea[0];
    opod->Dinv[1][0] =opod->a*opod->ea[1];
    opod->Dinv[2][0] =opod->a*opod->ea[2];

    opod->Dinv[0][1] =opod->b*opod->eb[0];
    opod->Dinv[1][1] =opod->b*opod->eb[1];
    opod->Dinv[2][1] =opod->b*opod->eb[2];

    opod->Dinv[0][2] =opod->c*opod->ec[0];
    opod->Dinv[1][2] =opod->c*opod->ec[1];
    opod->Dinv[2][2] =opod->c*opod->ec[2];
}

void opo_setD(opo_data *opod) {
    opod->D[0][0] =+(opod->b*opod->eb[1]*opod->c*opod->ec[2]-opod->b*opod->eb[2]*opod->c*opod->ec[1]); // b*eb[1]*c*ec[2]-b*eb[2]*c*ec[1]
    opod->D[1][0] =+(opod->b*opod->eb[2]*opod->c*opod->ec[0]-opod->b*opod->eb[0]*opod->c*opod->ec[2]); // b*eb[2]*c*ec[0]-b*eb[0]*c*ec[2]
    opod->D[2][0] =+(opod->b*opod->eb[0]*opod->c*opod->ec[1]-opod->b*opod->eb[1]*opod->c*opod->ec[0]); // b*eb[0]*c*ec[1]-b*eb[1]*c*ec[0]

    opod->D[0][1] =-(opod->a*opod->ea[1]*opod->c*opod->ec[2]-opod->a*opod->ea[2]*opod->c*opod->ec[1]); // -(a*ea[1]*c*ec[2]-a*ea[2]*c*ec[1])
    opod->D[1][1] =-(opod->a*opod->ea[2]*opod->c*opod->ec[0]-opod->a*opod->ea[0]*opod->c*opod->ec[2]); // -(a*ea[2]*c*ec[0]-a*ea[0]*c*ec[2])
    opod->D[2][1] =-(opod->a*opod->ea[0]*opod->c*opod->ec[1]-opod->a*opod->ea[1]*opod->c*opod->ec[0]); // -(a*ea[0]*c*ec[1]-a*ea[1]*c*ec[0])

    opod->D[0][2] =+(opod->a*opod->ea[1]*opod->b*opod->eb[2]-opod->a*opod->ea[2]*opod->b*opod->eb[1]); // a*ea[1]*b*eb[2]-b*ea[2]*b*eb[1]
    opod->D[1][2] =+(opod->a*opod->ea[2]*opod->b*opod->eb[0]-opod->a*opod->ea[0]*opod->b*opod->eb[2]); // a*ea[2]*b*eb[0]-b*ea[0]*b*eb[2]
    opod->D[2][2] =+(opod->a*opod->ea[0]*opod->b*opod->eb[1]-opod->a*opod->ea[1]*opod->b*opod->eb[0]); // a*ea[0]*b*eb[1]-b*ea[1]*b*eb[0]
}


void opo_setDetDinv(opo_data *opod) {
    opod->detDinv= opod->Dinv[0][0]*(opod->Dinv[1][1]*opod->Dinv[2][2]-opod->Dinv[2][1]*opod->Dinv[1][2])
                  -opod->Dinv[1][0]*(opod->Dinv[0][1]*opod->Dinv[2][2]-opod->Dinv[2][1]*opod->Dinv[0][2])
                  +opod->Dinv[2][0]*(opod->Dinv[0][1]*opod->Dinv[1][2]-opod->Dinv[1][1]*opod->Dinv[0][2]);
}

void opo_setDetD(opo_data *opod) {
    opod->detD= opod->D[0][0]*(opod->D[1][1]*opod->D[2][2]-opod->D[2][1]*opod->D[1][2])
               -opod->D[1][0]*(opod->D[0][1]*opod->D[2][2]-opod->D[2][1]*opod->D[0][2])
               +opod->D[2][0]*(opod->D[0][1]*opod->D[1][2]-opod->D[1][1]*opod->D[0][2]);
}

void opo_setQhat(opo_data *opod) {
    opod->Qhat[0]= opod->a*(opod->Rot_ea[0]*opod->Q[0]+opod->Rot_ea[1]*opod->Q[1]+opod->Rot_ea[2]*opod->Q[2]);
    opod->Qhat[1]= opod->b*(opod->Rot_eb[0]*opod->Q[0]+opod->Rot_eb[1]*opod->Q[1]+opod->Rot_eb[2]*opod->Q[2]);
    opod->Qhat[2]= opod->c*(opod->Rot_ec[0]*opod->Q[0]+opod->Rot_ec[1]*opod->Q[1]+opod->Rot_ec[2]*opod->Q[2]);
}

void opo_RotateVector(opo_rot *opoparam, double *vec, double* Rot_vec) {
    Rot_vec[0] = opoparam->RotationMatrix[0][0]*vec[0]
                +opoparam->RotationMatrix[1][0]*vec[1]
                +opoparam->RotationMatrix[2][0]*vec[2];
    Rot_vec[1] = opoparam->RotationMatrix[0][1]*vec[0]
                +opoparam->RotationMatrix[1][1]*vec[1]
                +opoparam->RotationMatrix[2][1]*vec[2];
    Rot_vec[2] = opoparam->RotationMatrix[0][2]*vec[0]
                +opoparam->RotationMatrix[1][2]*vec[1]
                +opoparam->RotationMatrix[2][2]*vec[2];
}

scalar opo_Modulus(double *vec) {
    return sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
}

void opo_RotateAxis(opo_data *opod) {
    opo_RotateVector(&opod->Rotation,opod->ea,opod->Rot_ea);
    opo_RotateVector(&opod->Rotation,opod->eb,opod->Rot_eb);
    opo_RotateVector(&opod->Rotation,opod->ec,opod->Rot_ec);
}

scalar opo_sinc(scalar x)
{
	if (fabs(x) <= 1e-4) {
		return 1.0-gsl_pow_2(x)/6.0
		          +gsl_pow_4(x)/120.0
		          -gsl_pow_6(x)/5040.0
		          +gsl_pow_8(x)/362880.0;
	} else {
		return sin(x)/x;
	}
}

scalar opo_J1x_x(scalar x)
{
	if (fabs(x) <= 1e-4) {
		return 0.5-gsl_pow_2(x)/16.0
		          +gsl_pow_4(x)/384.0
		          -gsl_pow_6(x)/18432.0
		          +gsl_pow_8(x)/1474560.0;
	} else {
		return gsl_sf_bessel_J1(x)/x;
	}
}

scalar opo_Fe(opo_data *opod) {
    scalar Qh;
    Qh = opo_Modulus(opod->Qhat);
    return 4*M_PI*opod->detDinv*(sin(Qh)-Qh*cos(Qh))/gsl_pow_3(Qh);
}

scalar opo_Fp(opo_data *opod) {
    scalar Qh;
    Qh = opo_Modulus(opod->Qhat);
    return 8*opod->detDinv*opo_sinc(opod->Qhat[0])
                          *opo_sinc(opod->Qhat[1])
                          *opo_sinc(opod->Qhat[2]);
}

scalar opo_Fc(opo_data *opod) {
    scalar Qxy;
    Qxy = sqrt(opod->Qhat[0]*opod->Qhat[0]+opod->Qhat[1]*opod->Qhat[1]);
    return 8*M_PI*opod->detDinv
                *opo_sinc(opod->Qhat[2])
                *opo_J1x_x(Qxy);
}
