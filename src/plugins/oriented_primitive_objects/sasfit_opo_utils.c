/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

scalar opo_Cot(scalar alpha) {
    return cos(alpha)/sin(alpha);
}
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
    int i,j;
    opo_rot *opoparam;
    opoparam = &opod->Rotation;

    for (i=0;i<3;i++) {
        s[i] = sin(opoparam->EulerAngles[i]);
        c[i] = cos(opoparam->EulerAngles[i]);
    }
    switch (opoparam->convention) {
// proper Euler Angles:
        case X1_Z2_X3 : {
                        opoparam->RotationMatrixT[0][0] = c[1];
                        opoparam->RotationMatrixT[1][0] = c[0]*s[1];
                        opoparam->RotationMatrixT[2][0] = s[0]*s[1];

                        opoparam->RotationMatrixT[0][1] = -c[2]*s[1];
                        opoparam->RotationMatrixT[1][1] =  c[0]*c[1]*c[2]-s[0]*s[2];
                        opoparam->RotationMatrixT[2][1] =  c[0]*s[2]     +c[1]*c[2]*s[0];

                        opoparam->RotationMatrixT[0][2] =  s[1]*s[2];
                        opoparam->RotationMatrixT[1][2] = -c[2]*s[0]-c[0]*c[1]*s[2];
                        opoparam->RotationMatrixT[2][2] =  c[0]*c[2]-c[1]*s[0]*s[2];
                        break;
                        }
        case X1_Y2_X3 : {
                        opoparam->RotationMatrixT[0][0] =  c[1];
                        opoparam->RotationMatrixT[1][0] =  s[0]*s[1];
                        opoparam->RotationMatrixT[2][0] = -c[0]*s[1];

                        opoparam->RotationMatrixT[0][1] =  s[1]*s[2];
                        opoparam->RotationMatrixT[1][1] =  c[0]*c[2]-c[1]*s[0]*s[2];
                        opoparam->RotationMatrixT[2][1] =  c[2]*s[0]+c[0]*c[1]*s[2];

                        opoparam->RotationMatrixT[0][2] =  c[2]*s[1];
                        opoparam->RotationMatrixT[1][2] = -c[0]*s[2]     -c[1]*c[2]*s[0];
                        opoparam->RotationMatrixT[2][2] =  c[0]*c[1]*c[2]-s[0]*s[2];
                        break;
                        }
        case Y1_X2_Y3 : {
                        opoparam->RotationMatrixT[0][0] =  c[0]*c[2]-c[1]*s[0]*s[2];
                        opoparam->RotationMatrixT[1][0] =  s[1]*s[2];
                        opoparam->RotationMatrixT[2][0] = -c[2]*s[0]-c[0]*c[1]*s[2];

                        opoparam->RotationMatrixT[0][1] =  s[0]*s[1];
                        opoparam->RotationMatrixT[1][1] =  c[1];
                        opoparam->RotationMatrixT[2][1] =  c[0]*s[1];

                        opoparam->RotationMatrixT[0][2] =  c[0]*s[2]+c[1]*c[2]*s[0];
                        opoparam->RotationMatrixT[1][2] = -c[2]*s[1];
                        opoparam->RotationMatrixT[2][2] =  c[0]*c[1]*c[2]-s[0]*s[2];
                        break;
                        }
        case Y1_Z2_Y3 : {
                        opoparam->RotationMatrixT[0][0] =  c[0]*c[1]*c[2]-s[0]*s[2];
                        opoparam->RotationMatrixT[1][0] =  c[2]*s[1];
                        opoparam->RotationMatrixT[2][0] = -c[0]*s[2]-c[1]*c[2]*s[0];

                        opoparam->RotationMatrixT[0][1] = -c[0]*s[1];
                        opoparam->RotationMatrixT[1][1] =  c[1];
                        opoparam->RotationMatrixT[2][1] =  s[0]*s[1];

                        opoparam->RotationMatrixT[0][2] =  c[2]*s[0]+c[0]*c[1]*s[2];
                        opoparam->RotationMatrixT[1][2] =  s[1]*s[2];
                        opoparam->RotationMatrixT[2][2] =  c[0]*c[2]-c[1]*s[0]*s[2];
                        break;
                        }
        case Z1_X2_Z3 : { // x-convention
                        opoparam->RotationMatrixT[0][0] = c[0]*c[2]-c[1]*s[0]*s[2];
                        opoparam->RotationMatrixT[1][0] = c[2]*s[0]+c[0]*c[1]*s[2];
                        opoparam->RotationMatrixT[2][0] = s[1]*s[2];

                        opoparam->RotationMatrixT[0][1] = -c[0]*s[2]     -c[1]*c[2]*s[0];
                        opoparam->RotationMatrixT[1][1] =  c[0]*c[1]*c[2]-s[0]*s[2];
                        opoparam->RotationMatrixT[2][1] =  c[2]*s[1];

                        opoparam->RotationMatrixT[0][2] =  s[0]*s[1];
                        opoparam->RotationMatrixT[1][2] = -c[0]*s[1];
                        opoparam->RotationMatrixT[2][2] =  c[2];
                        break;
                        }
        case Z1_Y2_Z3 : {
                        opoparam->RotationMatrixT[0][0] =  c[0]*c[1]*c[2]-s[0]*s[2];
                        opoparam->RotationMatrixT[1][0] =  c[0]*s[2]     +c[1]*c[2]*s[0];
                        opoparam->RotationMatrixT[2][0] = -c[2]*s[1];

                        opoparam->RotationMatrixT[0][1] = -c[2]*s[0]-c[0]*c[1]*s[2];
                        opoparam->RotationMatrixT[1][1] =  c[0]*c[2]-c[1]*s[0]*s[2];
                        opoparam->RotationMatrixT[2][1] =  s[1]*s[2];

                        opoparam->RotationMatrixT[0][2] = c[0]*s[1];
                        opoparam->RotationMatrixT[1][2] = s[0]*s[1];
                        opoparam->RotationMatrixT[2][2] = c[1];
                        break;
                        }
// Tait-Bryan angles:
        case X1_Z2_Y3 : {
                        opoparam->RotationMatrixT[0][0] =  c[1]*c[2];
                        opoparam->RotationMatrixT[1][0] =  s[0]*s[2]     +c[0]*c[2]*s[1];
                        opoparam->RotationMatrixT[2][0] =  c[2]*s[0]*s[1]-c[0]*s[2];

                        opoparam->RotationMatrixT[0][1] = -s[1];
                        opoparam->RotationMatrixT[1][1] =  c[0]*c[1];
                        opoparam->RotationMatrixT[2][1] =  c[1]*s[0];

                        opoparam->RotationMatrixT[0][2] = c[1]*s[2];
                        opoparam->RotationMatrixT[1][2] = c[0]*s[1]*s[2]-c[2]*s[0];
                        opoparam->RotationMatrixT[2][2] = c[0]*c[2]     +s[0]*s[1]*s[2];
                        break;
                        }
        case X1_Y2_Z3 : {
                        opoparam->RotationMatrixT[0][0] =  c[1]*c[2];
                        opoparam->RotationMatrixT[1][0] =  c[0]*s[2]+c[2]*s[0]*s[1];
                        opoparam->RotationMatrixT[2][0] =  s[0]*s[2]-c[0]*c[2]*s[1];

                        opoparam->RotationMatrixT[0][1] = -c[1]*s[2];
                        opoparam->RotationMatrixT[1][1] =  c[0]*c[2]-s[0]*s[1]*s[2];
                        opoparam->RotationMatrixT[2][1] =  c[2]*s[0]+c[0]*s[1]*s[2];

                        opoparam->RotationMatrixT[0][2] =  s[1];
                        opoparam->RotationMatrixT[1][2] = -c[1]*s[0];
                        opoparam->RotationMatrixT[2][2] =  c[0]*c[1];
                        break;

                        }
        case Y1_X2_Z3 : {
                        opoparam->RotationMatrixT[0][0] =  c[0]*c[2]     +s[0]*s[1]*s[2];
                        opoparam->RotationMatrixT[1][0] =  c[1]*s[2];
                        opoparam->RotationMatrixT[2][0] =  c[0]*s[1]*s[2]-c[2]*s[1];

                        opoparam->RotationMatrixT[0][1] =  c[2]*s[0]*s[1]-c[0]*s[2];
                        opoparam->RotationMatrixT[1][1] =  c[1]*c[2];
                        opoparam->RotationMatrixT[2][1] =  c[0]*c[2]*s[1]+s[0]*s[2];

                        opoparam->RotationMatrixT[0][2] =  c[1]*s[0];
                        opoparam->RotationMatrixT[1][2] = -s[1];
                        opoparam->RotationMatrixT[2][2] =  c[0]*c[1];
                        break;
                        }
        case Y1_Z2_X3 : {
                        opoparam->RotationMatrixT[0][0] =  c[0]*c[1];
                        opoparam->RotationMatrixT[1][0] =  s[1];
                        opoparam->RotationMatrixT[2][0] = -c[1]*s[0];

                        opoparam->RotationMatrixT[0][1] = s[0]*s[2]-c[0]*c[2]*s[1];
                        opoparam->RotationMatrixT[1][1] = c[1]*c[2];
                        opoparam->RotationMatrixT[2][1] = c[0]*s[2]+c[2]*s[0]*s[1];

                        opoparam->RotationMatrixT[0][2] =  c[2]*s[0]+c[0]*s[1]*s[2];
                        opoparam->RotationMatrixT[1][2] = -c[1]*s[2];
                        opoparam->RotationMatrixT[2][2] =  c[0]*c[2]-s[0]*s[1]*s[2];
                        break;
                        }
        case Z1_Y2_X3 : {
                        opoparam->RotationMatrixT[0][0] = c[0]*c[1];
                        opoparam->RotationMatrixT[1][0] = c[1]*s[0];
                        opoparam->RotationMatrixT[2][0] = -s[1];

                        opoparam->RotationMatrixT[0][1] = c[0]*s[1]*s[2]-c[2]*s[0];
                        opoparam->RotationMatrixT[1][1] = c[0]*c[2]     +s[0]*s[1]*s[2];
                        opoparam->RotationMatrixT[2][1] = c[1]*s[2];

                        opoparam->RotationMatrixT[0][2] = s[0]*s[2]     +c[0]*c[2]*s[1];
                        opoparam->RotationMatrixT[1][2] = c[2]*s[0]*s[1]-c[0]*s[2];
                        opoparam->RotationMatrixT[2][2] = c[1]*c[2];
                        break;
                        }
        case Z1_X2_Y3 : {
                        opoparam->RotationMatrixT[0][0] =  c[0]*c[2]-s[0]*s[1]*s[2];
                        opoparam->RotationMatrixT[1][0] =  c[2]*s[0]+c[0]*s[1]*s[2];
                        opoparam->RotationMatrixT[2][0] = -c[1]*s[2];

                        opoparam->RotationMatrixT[0][1] = -c[1]*s[0];
                        opoparam->RotationMatrixT[1][1] =  c[0]*c[1];
                        opoparam->RotationMatrixT[2][1] =  s[1];

                        opoparam->RotationMatrixT[0][2] = c[0]*s[2]+c[2]*s[0]*s[1];
                        opoparam->RotationMatrixT[1][2] = s[0]*s[2]-c[0]*c[2]*s[1];
                        opoparam->RotationMatrixT[2][2] = c[1]*c[2];
                        break;
                        }
        default:        {// x-convention
                        opoparam->RotationMatrixT[0][0] = c[0]*c[2]-c[1]*s[0]*s[2];
                        opoparam->RotationMatrixT[1][0] = c[2]*s[0]+c[0]*c[1]*s[2];
                        opoparam->RotationMatrixT[2][0] = s[1]*s[2];

                        opoparam->RotationMatrixT[0][1] = -c[0]*s[2]     -c[1]*c[2]*s[0];
                        opoparam->RotationMatrixT[1][1] =  c[0]*c[1]*c[2]-s[0]*s[2];
                        opoparam->RotationMatrixT[2][1] =  c[2]*s[1];

                        opoparam->RotationMatrixT[0][2] =  s[0]*s[1];
                        opoparam->RotationMatrixT[1][2] = -c[0]*s[1];
                        opoparam->RotationMatrixT[2][2] =  c[2];
                        }
    }
    for (i=0;i<3;i++) {
        for (j=0;j<3;j++) {
            opoparam->RotationMatrix[i][j] = opoparam->RotationMatrixT[j][i];
        }
    }
}
int opo_check_orthogonal(opo_data *opod){
    scalar a,b,c;
    int i;
    a=0;b=0;c=0;
    for (i=0;i<3;i++) {
        a=a+opod->ea[i]*opod->eb[i];
        b=b+opod->ea[i]*opod->ec[i];
        c=c+opod->eb[i]*opod->ec[i];
    }
    if (fabs(a)+fabs(b)+fabs(c)<=sasfit_eps_get_comp() ) return TRUE;
    return FALSE;
}

int opo_set_e(double *e, double ex, double ey, double ez) {
    scalar emod;
    emod = sqrt(ex*ex+ey*ey+ez*ez);
    if (fabs(emod) <= sasfit_eps_get_comp()) return 0;
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
    return gsl_hypot3(vec[0],vec[1],vec[2]);
}

void opo_RotateAxis(opo_data *opod) {
    opo_RotateVector(&opod->Rotation,opod->ea,opod->Rot_ea);
    opo_RotateVector(&opod->Rotation,opod->eb,opod->Rot_eb);
    opo_RotateVector(&opod->Rotation,opod->ec,opod->Rot_ec);
}

void opo_init (opo_data *opod) {
    opo_setRotationMatrix(opod);
    opo_RotateAxis(opod);
    opo_setDinv(opod);
    opo_setDetDinv(opod);
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
    Qxy = gsl_hypot(opod->Qhat[0],opod->Qhat[1]);
    return 4*M_PI*opod->detDinv
                *opo_sinc(opod->Qhat[2])
                *opo_J1x_x(Qxy);
}

scalar opo_Fprism3(opo_data *opod) {
    scalar Qx, Qy, Qz, Qx2_Qy2, Qx2_Qz2, Qy2_Qz2;
    Qx = opod->Qhat[0];
    Qy = opod->Qhat[1];
    Qz = opod->Qhat[2];
}

scalar opo_Fpyramid4_Re(opo_data *opod) {
    scalar Qx, Qy, Qz, K1, K2, K3, K4,q1,q2,q3,q4;
    sasfit_param * param;
    param = opod->param;
    Qx = opod->Qhat[0];
    Qy = opod->Qhat[1];
    Qz = opod->Qhat[2];
    q1 = 0.5*((Qx-Qy)*opo_Cot(TILT)+Qz);
    q2 = 0.5*((Qx-Qy)*opo_Cot(TILT)-Qz);
    q3 = 0.5*((Qx+Qy)*opo_Cot(TILT)+Qz);
    q4 = 0.5*((Qx+Qy)*opo_Cot(TILT)-Qz);
    K1 = opo_sinc(q1*H_R)*cos(q1*H_R)+opo_sinc(q2*H_R)*cos(-q2*H_R);
    K2 = opo_sinc(q1*H_R)*sin(q1*H_R)-opo_sinc(q2*H_R)*sin(-q2*H_R);
    K3 = opo_sinc(q3*H_R)*cos(q3*H_R)+opo_sinc(q4*H_R)*cos(-q4*H_R);
    K4 = opo_sinc(q3*H_R)*sin(q3*H_R)-opo_sinc(q4*H_R)*sin(-q4*H_R);

    if (tan(TILT)==0) return 0;
goto labelPy4Re;
    if (fabs(Qx)<= EPS_OH && fabs(Qy)> EPS_OH && fabs(Qz) > EPS_OH) {
        return (4*cos(TILT)*(-(H_R*gsl_pow_3(Qy)*gsl_pow_3(cos(TILT))*cos(H_R*Qz)*cos(Qy - H_R*Qy*opo_Cot(TILT))) +
            gsl_pow_2(Qz)*gsl_pow_3(sin(TILT))*(Qy*cos(Qy) - Qy*cos(H_R*Qz)*cos(Qy - H_R*Qy*opo_Cot(TILT)) + sin(Qy)) +
            gsl_pow_2(Qy)*gsl_pow_2(cos(TILT))*sin(TILT)*(-(Qy*cos(Qy)) + Qy*cos(H_R*Qz)*cos(Qy - H_R*Qy*opo_Cot(TILT)) + sin(Qy)) +
            Qy*Qz*cos(TILT)*cos(Qy - H_R*Qy*opo_Cot(TILT))*gsl_pow_2(sin(TILT))*(H_R*Qz*cos(H_R*Qz) - 2*sin(H_R*Qz))) -
            4*sin(TILT)*(cos(TILT)*cos(H_R*Qz)*(gsl_pow_2(Qy)*gsl_pow_2(cos(TILT)) + gsl_pow_2(Qz)*gsl_pow_2(sin(TILT))) +
            Qz*(H_R*cos(TILT) - sin(TILT))*(-(gsl_pow_2(Qy)*gsl_pow_2(cos(TILT))) + gsl_pow_2(Qz)*gsl_pow_2(sin(TILT)))*sin(H_R*Qz))*
            sin(Qy - H_R*Qy*opo_Cot(TILT)))/(Qy*gsl_pow_2(gsl_pow_2(Qy)*gsl_pow_2(cos(TILT)) - gsl_pow_2(Qz)*gsl_pow_2(sin(TILT))));
    } else if (fabs(Qx) > EPS_OH && fabs(Qy) <= EPS_OH && fabs(Qz) > EPS_OH) {
        return (4*cos(TILT)*(-(H_R*gsl_pow_3(Qx)*gsl_pow_3(cos(TILT))*cos(H_R*Qz)*cos(Qx - H_R*Qx*opo_Cot(TILT))) +
            gsl_pow_2(Qz)*gsl_pow_3(sin(TILT))*(Qx*cos(Qx) - Qx*cos(H_R*Qz)*cos(Qx - H_R*Qx*opo_Cot(TILT)) + sin(Qx)) +
            gsl_pow_2(Qx)*gsl_pow_2(cos(TILT))*sin(TILT)*(-(Qx*cos(Qx)) + Qx*cos(H_R*Qz)*cos(Qx - H_R*Qx*opo_Cot(TILT)) + sin(Qx)) +
            Qx*Qz*cos(TILT)*cos(Qx - H_R*Qx*opo_Cot(TILT))*gsl_pow_2(sin(TILT))*(H_R*Qz*cos(H_R*Qz) - 2*sin(H_R*Qz))) -
            4*sin(TILT)*(cos(TILT)*cos(H_R*Qz)*(gsl_pow_2(Qx)*gsl_pow_2(cos(TILT)) + gsl_pow_2(Qz)*gsl_pow_2(sin(TILT))) +
            Qz*(H_R*cos(TILT) - sin(TILT))*(-(gsl_pow_2(Qx)*gsl_pow_2(cos(TILT))) + gsl_pow_2(Qz)*gsl_pow_2(sin(TILT)))*sin(H_R*Qz))*
            sin(Qx - H_R*Qx*opo_Cot(TILT)))/(Qx*gsl_pow_2(gsl_pow_2(Qx)*gsl_pow_2(cos(TILT)) - gsl_pow_2(Qz)*gsl_pow_2(sin(TILT))));
    } else if (fabs(Qx) > EPS_OH && fabs(Qy)> EPS_OH && fabs(Qz) <= EPS_OH) {
        if (fabs(opo_Cot(TILT))<=EPS_OH) {
            return -((H_R*(H_R*(-2*TILT + M_PI)*Qy*cos(Qy)*sin(Qx) + (H_R*(-2*TILT + M_PI)*Qx*cos(Qx) - 4*sin(Qx))*sin(Qy)))/(Qx*Qy));
        } else if (fabs(Qx-Qy)<=EPS_OH) {
            return (-4*H_R*(Qx - 2*Qy)*Qy - 2*(Qx - Qy)*Qy*cos(2*Qy*(-1 + H_R*opo_Cot(TILT)))*(H_R - tan(TILT)) +
                (2*Qy*(-Qx + Qy)*cos(2*Qy) + (3*Qx - 5*Qy)*sin(2*Qy) + 3*Qx*sin(2*Qy*(-1 + H_R*opo_Cot(TILT))) +
                5*Qy*sin(2*Qy - 2*H_R*Qy*opo_Cot(TILT)))*tan(TILT))/(2.*gsl_pow_4(Qy));
        } else if (fabs(Qx+Qy)<=EPS_OH) {
            return (4*H_R*Qy*(Qx + 2*Qy) + 2*Qy*(Qx + Qy)*cos(2*Qy*(-1 + H_R*opo_Cot(TILT)))*(H_R - tan(TILT)) +
                2*Qy*(Qx + Qy)*cos(2*Qy)*tan(TILT) - (3*Qx + 5*Qy)*(sin(2*Qy) + sin(2*Qy*(-1 + H_R*opo_Cot(TILT))))*
                tan(TILT))/(2.*gsl_pow_4(Qy));
        } else {
            return (2*((Qx + Qy)*sin(Qx - Qy) + (Qx + Qy)*sin((Qx - Qy)*(-1 + H_R*opo_Cot(TILT))) -
                (Qx - Qy)*(sin(Qx + Qy) - sin(Qx + Qy - H_R*(Qx + Qy)*opo_Cot(TILT))))*tan(TILT))/
                (Qx*(Qx - Qy)*Qy*(Qx + Qy));
        }
    } else if  (fabs(Qx)<=EPS_OH && fabs(Qy)<= EPS_OH && fabs(Qz) > EPS_OH) {
        return (8*Qz*opo_Cot(TILT)*(1 + cos(H_R*Qz)*(-1 + H_R*opo_Cot(TILT))) + 4*
            (gsl_pow_2(Qz) - 2*H_R*gsl_pow_2(Qz)*opo_Cot(TILT) + (-2 + gsl_pow_2(H_R)*gsl_pow_2(Qz))*gsl_pow_2(opo_Cot(TILT)))*sin(H_R*Qz))/gsl_pow_3(Qz);
    } else if  (fabs(Qx) <= EPS_OH && fabs(Qy) > EPS_OH && fabs(Qz) <= EPS_OH) {
        if (fabs(opo_Cot(TILT))<=EPS_OH) {
            return (H_R*(4*sin(Qy) - H_R*(-2*TILT + M_PI)*(Qy*cos(Qy) + sin(Qy))))/Qy;
        } else {
            return (-4*(Qy*cos(Qy) + Qy*cos(Qy - H_R*Qy*opo_Cot(TILT))*(-1 + H_R*opo_Cot(TILT)) - sin(Qy) + sin(Qy - H_R*Qy*opo_Cot(TILT)))*
                tan(TILT))/gsl_pow_3(Qy);
        }
    } else if  (fabs(Qx) > EPS_OH && fabs(Qy) <= EPS_OH && fabs(Qz) <= EPS_OH) {
        if (fabs(opo_Cot(TILT))<=EPS_OH) {
            return (H_R*(4*sin(Qx) - H_R*(-2*TILT + M_PI)*(Qx*cos(Qx) + sin(Qx))))/Qx;
        } else {
            return (-4*(Qx*cos(Qx) + Qx*cos(Qx - H_R*Qx*opo_Cot(TILT))*(-1 + H_R*opo_Cot(TILT)) - sin(Qx) + sin(Qx - H_R*Qx*opo_Cot(TILT)))*
                tan(TILT))/gsl_pow_3(Qx);
        }
    } else if  (fabs(Qx) <= EPS_OH && fabs(Qy) <= EPS_OH && fabs(Qz) <= EPS_OH) {
        return (4*H_R*(3 + H_R*opo_Cot(TILT)*(-3 + H_R*opo_Cot(TILT))))/3.;
    } else {
        return (sin(TILT)*(-2*Qz*gsl_pow_2(cos(TILT))*(gsl_pow_2(Qx + Qy)*cos((Qx - Qy)*(-1 + H_R*opo_Cot(TILT))) -
            gsl_pow_2(Qx - Qy)*cos(Qx + Qy - H_R*(Qx + Qy)*opo_Cot(TILT)))*sin(TILT)*sin(H_R*Qz) -
            4*gsl_pow_2(Qz)*cos(TILT)*gsl_pow_2(sin(TILT))*
            (-(Qy*cos(Qy)*sin(Qx)) - Qx*cos(Qx)*sin(Qy) +
            (cos(H_R*Qz)*((Qx - Qy)*sin((Qx - Qy)*(-1 + H_R*opo_Cot(TILT))) -
            (Qx + Qy)*sin((Qx + Qy)*(-1 + H_R*opo_Cot(TILT)))))/2.) +
            4*gsl_pow_3(Qz)*gsl_pow_3(sin(TILT))*sin(H_R*Qz)*sin(Qx - H_R*Qx*opo_Cot(TILT))*sin(Qy - H_R*Qy*opo_Cot(TILT)) +
            2*(Qx - Qy)*(Qx + Qy)*gsl_pow_3(cos(TILT))*
            ((Qx + Qy)*sin(Qx - Qy) + (-Qx + Qy)*sin(Qx + Qy) +
            cos(H_R*Qz)*((Qx + Qy)*sin((Qx - Qy)*(-1 + H_R*opo_Cot(TILT))) +
            (Qx - Qy)*sin(Qx + Qy - H_R*(Qx + Qy)*opo_Cot(TILT))))))/
            (Qx*Qy*(gsl_pow_2(gsl_pow_2(Qx) - gsl_pow_2(Qy))*gsl_pow_4(cos(TILT)) -
            2*(gsl_pow_2(Qx) + gsl_pow_2(Qy))*gsl_pow_2(Qz)*gsl_pow_2(cos(TILT))*gsl_pow_2(sin(TILT)) +
            gsl_pow_4(Qz)*gsl_pow_4(sin(TILT))));
    }
labelPy4Re:
    return H_R/(Qx*Qy) * (cos(Qx-Qy)*K1+sin(Qx-Qy)*K2-cos(Qx+Qy)*K3-sin(Qx+Qy)*K4);
}

scalar opo_Fpyramid4_Im(opo_data *opod) {
    scalar Qx, Qy, Qz, K1, K2, K3, K4,q1,q2,q3,q4;
    sasfit_param * param;
    param = opod->param;
    Qx = opod->Qhat[0];
    Qy = opod->Qhat[1];
    Qz = opod->Qhat[2];
    q1 = 0.5*((Qx-Qy)*opo_Cot(TILT)+Qz);
    q2 = 0.5*((Qx-Qy)*opo_Cot(TILT)-Qz);
    q3 = 0.5*((Qx+Qy)*opo_Cot(TILT)+Qz);
    q4 = 0.5*((Qx+Qy)*opo_Cot(TILT)-Qz);
    K1 =  opo_sinc(q1*H_R)*sin(q1*H_R)+opo_sinc(q2*H_R)*sin(-q2*H_R);
    K2 = -opo_sinc(q1*H_R)*cos(q1*H_R)+opo_sinc(q2*H_R)*cos(-q2*H_R);
    K3 =  opo_sinc(q3*H_R)*sin(q3*H_R)+opo_sinc(q4*H_R)*sin(-q4*H_R);
    K4 = -opo_sinc(q3*H_R)*cos(q3*H_R)+opo_sinc(q4*H_R)*cos(-q4*H_R);

//goto labelPy4Im;
    if (tan(TILT)==0) return 0;
    if (fabs(Qx) <= EPS_OH && fabs(Qy)> EPS_OH && fabs(Qz) > EPS_OH) {
        return (-4*H_R*gsl_pow_3(Qy)*gsl_pow_4(cos(TILT))*cos(Qy - H_R*Qy*opo_Cot(TILT))*sin(H_R*Qz) +
            4*gsl_pow_3(Qz)*gsl_pow_4(sin(TILT))*(sin(Qy) - cos(H_R*Qz)*sin(Qy - H_R*Qy*opo_Cot(TILT))) +
            Qy*Qz*gsl_pow_2(sin(2*TILT))*(-2*cos(Qy) - Qy*sin(Qy) + cos(Qy - H_R*Qy*opo_Cot(TILT))*(2*cos(H_R*Qz) + H_R*Qz*sin(H_R*Qz)) +
            Qy*cos(H_R*Qz)*sin(Qy - H_R*Qy*opo_Cot(TILT))) + 4*gsl_pow_2(Qz)*cos(TILT)*gsl_pow_3(sin(TILT))*
            (-(Qy*cos(Qy - H_R*Qy*opo_Cot(TILT))*sin(H_R*Qz)) + (H_R*Qz*cos(H_R*Qz) - sin(H_R*Qz))*sin(Qy - H_R*Qy*opo_Cot(TILT))) +
            4*gsl_pow_2(Qy)*gsl_pow_3(cos(TILT))*sin(TILT)*(Qy*cos(Qy - H_R*Qy*opo_Cot(TILT))*sin(H_R*Qz) -
            (H_R*Qz*cos(H_R*Qz) + sin(H_R*Qz))*sin(Qy - H_R*Qy*opo_Cot(TILT))))/
            (Qy*gsl_pow_2(gsl_pow_2(Qy)*gsl_pow_2(cos(TILT)) - gsl_pow_2(Qz)*gsl_pow_2(sin(TILT))));
    } else if (fabs(Qx) > EPS_OH && fabs(Qy) <= EPS_OH && fabs(Qz) > EPS_OH) {
        return (-4*H_R*gsl_pow_3(Qx)*gsl_pow_4(cos(TILT))*cos(Qx - H_R*Qx*opo_Cot(TILT))*sin(H_R*Qz) +
            4*gsl_pow_3(Qz)*gsl_pow_4(sin(TILT))*(sin(Qx) - cos(H_R*Qz)*sin(Qx - H_R*Qx*opo_Cot(TILT))) +
            Qx*Qz*gsl_pow_2(sin(2*TILT))*(-2*cos(Qx) - Qx*sin(Qx) + cos(Qx - H_R*Qx*opo_Cot(TILT))*(2*cos(H_R*Qz) + H_R*Qz*sin(H_R*Qz)) +
            Qx*cos(H_R*Qz)*sin(Qx - H_R*Qx*opo_Cot(TILT))) + 4*gsl_pow_2(Qz)*cos(TILT)*gsl_pow_3(sin(TILT))*
            (-(Qx*cos(Qx - H_R*Qx*opo_Cot(TILT))*sin(H_R*Qz)) + (H_R*Qz*cos(H_R*Qz) - sin(H_R*Qz))*sin(Qx - H_R*Qx*opo_Cot(TILT))) +
            4*gsl_pow_2(Qx)*gsl_pow_3(cos(TILT))*sin(TILT)*(Qx*cos(Qx - H_R*Qx*opo_Cot(TILT))*sin(H_R*Qz) -
            (H_R*Qz*cos(H_R*Qz) + sin(H_R*Qz))*sin(Qx - H_R*Qx*opo_Cot(TILT))))/
            (Qx*gsl_pow_2(gsl_pow_2(Qx)*gsl_pow_2(cos(TILT)) - gsl_pow_2(Qz)*gsl_pow_2(sin(TILT))));
    } else if (fabs(Qx) > EPS_OH && fabs(Qy) > EPS_OH && fabs(Qz) <= EPS_OH) {
        if (fabs(opo_Cot(TILT))<=EPS_OH) {
            return (-2*gsl_pow_2(H_R)*Qz*(H_R*(-2*TILT + M_PI)*Qy*cos(Qy)*sin(Qx) +
                (H_R*(-2*TILT + M_PI)*Qx*cos(Qx) - 3*sin(Qx))*sin(Qy)))/(3.*Qx*Qy);
        } else if (fabs(Qx-Qy)<=EPS_OH) {
            return (Qz*(-(gsl_pow_2(H_R)*(Qx - 2*Qy)*gsl_pow_2(Qy)) -
                H_R*(Qx - Qy)*gsl_pow_2(Qy)*cos(2*Qy*(-1 + H_R*opo_Cot(TILT)))*(H_R - tan(TILT)) +
                H_R*Qy*(-2*Qx + 3*Qy)*sin(2*Qy - 2*H_R*Qy*opo_Cot(TILT))*tan(TILT) +
                sin(H_R*Qy*opo_Cot(TILT))*(Qy*(-Qx + Qy)*cos(Qy*(2 - H_R*opo_Cot(TILT))) +
                (2*Qx - 3*Qy)*sin(Qy*(2 - H_R*opo_Cot(TILT))))*gsl_pow_2(tan(TILT))))/gsl_pow_5(Qy);
        } else if (fabs(Qx+ Qy)<=EPS_OH) {
            return (Qz*(gsl_pow_2(H_R)*gsl_pow_2(Qy)*(Qx + 2*Qy) + H_R*gsl_pow_2(Qy)*(Qx + Qy)*cos(2*Qy*(-1 + H_R*opo_Cot(TILT)))*
                (H_R - tan(TILT)) + H_R*Qy*(2*Qx + 3*Qy)*sin(2*Qy - 2*H_R*Qy*opo_Cot(TILT))*tan(TILT) +
                sin(H_R*Qy*opo_Cot(TILT))*(Qy*(Qx + Qy)*cos(Qy*(2 - H_R*opo_Cot(TILT))) -
                (2*Qx + 3*Qy)*sin(Qy*(2 - H_R*opo_Cot(TILT))))*gsl_pow_2(tan(TILT))))/gsl_pow_5(Qy);
        } else {
            return (2*Qz*tan(TILT)*(H_R*(Qx - Qy)*gsl_pow_2(Qx + Qy)*sin((Qx - Qy)*(-1 + H_R*opo_Cot(TILT))) +
                H_R*gsl_pow_2(Qx - Qy)*(Qx + Qy)*sin(Qx + Qy - H_R*(Qx + Qy)*opo_Cot(TILT)) +
                (-4*Qx*Qy*cos(Qx)*cos(Qy) + gsl_pow_2(Qx + Qy)*cos((Qx - Qy)*(-1 + H_R*opo_Cot(TILT))) -
                gsl_pow_2(Qx)*cos((Qx + Qy)*(-1 + H_R*opo_Cot(TILT))) + 2*Qx*Qy*cos((Qx + Qy)*(-1 + H_R*opo_Cot(TILT))) -
                gsl_pow_2(Qy)*cos((Qx + Qy)*(-1 + H_R*opo_Cot(TILT))) - 2*gsl_pow_2(Qx)*sin(Qx)*sin(Qy) -
                2*gsl_pow_2(Qy)*sin(Qx)*sin(Qy))*tan(TILT)))/(Qx*gsl_pow_2(Qx - Qy)*Qy*gsl_pow_2(Qx + Qy));
        }
    } else if  (fabs(Qx) <= EPS_OH && fabs(Qy) <= EPS_OH && fabs(Qz) > EPS_OH) {
        return (4*(gsl_pow_2(Qz) - 2*gsl_pow_2(opo_Cot(TILT)) - cos(H_R*Qz)*(gsl_pow_2(Qz) - 2*H_R*gsl_pow_2(Qz)*opo_Cot(TILT) +
            (-2 + gsl_pow_2(H_R)*gsl_pow_2(Qz))*gsl_pow_2(opo_Cot(TILT))) + 2*Qz*opo_Cot(TILT)*(-1 + H_R*opo_Cot(TILT))*sin(H_R*Qz)))/gsl_pow_3(Qz);
    } else if  (fabs(Qx) <= EPS_OH && fabs(Qy) > EPS_OH && fabs(Qz) <= EPS_OH) {
        if (fabs(opo_Cot(TILT))<=EPS_OH) {
            return (2*gsl_pow_2(H_R)*Qz*(3*sin(Qy) - H_R*(-2*TILT + M_PI)*(Qy*cos(Qy) + sin(Qy))))/(3.*Qy);
        } else {
            return (-4*Qz*(2*cos(Qy) + cos(Qy - H_R*Qy*opo_Cot(TILT))*(-2 + H_R*gsl_pow_2(Qy)*opo_Cot(TILT)*(-1 + H_R*opo_Cot(TILT))) +
                Qy*(sin(Qy) + (-1 + 2*H_R*opo_Cot(TILT))*sin(Qy - H_R*Qy*opo_Cot(TILT))))*gsl_pow_2(tan(TILT)))/gsl_pow_4(Qy);
        }
    } else if  (fabs(Qx) > EPS_OH && fabs(Qy) <= EPS_OH && fabs(Qz) <= EPS_OH) {
        if (fabs(opo_Cot(TILT)) <= EPS_OH) {
            return (2*gsl_pow_2(H_R)*Qz*(3*sin(Qx) - H_R*(-2*TILT + M_PI)*(Qx*cos(Qx) + sin(Qx))))/(3.*Qx);
        } else {
            return (-4*(Qx*cos(Qx) + Qx*cos(Qx - H_R*Qx*opo_Cot(TILT))*(-1 + H_R*opo_Cot(TILT)) - sin(Qx) + sin(Qx - H_R*Qx*opo_Cot(TILT)))*
                tan(TILT))/gsl_pow_3(Qx);
        }
    } else if  (fabs(Qx) <= EPS_OH && fabs(Qy) <= EPS_OH && fabs(Qz) <= EPS_OH) {
        return (4*H_R*(3 + H_R*opo_Cot(TILT)*(-3 + H_R*opo_Cot(TILT))))/3.;
    } else {
        return (-2*sin(TILT)*(-2*Qz*gsl_pow_2(cos(TILT))*sin(TILT)*
            (sin(Qx)*(-(cos(Qy)*cos(H_R*Qz)*(-2*Qx*Qy*cos(H_R*Qy*opo_Cot(TILT))*sin(H_R*Qx*opo_Cot(TILT)) +
            (gsl_pow_2(Qx) + gsl_pow_2(Qy))*cos(H_R*Qx*opo_Cot(TILT))*sin(H_R*Qy*opo_Cot(TILT)))) +
            sin(Qy)*(-gsl_pow_2(Qx) - gsl_pow_2(Qy) +
            cos(H_R*Qz)*((gsl_pow_2(Qx) + gsl_pow_2(Qy))*cos(H_R*Qx*opo_Cot(TILT))*cos(H_R*Qy*opo_Cot(TILT)) +
            2*Qx*Qy*sin(H_R*Qx*opo_Cot(TILT))*sin(H_R*Qy*opo_Cot(TILT))))) +
            cos(Qx)*(-(cos(H_R*Qz)*sin(Qy)*((gsl_pow_2(Qx) + gsl_pow_2(Qy))*cos(H_R*Qy*opo_Cot(TILT))*
            sin(H_R*Qx*opo_Cot(TILT)) - 2*Qx*Qy*cos(H_R*Qx*opo_Cot(TILT))*sin(H_R*Qy*opo_Cot(TILT)))) +
            cos(Qy)*(-2*Qx*Qy + cos(H_R*Qz)*(2*Qx*Qy*cos(H_R*Qx*opo_Cot(TILT))*cos(H_R*Qy*opo_Cot(TILT)) +
            (gsl_pow_2(Qx) + gsl_pow_2(Qy))*sin(H_R*Qx*opo_Cot(TILT))*sin(H_R*Qy*opo_Cot(TILT)))))) +
            2*gsl_pow_3(Qz)*gsl_pow_3(sin(TILT))*(-(sin(Qx)*sin(Qy)) +
            cos(H_R*Qz)*sin(Qx - H_R*Qx*opo_Cot(TILT))*sin(Qy - H_R*Qy*opo_Cot(TILT))) -
            (gsl_pow_2(Qx) - gsl_pow_2(Qy))*gsl_pow_3(cos(TILT))*sin(H_R*Qz)*
            ((Qx + Qy)*sin((Qx - Qy)*(-1 + H_R*opo_Cot(TILT))) + (Qx - Qy)*sin(Qx + Qy - H_R*(Qx + Qy)*opo_Cot(TILT))) +
            gsl_pow_2(Qz)*cos(TILT)*gsl_pow_2(sin(TILT))*sin(H_R*Qz)*
            ((Qx - Qy)*sin((Qx - Qy)*(-1 + H_R*opo_Cot(TILT))) + (Qx + Qy)*sin(Qx + Qy - H_R*(Qx + Qy)*opo_Cot(TILT)))))/
            (Qx*Qy*((Qx - Qy)*cos(TILT) - Qz*sin(TILT))*((Qx + Qy)*cos(TILT) - Qz*sin(TILT))*
            ((Qx - Qy)*cos(TILT) + Qz*sin(TILT))*((Qx + Qy)*cos(TILT) + Qz*sin(TILT)));
    }
labelPy4Im:
    return H_R/(Qx*Qy) * (cos(Qx-Qy)*K1+sin(Qx-Qy)*K2-cos(Qx+Qy)*K3-sin(Qx+Qy)*K4);
}

scalar opo_Fpyramid4(opo_data *opod) {
    return gsl_hypot(opo_Fpyramid4_Re(opod),opo_Fpyramid4_Im(opod));
}

scalar Fcone_kernel_Re(scalar z, sasfit_param *param){
    scalar q_parallel,Rz;
    q_parallel = gsl_hypot(QQX,QQY);
    Rz = 1-z*opo_Cot(TILT);
    return 2*M_PI*gsl_pow_2(Rz)*opo_J1x_x(q_parallel*Rz)*cos(QQZ*z);
}

scalar Fcone_kernel_Im(scalar z, sasfit_param *param){
    scalar q_parallel,Rz;
    q_parallel = gsl_hypot(QQX,QQY);
    Rz = 1-z*opo_Cot(TILT);
    return 2*M_PI*gsl_pow_2(Rz)*opo_J1x_x(q_parallel*Rz)*sin(QQZ*z);
}

scalar opo_Fcone(opo_data *opod) {
    scalar FconeRe, FconeIm;
    sasfit_param * param;
    param = opod->param;
    QQX = opod->Qhat[0];
    QQY = opod->Qhat[1];
    QQZ = opod->Qhat[2];
    if (tan(TILT)==0) return 0;
    FconeRe = sasfit_integrate(0,H_R,&Fcone_kernel_Re,param);
    FconeIm = sasfit_integrate(0,H_R,&Fcone_kernel_Im,param);
    return gsl_hypot(FconeRe,FconeIm);
}

scalar Fcone6_kernel_Re(scalar z, sasfit_param *param){
    scalar Rz,Rz2,Rz4,Rz6,sqrt3,qx2,qx4,qy2,qy4,qz2;
    Rz = 1-z*opo_Cot(TILT);
    Rz2=Rz*Rz;
    Rz4=Rz2*Rz2;
    Rz6=Rz4*Rz2;
    qx2=QQX*QQX;
    qx4=qx2*qx2;
    qy2=QQY*QQY;
    qy4=qy2*qy2;
    qz2=QQZ*QQZ;
    sqrt3=sqrt(3.0);
    if (fabs(QQX)<gsl_pow_2(EPS_OH) && fabs(QQY)<gsl_pow_2(EPS_OH)) {
        return 2*sqrt3*Rz2*cos(QQZ*z);
    } else if (fabs(QQX)<EPS_OH && fabs(QQY)<EPS_OH) {
        return ((Rz2*(24494400 - 3402000*(qx2 + qy2)*Rz2 + 158760*gsl_pow_2(qx2 + qy2)*Rz4 -
            135*(73*qx4*qy2 + 93*qx2*qy4)*Rz6 + 319*qx4*qy4*Rz4*Rz4))/
            (4.0824e6*sqrt3))*cos(QQZ*z);
    } else if (fabs(QQX-sqrt3*QQY)<EPS_OH) {
        return ((6*QQX*QQY*Rz2*cos(2*QQY*Rz) + sqrt3*(5 - (5 + 6*qy2*Rz2)*cos(2*QQY*Rz) + 4*QQY*Rz*sin(2*QQY*Rz)) -
            6*QQX*Rz*sin(QQY*Rz)*opo_sinc(QQY*Rz))/(6.*qy2))*cos(QQZ*z);
    } else if (fabs(QQX+sqrt3*QQY)<EPS_OH) {
        return ((3*QQX - (3*QQX + 5*sqrt3*QQY + 6*qy2*(QQX + sqrt3*QQY)*Rz2)*cos(2*QQY*Rz) + sqrt3*QQY*(5 + 4*QQY*Rz*sin(2*QQY*Rz)))/
            (6.*QQY*qy2))*cos(QQZ*z);
    } else {
        return 4.*sqrt3/(3.*qy2-qx2)* (qy2*Rz2*opo_sinc(QQX*Rz/sqrt3)*opo_sinc(QQY*Rz)+cos(2./sqrt3*QQX*Rz)-cos(QQY*Rz)*cos(QQX*Rz/sqrt3))*cos(QQZ*z);
    }
}

scalar Fcone6_kernel_Im(scalar z, sasfit_param *param){
    scalar Rz,Rz2,Rz4,Rz6,sqrt3,qx2,qx4,qy2,qy4,qz2;
    Rz = 1-z*opo_Cot(TILT);
    Rz2=Rz*Rz;
    Rz4=Rz2*Rz2;
    Rz6=Rz4*Rz2;
    qx2=QQX*QQX;
    qx4=qx2*qx2;
    qy2=QQY*QQY;
    qy4=qy2*qy2;
    qz2=QQZ*QQZ;
    sqrt3=sqrt(3);
    if (fabs(QQX)<gsl_pow_2(EPS_OH) && fabs(QQY)<gsl_pow_2(EPS_OH)) {
        return 2*sqrt3*Rz2*sin(QQZ*z);
    } else if (fabs(QQX)<EPS_OH && fabs(QQY)<EPS_OH) {
        return ((Rz2*(24494400 - 3402000*(qx2 + qy2)*Rz2 + 158760*gsl_pow_2(qx2 + qy2)*Rz4 -
            135*(73*qx4*qy2 + 93*qx2*qy4)*Rz6 + 319*qx4*qy4*Rz4*Rz4))/
            (4.0824e6*sqrt3))*sin(QQZ*z);
    } else if (fabs(QQX-sqrt3*QQY)<EPS_OH) {
        return ((6*QQX*QQY*Rz2*cos(2*QQY*Rz) + sqrt3*(5 - (5 + 6*qy2*Rz2)*cos(2*QQY*Rz) + 4*QQY*Rz*sin(2*QQY*Rz)) -
            6*QQX*Rz*sin(QQY*Rz)*opo_sinc(QQY*Rz))/(6.*qy2))*sin(QQZ*z);
    } else if (fabs(QQX+sqrt3*QQY)<EPS_OH) {
        return ((3*QQX - (3*QQX + 5*sqrt3*QQY + 6*qy2*(QQX + sqrt3*QQY)*Rz2)*cos(2*QQY*Rz) + sqrt3*QQY*(5 + 4*QQY*Rz*sin(2*QQY*Rz)))/
            (6.*QQY*qy2))*sin(QQZ*z);
    } else {
        return 4.*sqrt3/(3.*qy2-qx2)* (qy2*Rz2*opo_sinc(QQX*Rz/sqrt3)*opo_sinc(QQY*Rz)+cos(2./sqrt3*QQX*Rz)-cos(QQY*Rz)*cos(QQX*Rz/sqrt3))*sin(QQZ*z);
    }
}


scalar opo_Fcone6(opo_data *opod) {
    scalar Fcone6Re, Fcone6Im;
    sasfit_param * param;
    param = opod->param;
    QQX = opod->Qhat[0];
    QQY = opod->Qhat[1];
    QQZ = opod->Qhat[2];
    if (tan(TILT)==0) return 0;
    Fcone6Re = sasfit_integrate(0,H_R,&Fcone6_kernel_Re,param);
    Fcone6Im = sasfit_integrate(0,H_R,&Fcone6_kernel_Im,param);
    return gsl_hypot(Fcone6Re,Fcone6Im);
}


scalar opo_Fpyramid(opo_data *opod) {
    scalar Qx, Qy, Qz, Qx2_Qy2, Qx2_Qz2, Qy2_Qz2;
    Qx = opod->Qhat[0];
    Qy = opod->Qhat[1];
    Qz = opod->Qhat[2];
}

scalar Ftetrahedron_kernel_Re(opo_data *opod) {
    scalar Qx,Qx2,Qy,Qy2,Qz,Qz2,L,q1,q2,q3,qzR,sqrt3;
    sasfit_param * param;
    param = opod->param;
    Qx = opod->Qhat[0];
    Qx2=Qx*Qx;
    Qy = opod->Qhat[1];
    Qy2=Qy*Qy;
    Qz = opod->Qhat[2];
    Qz2=Qz*Qz;
    sqrt3=sqrt(3.0);
    qzR=Qz*tan(TILT)/sqrt3;
    q1 = 0.5*((sqrt3*Qx-Qy)*opo_Cot(TILT)-Qz);
    q2 = 0.5*((sqrt3*Qx+Qy)*opo_Cot(TILT)+Qz);
    q3 = 0.5*(2*Qy*opo_Cot(TILT)-Qz);
    L = 2*tan(TILT)/sqrt3-H_R;

    if (fabs(Qx)<EPS_OH && fabs(Qy)<EPS_OH && fabs(Qz)<EPS_OH) {
        return H_R*(sqrt3 + H_R*opo_Cot(TILT)*(-3 + sqrt3*H_R*opo_Cot(TILT)));
    }
    if (fabs(Qx)<EPS_OH) {
        if (fabs(Qy)<EPS_OH) {
            return (gsl_pow_4(1.0/sin(TILT))*gsl_pow_2(sin(2*TILT))*(6*Qz*cos(H_R*Qz)*(sqrt3*H_R - tan(TILT)) + 6*Qz*tan(TILT) +
                sin(H_R*Qz)*(3*sqrt3*(-2 + gsl_pow_2(H_R)*gsl_pow_2(Qz)) + gsl_pow_2(Qz)*tan(TILT)*(-6*H_R + sqrt3*tan(TILT)))))/(4.*gsl_pow_3(Qz));
        } else {
            return ((4*sqrt3*cos((2*Qy)/sqrt3 + (H_R*Qz)/2. - H_R*Qy*opo_Cot(TILT))*sin(TILT)*sin((H_R*(Qz - 2*Qy*opo_Cot(TILT)))/2.))/
                (2*Qy*cos(TILT) - Qz*sin(TILT)) + (-6*sqrt3*H_R*Qy*cos(Qy/sqrt3 - H_R*Qz - H_R*Qy*opo_Cot(TILT))*opo_Cot(TILT)*(Qz + Qy*opo_Cot(TILT)) +
                4*sin((H_R*(Qz + Qy*opo_Cot(TILT)))/2.)*(sqrt3*cos((-2*sqrt3*Qy + 3*H_R*Qz + 3*H_R*Qy*opo_Cot(TILT))/6.)*(Qz + 4*Qy*opo_Cot(TILT)) +
                3*Qy*(Qz + Qy*opo_Cot(TILT))*sin((2*sqrt3*Qy - 3*H_R*Qz - 3*H_R*Qy*opo_Cot(TILT))/6.)))/gsl_pow_2(Qz + Qy*opo_Cot(TILT)))/(3.*Qy2);
        }
    } else if (fabs(Qx-sqrt3*Qy)<EPS_OH) {
        return -999999.;
    } else if (fabs(Qx+sqrt3*Qy)<EPS_OH) {
        return -9999999.;
    } else {
        return sqrt3*H_R/(Qx*(Qx2-3*Qy2))*(
                    -( Qx+sqrt3*Qy)*opo_sinc(q1*H_R) * (cos( qzR+q1*L))
                    +(-Qx+sqrt3*Qy)*opo_sinc(q2*H_R) * (cos( qzR-q2*L))
                    +2*Qx          *opo_sinc(q3*H_R) * (cos( qzR+q3*L))
                    );
    }
}

scalar Ftetrahedron_kernel_Im(opo_data *opod) {
    scalar Qx,Qx2,Qy,Qy2,Qz,Qz2,L,q1,q2,q3,qzR,sqrt3;
    sasfit_param * param;
    param = opod->param;
    Qx = opod->Qhat[0];
    Qx2=Qx*Qx;
    Qy = opod->Qhat[1];
    Qy2=Qy*Qy;
    Qz = opod->Qhat[2];
    Qz2=Qz*Qz;
    sqrt3=sqrt(3.0);
    qzR=Qz*tan(TILT)/sqrt3;
    q1 = 0.5*((sqrt3*Qx-Qy)*opo_Cot(TILT)-Qz);
    q2 = 0.5*((sqrt3*Qx+Qy)*opo_Cot(TILT)+Qz);
    q3 = 0.5*(2*Qy*opo_Cot(TILT)-Qz);
    L = 2*tan(TILT)/sqrt3-H_R;

    if (fabs(Qx)<EPS_OH && fabs(Qy)<EPS_OH && fabs(Qz)<EPS_OH) {
        return (gsl_pow_2(H_R)*Qz*(2*sqrt3 + H_R*opo_Cot(TILT)*(-8 + 3*sqrt3*H_R*opo_Cot(TILT))))/4.;
    }
    if (fabs(Qx)<EPS_OH) {
        if (fabs(Qy)<EPS_OH) {
            return -(gsl_pow_4(1.0/sin(TILT))*gsl_pow_2(sin(2*TILT))*(6*Qz*sin(H_R*Qz)*(-(sqrt3*H_R) + tan(TILT)) + sqrt3*(6 - gsl_pow_2(Qz)*gsl_pow_2(tan(TILT))) +
                cos(H_R*Qz)*(3*sqrt3*(-2 + gsl_pow_2(H_R)*gsl_pow_2(Qz)) + gsl_pow_2(Qz)*tan(TILT)*(-6*H_R + sqrt3*tan(TILT)))))/(4.*gsl_pow_3(Qz));
        } else {
            return (2*cos(TILT)*(-3*H_R*Qy*cos(TILT)*cos((-2*sqrt3*Qy + 3*H_R*Qz + 3*H_R*Qy*opo_Cot(TILT))/6.)*sin((H_R*(Qz + Qy*opo_Cot(TILT)))/2.) +
                sin(TILT)*(4*cos(Qy/sqrt3) - 4*cos(Qy/sqrt3 - H_R*Qz - H_R*Qy*opo_Cot(TILT)) + 3*H_R*Qz*sin(Qy/sqrt3 - H_R*Qz - H_R*Qy*opo_Cot(TILT)))))/
                (sqrt3*Qy*gsl_pow_2(Qy*cos(TILT) + Qz*sin(TILT))) +
                (2*sin(TILT)*((sqrt3*(cos((2*Qy)/sqrt3) - cos((2*Qy)/sqrt3 + H_R*Qz - 2*H_R*Qy*opo_Cot(TILT))))/(2*Qy*cos(TILT) - Qz*sin(TILT)) +
                (2*sin((H_R*(Qz + Qy*opo_Cot(TILT)))/2.)*(3*gsl_pow_2(Qy)*cos(TILT)*cos((-2*sqrt3*Qy + 3*H_R*Qz + 3*H_R*Qy*opo_Cot(TILT))/6.) +
                Qz*sin(TILT)*(3*Qy*cos((-2*sqrt3*Qy + 3*H_R*Qz + 3*H_R*Qy*opo_Cot(TILT))/6.) +
                sqrt3*sin((-2*sqrt3*Qy + 3*H_R*Qz + 3*H_R*Qy*opo_Cot(TILT))/6.))))/gsl_pow_2(Qy*cos(TILT) + Qz*sin(TILT))))/(3.*Qy2) +
                (sqrt3*H_R*(sin(Qy/sqrt3) + sin(Qy/sqrt3 - H_R*Qz - H_R*Qy*opo_Cot(TILT))))/gsl_pow_2(Qy + Qz*tan(TILT));
        }
    }

    return sqrt3*H_R/(Qx*(Qx2-3*Qy2))*(
                    -( Qx+sqrt3*Qy)*opo_sinc(q1*H_R) * (sin(qzR+q1*L))
                    +(-Qx+sqrt3*Qy)*opo_sinc(q2*H_R) * (sin(qzR-q2*L))
                    +2*Qx          *opo_sinc(q3*H_R) * (sin(qzR+q3*L))
                    );
}

scalar opo_Ftetrahedron(opo_data *opod) {
    scalar FtetrahedronRe, FtetrahedronIm;
    sasfit_param * param;
    param = opod->param;
    QQX = opod->Qhat[0];
    QQY = opod->Qhat[1];
    QQZ = opod->Qhat[2];
    if (tan(TILT)==0) return 0;
    FtetrahedronRe = Ftetrahedron_kernel_Re(opod);
    FtetrahedronIm = Ftetrahedron_kernel_Im(opod);
    return gsl_hypot(FtetrahedronRe,FtetrahedronIm);
}

scalar opo_Fcubooctahedron(opo_data *opod) {
    scalar Qx, Qy, Qz, Qx2_Qy2, Qx2_Qz2, Qy2_Qz2;
    Qx = opod->Qhat[0];
    Qy = opod->Qhat[1];
    Qz = opod->Qhat[2];
}
scalar ri(scalar theta, scalar alpha, scalar m, scalar n1, scalar n2, scalar n3, scalar a, scalar b) {
    return pow(pow(fabs(cos((m*(theta-alpha)/4.)/a)),n2) + pow(fabs(sin((m*(theta-alpha)/4.)/b)),n3),-1/n1);
}

scalar ratri(scalar theta, scalar alpha, scalar m, scalar n1, scalar n2, scalar n3, scalar a, scalar b) {
    scalar W, U, V;
    U = fabs(cos(m*(theta-alpha)/4.)/a);
    V = fabs(sin(m*(theta-alpha)/4.)/b);
    W = U/(n2+(1-n2)*U) + V/(n3+(1-n3)*V);
    return  pow(2,-1./n1)*(1./(W*n1)+1-1./n1); // pow(2,-1./n1)*
}

int opo_Fsth_cub_dtp(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
    scalar r,theta,phi,r1,r2,Ir2CosQR, Ir2SinQR, Asth,Bsth;
	sasfit_param * param;
	param = (sasfit_param *) pam;
	if ((ndim < 2) || (fdim < 2)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	theta = x[0];
	phi   = x[1];
	r1=ri(theta,FALPHA,FMM,FN1,FN2,FN3,FAA,FBB);
	r2=ri(phi,SALPHA+theta*SGAMMA,SMM,SN1,SN2,SN3,SAA,SBB);
	Asth = QQZ*PITCH*theta/(2*M_PI) +
           QQX*RADIUS*r1*cos(theta)+
           QQY*RADIUS*r1*sin(theta);
	Bsth = (QQX*r1*r2*cos(theta)*cos(phi) +
            QQY*r1*r2*sin(theta)*cos(phi) +
            QQZ*r2*sin(phi));

    if (fabs(Bsth) > 1e-2) {
        Ir2CosQR = (gsl_pow_2(r1*r2)*(Bsth*RADIUS*(- cos(Asth)
                                                   + cos(Asth+Bsth)
                                                   + Bsth*sin(Asth+Bsth))
                                        + r2*cos(phi)*(  2*Bsth*cos(Asth+Bsth)
                                                        + 2*sin(Asth)
                                                        + (gsl_pow_2(Bsth)-2)*sin(Asth+Bsth))))/gsl_pow_3(Bsth);
        Ir2SinQR = -((gsl_pow_2(r1*r2)*(  2*r2*cos(Asth)*cos(phi)
                                        + cos(Asth+Bsth)*(gsl_pow_2(Bsth)*RADIUS + (-2 + gsl_pow_2(Bsth))*r2*cos(phi))
                                        + Bsth*RADIUS*sin(Asth)
                                        - Bsth*(RADIUS + 2*r2*cos(phi))*sin(Asth+Bsth)))/gsl_pow_3(Bsth));
    } else {
        Ir2CosQR =      (gsl_pow_2(r1*r2)*RADIUS*cos(Asth))/2. +
                        (gsl_pow_2(r1*r2)*r2*cos(Asth)*cos(phi))/3. +
                        gsl_pow_2(Bsth)*(-(gsl_pow_2(r1*r2)*RADIUS*cos(Asth))/8. -
                        (gsl_pow_2(r1*r2)*r2*cos(Asth)*cos(phi))/10.) +
                    Bsth*(-(gsl_pow_2(r1*r2)*RADIUS*sin(Asth))/3. -
                           (gsl_pow_2(r1*r2)*r2*cos(phi)*sin(Asth))/4.);
        Ir2SinQR = Bsth*((gsl_pow_2(r1*r2)*RADIUS*cos(Asth))/3. +
                        (gsl_pow_2(r1*r2)*r2*cos(Asth)*cos(phi))/4.) +
                        (gsl_pow_2(r1*r2)*RADIUS*sin(Asth))/2. +
                        (gsl_pow_2(r1*r2)*r2*cos(phi)*sin(Asth))/3. +
                        gsl_pow_2(Bsth)*(-(gsl_pow_2(r1*r2)*RADIUS*sin(Asth))/8. -
                                          (gsl_pow_2(r1*r2)*r2*cos(phi)*sin(Asth))/10.);
    }
    fval[0] = Ir2CosQR;
    fval[1] = Ir2SinQR;
	return 0;
}

scalar call_opo_Fsth_cub_dtp(scalar x1, scalar x2, scalar *fval, void *pam) {
    scalar fv[2], x[2];
    x[0]=x1;
    x[1]=x2;
    opo_Fsth_cub_dtp(2,x,pam,2,fval);
}

scalar opo_Fsth_GL_3DtpOOURA_Re_dphi(scalar phi, void * pam) {
    scalar theta,r1,r2,Ir2CosQR, Ir2SinQR, Asth,Bsth;
	sasfit_param * param;
	param = (sasfit_param *) pam;
    theta = THETA;
	r1=ri(theta,FALPHA,             FMM,FN1,FN2,FN3,FAA,FBB);
	r2=ri(phi,  SALPHA+theta*SGAMMA,SMM,SN1,SN2,SN3,SAA,SBB);
	Asth = QQZ*PITCH*theta/(2*M_PI) +
           QQX*RADIUS*r1*cos(theta)+
           QQY*RADIUS*r1*sin(theta);
	Bsth = (QQX*r1*r2*cos(theta)*cos(phi) +
            QQY*r1*r2*sin(theta)*cos(phi) +
            QQZ*r2*sin(phi));

    if (fabs(Bsth) > 1e-2) {
        Ir2CosQR = (gsl_pow_2(r1*r2)*(Bsth*RADIUS*(- cos(Asth)
                                                   + cos(Asth+Bsth)
                                                   + Bsth*sin(Asth+Bsth))
                                        + r2*cos(phi)*(  2*Bsth*cos(Asth+Bsth)
                                                        + 2*sin(Asth)
                                                        + (gsl_pow_2(Bsth)-2)*sin(Asth+Bsth))))/gsl_pow_3(Bsth);
/*
        Ir2SinQR = -((gsl_pow_2(r1*r2)*(  2*r2*cos(Asth)*cos(phi)
                                        + cos(Asth+Bsth)*(gsl_pow_2(Bsth)*RADIUS + (-2 + gsl_pow_2(Bsth))*r2*cos(phi))
                                        + Bsth*RADIUS*sin(Asth)
                                        - Bsth*(RADIUS + 2*r2*cos(phi))*sin(Asth+Bsth)))/gsl_pow_3(Bsth));
*/
    } else {
        Ir2CosQR =      (gsl_pow_2(r1*r2)*RADIUS*cos(Asth))/2. +
                        (gsl_pow_2(r1*r2)*r2*cos(Asth)*cos(phi))/3. +
                        gsl_pow_2(Bsth)*(-(gsl_pow_2(r1*r2)*RADIUS*cos(Asth))/8. -
                        (gsl_pow_2(r1*r2)*r2*cos(Asth)*cos(phi))/10.) +
                    Bsth*(-(gsl_pow_2(r1*r2)*RADIUS*sin(Asth))/3. -
                           (gsl_pow_2(r1*r2)*r2*cos(phi)*sin(Asth))/4.);
/*
        Ir2SinQR = Bsth*((gsl_pow_2(r1*r2)*RADIUS*cos(Asth))/3. +
                        (gsl_pow_2(r1*r2)*r2*cos(Asth)*cos(phi))/4.) +
                        (gsl_pow_2(r1*r2)*RADIUS*sin(Asth))/2. +
                        (gsl_pow_2(r1*r2)*r2*cos(phi)*sin(Asth))/3. +
                        gsl_pow_2(Bsth)*(-(gsl_pow_2(r1*r2)*RADIUS*sin(Asth))/8. -
                                          (gsl_pow_2(r1*r2)*r2*cos(phi)*sin(Asth))/10.);
*/
    }
    return Ir2CosQR;
}

scalar opo_Fsth_GL_3DtpOOURA_Im_dphi(scalar phi, void * pam) {
    scalar theta,r1,r2,Ir2CosQR, Ir2SinQR, Asth,Bsth;
	sasfit_param * param;
	param = (sasfit_param *) pam;
	theta = THETA;
	r1=ri(theta,FALPHA,FMM,FN1,FN2,FN3,FAA,FBB);
	r2=ri(phi,SALPHA+theta*SGAMMA,SMM,SN1,SN2,SN3,SAA,SBB);
	Asth = QQZ*PITCH*theta/(2*M_PI) +
           QQX*RADIUS*r1*cos(theta)+
           QQY*RADIUS*r1*sin(theta);
	Bsth = (QQX*r1*r2*cos(theta)*cos(phi) +
            QQY*r1*r2*sin(theta)*cos(phi) +
            QQZ*r2*sin(phi));

    if (fabs(Bsth) > 1e-2) {
/*
        Ir2CosQR = (gsl_pow_2(r1*r2)*(Bsth*RADIUS*(- cos(Asth)
                                                   + cos(Asth+Bsth)
                                                   + Bsth*sin(Asth+Bsth))
                                        + r2*cos(phi)*(  2*Bsth*cos(Asth+Bsth)
                                                        + 2*sin(Asth)
                                                        + (gsl_pow_2(Bsth)-2)*sin(Asth+Bsth))))/gsl_pow_3(Bsth);
*/
        Ir2SinQR = -((gsl_pow_2(r1*r2)*(  2*r2*cos(Asth)*cos(phi)
                                        + cos(Asth+Bsth)*(gsl_pow_2(Bsth)*RADIUS + (-2 + gsl_pow_2(Bsth))*r2*cos(phi))
                                        + Bsth*RADIUS*sin(Asth)
                                        - Bsth*(RADIUS + 2*r2*cos(phi))*sin(Asth+Bsth)))/gsl_pow_3(Bsth));
    } else {
/*
        Ir2CosQR =      (gsl_pow_2(r1*r2)*RADIUS*cos(Asth))/2. +
                        (gsl_pow_2(r1*r2)*r2*cos(Asth)*cos(phi))/3. +
                        gsl_pow_2(Bsth)*(-(gsl_pow_2(r1*r2)*RADIUS*cos(Asth))/8. -
                        (gsl_pow_2(r1*r2)*r2*cos(Asth)*cos(phi))/10.) +
                    Bsth*(-(gsl_pow_2(r1*r2)*RADIUS*sin(Asth))/3. -
                           (gsl_pow_2(r1*r2)*r2*cos(phi)*sin(Asth))/4.);
*/
        Ir2SinQR = Bsth*((gsl_pow_2(r1*r2)*RADIUS*cos(Asth))/3. +
                        (gsl_pow_2(r1*r2)*r2*cos(Asth)*cos(phi))/4.) +
                        (gsl_pow_2(r1*r2)*RADIUS*sin(Asth))/2. +
                        (gsl_pow_2(r1*r2)*r2*cos(phi)*sin(Asth))/3. +
                        gsl_pow_2(Bsth)*(-(gsl_pow_2(r1*r2)*RADIUS*sin(Asth))/8. -
                                          (gsl_pow_2(r1*r2)*r2*cos(phi)*sin(Asth))/10.);

    }
    return Ir2SinQR;
}

scalar opo_Fsth_GL_3DtpOOURA_Re_dtheta(scalar theta, void * pam) {
	sasfit_param * param;
	param = (sasfit_param *) pam;
    scalar *aw, res,err,sum, cubxmax[1],cubxmin[1];
    int intstrategy, lenaw=4000;
	cubxmax[0] = M_PI;
	cubxmin[0] =-M_PI;
	RR = 1;
	THETA = theta;
	intstrategy = sasfit_get_int_strategy();
//    intstrategy=OOURA_DOUBLE_EXP_QUADRATURE;
	switch(intstrategy) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_aniso(), aw);
            sasfit_intde(&opo_Fsth_GL_3DtpOOURA_Re_dphi,cubxmin[0], cubxmax[0], aw, &res, &err, param);
            sum = res;
            free(aw);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE:
    default: {
            aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
            sasfit_intccini(lenaw, aw);
            sasfit_intcc(&opo_Fsth_GL_3DtpOOURA_Re_dphi,cubxmin[0], cubxmax[0], sasfit_eps_get_aniso(), lenaw, aw, &res, &err,param);
            sum = res;
            free(aw);
            break;
            }
    }
	return sum;
}

scalar opo_Fsth_GL_3DtpOOURA_Im_dtheta(scalar theta, void * pam) {
	sasfit_param * param;
	param = (sasfit_param *) pam;
    scalar *aw, res,err,sum, cubxmax[1],cubxmin[1];
    int intstrategy, lenaw=4000;
	cubxmax[0] = M_PI;
	cubxmin[0] =-M_PI;
	RR = 1;
	THETA = theta;
	intstrategy = sasfit_get_int_strategy();
//    intstrategy=OOURA_DOUBLE_EXP_QUADRATURE;
	switch(intstrategy) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_aniso(), aw);
            sasfit_intde(&opo_Fsth_GL_3DtpOOURA_Im_dphi,cubxmin[0], cubxmax[0], aw, &res, &err, param);
            sum = res;
            free(aw);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE:
    default: {
            aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
            sasfit_intccini(lenaw, aw);
            sasfit_intcc(&opo_Fsth_GL_3DtpOOURA_Im_dphi,cubxmin[0], cubxmax[0], sasfit_eps_get_aniso(), lenaw, aw, &res, &err,param);
            sum = res;
            free(aw);
            break;
            }
    }
	return sum;
}

int opo_Fss_cub_dtp(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
    scalar r,theta,phi,r1,r2,DJSS3D,QR,Ir2CosQR, Ir2SinQR;
	sasfit_param * param;
	param = (sasfit_param *) pam;
	if ((ndim < 2) || (fdim < 2)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	theta = x[0];
	phi   = x[1];
	r1=ri(theta,FALPHA,FMM,FN1,FN2,FN3,FAA,FBB);
	r2=ri(phi  ,SALPHA,SMM,SN1,SN2,SN3,SAA,SBB);
	XX = r1*cos(theta)*r2*cos(phi);
	YY = r1*sin(theta)*r2*cos(phi);
	ZZ = r2*sin(phi);
    QR = QQX*XX + QQY*YY + QQZ*ZZ;
    if (fabs(QR)>1e-6) {
        Ir2CosQR = (2*QR*cos(QR) + (-2 + gsl_pow_2(QR))*sin(QR))/gsl_pow_3(QR);
        Ir2SinQR = (-2 + (2 - gsl_pow_2(QR))*cos(QR) + 2*QR*sin(QR))/gsl_pow_3(QR);
    } else {
        Ir2CosQR = 1/3-gsl_pow_2(QR)/10.+gsl_pow_4(QR)/168.-gsl_pow_6(QR)/6480;
        Ir2SinQR = QR/4.-gsl_pow_3(QR)/36.+gsl_pow_5(QR)/960.-gsl_pow_7(QR)/50400.;
    }
	DJSS3D = cos(phi)*gsl_pow_2(r1)*gsl_pow_3(r2);
    fval[0] = DJSS3D*Ir2CosQR;
    fval[1] = DJSS3D*Ir2SinQR;
	return 0;
}

int opo_Fratss_cub_dtp(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
    scalar r,theta,phi,r1,r2,DJSS3D,QR,Ir2CosQR, Ir2SinQR;
	sasfit_param * param;
	param = (sasfit_param *) pam;
	if ((ndim < 2) || (fdim < 2)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	theta = x[0];
	phi   = x[1];
	r1=ratri(theta,FALPHA,FMM,FN1,FN2,FN3,FAA,FBB);
	r2=ratri(phi,SALPHA,SMM,SN1,SN2,SN3,SAA,SBB);
	XX = r1*cos(theta)*r2*cos(phi);
	YY = r1*sin(theta)*r2*cos(phi);
	ZZ = r2*sin(phi);
    QR = QQX*XX + QQY*YY + QQZ*ZZ;
    if (fabs(QR)>1e-6) {
        Ir2CosQR = (2*QR*cos(QR) + (-2 + gsl_pow_2(QR))*sin(QR))/gsl_pow_3(QR);
        Ir2SinQR = (-2 + (2 - gsl_pow_2(QR))*cos(QR) + 2*QR*sin(QR))/gsl_pow_3(QR);
    } else {
        Ir2CosQR = 1/3-gsl_pow_2(QR)/10.+gsl_pow_4(QR)/168.-gsl_pow_6(QR)/6480.;
        Ir2SinQR = QR/4.-gsl_pow_3(QR)/36.+gsl_pow_5(QR)/960.-gsl_pow_7(QR)/50400.;
    }
	DJSS3D = cos(phi)*gsl_pow_2(r1)*gsl_pow_3(r2);
    fval[0] = DJSS3D*Ir2CosQR;
    fval[1] = DJSS3D*Ir2SinQR;
	return 0;
}
scalar call_opo_Fss_cub_dtp(scalar x1, scalar x2, scalar *fval, void *pam) {
    scalar fv[2], x[2];
    x[0]=x1;
    x[1]=x2;
    opo_Fss_cub_dtp(2,x,pam,2,fval);
}

scalar call_opo_Fratss_cub_dtp(scalar x1, scalar x2, scalar *fval, void *pam) {
    scalar fv[2], x[2];
    x[0]=x1;
    x[1]=x2;
    opo_Fratss_cub_dtp(2,x,pam,2,fval);
}

int opo_Fss_cub_drtp(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
    scalar r,theta,phi,r1,r2,DJSS3D;
	sasfit_param * param;
	param = (sasfit_param *) pam;
	if ((ndim < 3) || (fdim < 2)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	r     = x[0];
	theta = x[1];
	phi   = x[2];
	if (r==0) return 0;
	r1=ri(theta,FALPHA,FMM,FN1,FN2,FN3,FAA,FBB);
	r2=ri(phi,SALPHA,SMM,SN1,SN2,SN3,SAA,SBB);
	XX = r*r1*cos(theta)*r2*cos(phi);
	YY = r*r1*sin(theta)*r2*cos(phi);
	ZZ = r*r2*sin(phi);

	DJSS3D = (r*r*cos(phi))*gsl_pow_2(r1)*gsl_pow_3(r2);
	fval[0] = DJSS3D*cos(QQX*XX + QQY*YY + QQZ*ZZ);
	fval[1] = DJSS3D*sin(QQX*XX + QQY*YY + QQZ*ZZ);
//	sasfit_out("r1=%lg r2=%lg theta=%lg phi=%lg Qx=%lg Qy=%lg Qz=%lg x=%lg y=%lg z=%lg DetJ=%lg\n",r1,r2,theta,phi,QQX,QQY,QQZ,XX,YY,ZZ,DJSS3D);
	return 0;
}

int opo_Fsth_cub_drtp(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
    scalar r,theta,phi,r1,r2,DJSTH3D;
	sasfit_param * param;
	param = (sasfit_param *) pam;
	if ((ndim < 3) || (fdim < 2)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	r     = x[0];
	theta = x[1];
	phi   = x[2];
//	if (r==0) return 0;
	r1=ri(theta,FALPHA,FMM,FN1,FN2,FN3,FAA,FBB);
	r2=ri(phi,SALPHA+theta*SGAMMA,SMM,SN1,SN2,SN3,SAA,SBB);
	XX =   r1*cos(theta)*(r*r2*cos(phi)+RADIUS);
	YY =   r1*sin(theta)*(r*r2*cos(phi)+RADIUS);
	ZZ =                  r*r2*sin(phi)+PITCH*theta/(2*M_PI);

	DJSTH3D = r*gsl_pow_2(r1)*(gsl_pow_2(r2)*RADIUS+gsl_pow_3(r2)*r*cos(phi));
	fval[0] = DJSTH3D*cos(QQX*XX + QQY*YY + QQZ*ZZ);
	fval[1] = DJSTH3D*sin(QQX*XX + QQY*YY + QQZ*ZZ);
//	sasfit_out("r1=%lg r2=%lg theta=%lg phi=%lg Qx=%lg Qy=%lg Qz=%lg x=%lg y=%lg z=%lg DetJ=%lg\n",r1,r2,theta,phi,QQX,QQY,QQZ,XX,YY,ZZ,DJSS3D);
	return 0;
}

int opo_Fratss_cub_drtp(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
    scalar r,theta,phi,r1,r2,DJSS3D;
	sasfit_param * param;
	param = (sasfit_param *) pam;
	if ((ndim < 3) || (fdim < 2)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	r     = x[0];
	theta = x[1];
	phi   = x[2];
	if (r==0) return 0;
	r1=ratri(theta,FALPHA,FMM,FN1,FN2,FN3,FAA,FBB);
	r2=ratri(phi,SALPHA,SMM,SN1,SN2,SN3,SAA,SBB);
	XX = r*r1*cos(theta)*r2*cos(phi);
	YY = r*r1*sin(theta)*r2*cos(phi);
	ZZ = r*r2*sin(phi);

	DJSS3D = (r*r*cos(phi))*gsl_pow_2(r1)*gsl_pow_3(r2);
	fval[0] = DJSS3D*cos(QQX*XX + QQY*YY + QQZ*ZZ);
	fval[1] = DJSS3D*sin(QQX*XX + QQY*YY + QQZ*ZZ);
//	sasfit_out("r1=%lg r2=%lg theta=%lg phi=%lg Qx=%lg Qy=%lg Qz=%lg x=%lg y=%lg z=%lg DetJ=%lg\n",r1,r2,theta,phi,QQX,QQY,QQZ,XX,YY,ZZ,DJSS3D);
	return 0;
}

scalar call_opo_Fss_cub_drtp(scalar x1, scalar x2, scalar x3, scalar *fval, void *pam) {
    scalar x[3];
    x[0]=x1;
    x[1]=x2;
    x[2]=x3;
    opo_Fss_cub_drtp(3,x,pam,2,fval);
}

scalar call_opo_Fsth_cub_drtp(scalar x1, scalar x2, scalar x3, scalar *fval, void *pam) {
    scalar x[3];
    x[0]=x1;
    x[1]=x2;
    x[2]=x3;
    opo_Fsth_cub_drtp(3,x,pam,2,fval);
}

scalar call_opo_Fratss_cub_drtp(scalar x1, scalar x2, scalar x3, scalar *fval, void *pam) {
    scalar x[3];
    x[0]=x1;
    x[1]=x2;
    x[2]=x3;
    opo_Fratss_cub_drtp(3,x,pam,2,fval);
}
scalar opo_Fss_GL_3Drtp(void *pam) {
	sasfit_param * param;
	param = (sasfit_param *) pam;
    int n,m,i,j,k, ndim=3, fdim=2;
    scalar fval[2],x[3],*A_GL,*W_GL,rs,is, fa,fb,fA,fB,fc,fd,fC,fD,fe,ff,fE,fF;
    gsl_integration_glfixed_table *t;
    n = abs(sasfit_eps_get_robertus_p());
    m = (n+1)>>1;
    t=gsl_integration_glfixed_table_alloc(n);

    fa=0;fb=1;
    fA=(fb-fa)*0.5;
    fB=(fb+fa)*0.5;
    fc=-M_PI;fd=M_PI;
    fC=(fd-fc)*0.5;
    fD=(fd+fc)*0.5;
    fe=-M_PI_2;ff=M_PI_2;
    fE=(ff-fe)*0.5;
    fF=(ff+fe)*0.5;
    A_GL = t->x;
    W_GL = t->w;
    rs = 0;
    is = 0;
    if (n&1) /* n - odd */
    {
        call_opo_Fss_cub_drtp(fB,fD,fF,fval,pam);
        rs += gsl_pow_3(W_GL[0])*fval[0];
        is += gsl_pow_3(W_GL[0])*fval[1];
        for (i=1;i<m;i++) {
            call_opo_Fss_cub_drtp(fB+fA*A_GL[i],fD,fF,fval,pam);
            rs += gsl_pow_2(W_GL[0])*W_GL[i]*fval[0];
            is += gsl_pow_2(W_GL[0])*W_GL[i]*fval[1];
            call_opo_Fss_cub_drtp(fB-fA*A_GL[i],fD,fF,fval,pam);
            rs += gsl_pow_2(W_GL[0])*W_GL[i]*fval[0];
            is += gsl_pow_2(W_GL[0])*W_GL[i]*fval[1];
            call_opo_Fss_cub_drtp(fB,fD+fC*A_GL[0],fF,fval,pam);
            rs += gsl_pow_2(W_GL[0])*W_GL[i]*fval[0];
            is += gsl_pow_2(W_GL[0])*W_GL[i]*fval[1];
            call_opo_Fss_cub_drtp(fB,fD-fC*A_GL[0],fF,fval,pam);
            rs += gsl_pow_2(W_GL[0])*W_GL[i]*fval[0];
            is += gsl_pow_2(W_GL[0])*W_GL[i]*fval[1];
            call_opo_Fss_cub_drtp(fB,fD,fF+fE*A_GL[0],fval,pam);
            rs += gsl_pow_2(W_GL[0])*W_GL[i]*fval[0];
            is += gsl_pow_2(W_GL[0])*W_GL[i]*fval[1];
            call_opo_Fss_cub_drtp(fB,fD,fF-fE*A_GL[0],fval,pam);
            rs += gsl_pow_2(W_GL[0])*W_GL[i]*fval[0];
            is += gsl_pow_2(W_GL[0])*W_GL[i]*fval[1];
            for (j=1;j<m;j++) {
                call_opo_Fss_cub_drtp(fB+fA*A_GL[i],fD+fC*A_GL[j],fF,fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fss_cub_drtp(fB+fA*A_GL[i],fD,fF+fE*A_GL[j],fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fss_cub_drtp(fB,fD+fC*A_GL[i],fF+fE*A_GL[j],fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fss_cub_drtp(fB+fA*A_GL[i],fD-fC*A_GL[j],fF,fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fss_cub_drtp(fB+fA*A_GL[i],fD,fF-fE*A_GL[j],fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fss_cub_drtp(fB,fD+fC*A_GL[i],fF-fE*A_GL[j],fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fss_cub_drtp(fB-fA*A_GL[i],fD+fC*A_GL[j],fF,fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fss_cub_drtp(fB-fA*A_GL[i],fD,fF+fE*A_GL[j],fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fss_cub_drtp(fB,fD-fC*A_GL[i],fF+fE*A_GL[j],fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fss_cub_drtp(fB-fA*A_GL[i],fD-fC*A_GL[j],fF,fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fss_cub_drtp(fB-fA*A_GL[i],fD,fF-fE*A_GL[j],fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fss_cub_drtp(fB,fD-fC*A_GL[i],fF-fE*A_GL[j],fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                for (k=1;k<m;k++) {
                    call_opo_Fss_cub_drtp(fB+fA*A_GL[i],fD+fC*A_GL[j],fF+fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fss_cub_drtp(fB+fA*A_GL[i],fD+fC*A_GL[j],fF-fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fss_cub_drtp(fB+fA*A_GL[i],fD-fC*A_GL[j],fF+fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fss_cub_drtp(fB-fA*A_GL[i],fD+fC*A_GL[j],fF+fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fss_cub_drtp(fB+fA*A_GL[i],fD-fC*A_GL[j],fF-fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fss_cub_drtp(fB-fA*A_GL[i],fD+fC*A_GL[j],fF-fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fss_cub_drtp(fB-fA*A_GL[i],fD-fC*A_GL[j],fF+fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fss_cub_drtp(fB-fA*A_GL[i],fD-fC*A_GL[j],fF-fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                }
            }
        }
    }
    else  /* n - even */
    {
      for (i=0;i<m;i++) {
            for (j=0;j<m;j++) {
                for (k=0;k<m;k++) {
                    call_opo_Fss_cub_drtp(fB+fA*A_GL[i],fD+fC*A_GL[j],fF+fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fss_cub_drtp(fB+fA*A_GL[i],fD+fC*A_GL[j],fF-fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fss_cub_drtp(fB+fA*A_GL[i],fD-fC*A_GL[j],fF+fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fss_cub_drtp(fB-fA*A_GL[i],fD+fC*A_GL[j],fF+fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fss_cub_drtp(fB+fA*A_GL[i],fD-fC*A_GL[j],fF-fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fss_cub_drtp(fB-fA*A_GL[i],fD+fC*A_GL[j],fF-fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fss_cub_drtp(fB-fA*A_GL[i],fD-fC*A_GL[j],fF+fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fss_cub_drtp(fB-fA*A_GL[i],fD-fC*A_GL[j],fF-fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                }
            }
        }
    }
    gsl_integration_glfixed_table_free(t);
    return fA*fC*fE*gsl_hypot(rs,is);
}

scalar opo_Fsth_GL_3Drtp(void *pam) {
	sasfit_param * param;
	param = (sasfit_param *) pam;
    int n,m,i,j,k, ndim=3, fdim=2;
    scalar fval[2],x[3],*A_GL,*W_GL,rs,is, fa,fb,fA,fB,fc,fd,fC,fD,fe,ff,fE,fF;
    gsl_integration_glfixed_table *t;
    n = abs(sasfit_eps_get_robertus_p());
    m = (n+1)>>1;
    t=gsl_integration_glfixed_table_alloc(n);

    fa=0;fb=1;
    fA=(fb-fa)*0.5;
    fB=(fb+fa)*0.5;
    fc=-TURNS*M_PI;fd=TURNS*M_PI;
    fC=(fd-fc)*0.5;
    fD=(fd+fc)*0.5;
    fe=-M_PI;ff=M_PI;
    fE=(ff-fe)*0.5;
    fF=(ff+fe)*0.5;
    A_GL = t->x;
    W_GL = t->w;
    rs = 0;
    is = 0;
    if (n&1) /* n - odd */
    {
        call_opo_Fsth_cub_drtp(fB,fD,fF,fval,pam);
        rs += gsl_pow_3(W_GL[0])*fval[0];
        is += gsl_pow_3(W_GL[0])*fval[1];
        for (i=1;i<m;i++) {
            call_opo_Fsth_cub_drtp(fB+fA*A_GL[i],fD,fF,fval,pam);
            rs += gsl_pow_2(W_GL[0])*W_GL[i]*fval[0];
            is += gsl_pow_2(W_GL[0])*W_GL[i]*fval[1];
            call_opo_Fsth_cub_drtp(fB-fA*A_GL[i],fD,fF,fval,pam);
            rs += gsl_pow_2(W_GL[0])*W_GL[i]*fval[0];
            is += gsl_pow_2(W_GL[0])*W_GL[i]*fval[1];
            call_opo_Fsth_cub_drtp(fB,fD+fC*A_GL[0],fF,fval,pam);
            rs += gsl_pow_2(W_GL[0])*W_GL[i]*fval[0];
            is += gsl_pow_2(W_GL[0])*W_GL[i]*fval[1];
            call_opo_Fsth_cub_drtp(fB,fD-fC*A_GL[0],fF,fval,pam);
            rs += gsl_pow_2(W_GL[0])*W_GL[i]*fval[0];
            is += gsl_pow_2(W_GL[0])*W_GL[i]*fval[1];
            call_opo_Fsth_cub_drtp(fB,fD,fF+fE*A_GL[0],fval,pam);
            rs += gsl_pow_2(W_GL[0])*W_GL[i]*fval[0];
            is += gsl_pow_2(W_GL[0])*W_GL[i]*fval[1];
            call_opo_Fsth_cub_drtp(fB,fD,fF-fE*A_GL[0],fval,pam);
            rs += gsl_pow_2(W_GL[0])*W_GL[i]*fval[0];
            is += gsl_pow_2(W_GL[0])*W_GL[i]*fval[1];
            for (j=1;j<m;j++) {
                call_opo_Fsth_cub_drtp(fB+fA*A_GL[i],fD+fC*A_GL[j],fF,fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fsth_cub_drtp(fB+fA*A_GL[i],fD,fF+fE*A_GL[j],fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fsth_cub_drtp(fB,fD+fC*A_GL[i],fF+fE*A_GL[j],fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fsth_cub_drtp(fB+fA*A_GL[i],fD-fC*A_GL[j],fF,fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fsth_cub_drtp(fB+fA*A_GL[i],fD,fF-fE*A_GL[j],fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fsth_cub_drtp(fB,fD+fC*A_GL[i],fF-fE*A_GL[j],fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fsth_cub_drtp(fB-fA*A_GL[i],fD+fC*A_GL[j],fF,fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fsth_cub_drtp(fB-fA*A_GL[i],fD,fF+fE*A_GL[j],fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fsth_cub_drtp(fB,fD-fC*A_GL[i],fF+fE*A_GL[j],fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fsth_cub_drtp(fB-fA*A_GL[i],fD-fC*A_GL[j],fF,fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fsth_cub_drtp(fB-fA*A_GL[i],fD,fF-fE*A_GL[j],fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fsth_cub_drtp(fB,fD-fC*A_GL[i],fF-fE*A_GL[j],fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                for (k=1;k<m;k++) {
                    call_opo_Fsth_cub_drtp(fB+fA*A_GL[i],fD+fC*A_GL[j],fF+fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fsth_cub_drtp(fB+fA*A_GL[i],fD+fC*A_GL[j],fF-fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fsth_cub_drtp(fB+fA*A_GL[i],fD-fC*A_GL[j],fF+fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fsth_cub_drtp(fB-fA*A_GL[i],fD+fC*A_GL[j],fF+fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fsth_cub_drtp(fB+fA*A_GL[i],fD-fC*A_GL[j],fF-fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fsth_cub_drtp(fB-fA*A_GL[i],fD+fC*A_GL[j],fF-fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fsth_cub_drtp(fB-fA*A_GL[i],fD-fC*A_GL[j],fF+fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fsth_cub_drtp(fB-fA*A_GL[i],fD-fC*A_GL[j],fF-fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                }
            }
        }
    }
    else  /* n - even */
    {
      for (i=0;i<m;i++) {
            for (j=0;j<m;j++) {
                for (k=0;k<m;k++) {
                    call_opo_Fsth_cub_drtp(fB+fA*A_GL[i],fD+fC*A_GL[j],fF+fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fsth_cub_drtp(fB+fA*A_GL[i],fD+fC*A_GL[j],fF-fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fsth_cub_drtp(fB+fA*A_GL[i],fD-fC*A_GL[j],fF+fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fsth_cub_drtp(fB-fA*A_GL[i],fD+fC*A_GL[j],fF+fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fsth_cub_drtp(fB+fA*A_GL[i],fD-fC*A_GL[j],fF-fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fsth_cub_drtp(fB-fA*A_GL[i],fD+fC*A_GL[j],fF-fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fsth_cub_drtp(fB-fA*A_GL[i],fD-fC*A_GL[j],fF+fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fsth_cub_drtp(fB-fA*A_GL[i],fD-fC*A_GL[j],fF-fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                }
            }
        }
    }
    gsl_integration_glfixed_table_free(t);
    return fA*fC*fE*gsl_hypot(rs,is);
}

scalar opo_Fratss_GL_3Drtp(void *pam)
  {
	sasfit_param * param;
	param = (sasfit_param *) pam;
    int n,m,i,j,k, ndim=3, fdim=2;
    scalar fval[2],x[3],*A_GL,*W_GL,rs,is, fa,fb,fA,fB,fc,fd,fC,fD,fe,ff,fE,fF;
    gsl_integration_glfixed_table *t;
    n = abs(sasfit_eps_get_robertus_p());
    m = (n+1)>>1;
    t=gsl_integration_glfixed_table_alloc(n);

    fa=0;fb=1;
    fA=(fb-fa)*0.5;
    fB=(fb+fa)*0.5;
    fc=-M_PI;fd=M_PI;
    fC=(fd-fc)*0.5;
    fD=(fd+fc)*0.5;
    fe=-M_PI_2;ff=M_PI_2;
    fE=(ff-fe)*0.5;
    fF=(ff+fe)*0.5;
    A_GL = t->x;
    W_GL = t->w;
    rs = 0;
    is = 0;
    if (n&1) /* n - odd */
    {
        call_opo_Fratss_cub_drtp(fB,fD,fF,fval,pam);
        rs += gsl_pow_3(W_GL[0])*fval[0];
        is += gsl_pow_3(W_GL[0])*fval[1];
        for (i=1;i<m;i++) {
            call_opo_Fratss_cub_drtp(fB+fA*A_GL[i],fD,fF,fval,pam);
            rs += gsl_pow_2(W_GL[0])*W_GL[i]*fval[0];
            is += gsl_pow_2(W_GL[0])*W_GL[i]*fval[1];
            call_opo_Fratss_cub_drtp(fB-fA*A_GL[i],fD,fF,fval,pam);
            rs += gsl_pow_2(W_GL[0])*W_GL[i]*fval[0];
            is += gsl_pow_2(W_GL[0])*W_GL[i]*fval[1];
            call_opo_Fratss_cub_drtp(fB,fD+fC*A_GL[0],fF,fval,pam);
            rs += gsl_pow_2(W_GL[0])*W_GL[i]*fval[0];
            is += gsl_pow_2(W_GL[0])*W_GL[i]*fval[1];
            call_opo_Fratss_cub_drtp(fB,fD-fC*A_GL[0],fF,fval,pam);
            rs += gsl_pow_2(W_GL[0])*W_GL[i]*fval[0];
            is += gsl_pow_2(W_GL[0])*W_GL[i]*fval[1];
            call_opo_Fratss_cub_drtp(fB,fD,fF+fE*A_GL[0],fval,pam);
            rs += gsl_pow_2(W_GL[0])*W_GL[i]*fval[0];
            is += gsl_pow_2(W_GL[0])*W_GL[i]*fval[1];
            call_opo_Fratss_cub_drtp(fB,fD,fF-fE*A_GL[0],fval,pam);
            rs += gsl_pow_2(W_GL[0])*W_GL[i]*fval[0];
            is += gsl_pow_2(W_GL[0])*W_GL[i]*fval[1];
            for (j=1;j<m;j++) {
                call_opo_Fratss_cub_drtp(fB+fA*A_GL[i],fD+fC*A_GL[j],fF,fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fratss_cub_drtp(fB+fA*A_GL[i],fD,fF+fE*A_GL[j],fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fratss_cub_drtp(fB,fD+fC*A_GL[i],fF+fE*A_GL[j],fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fratss_cub_drtp(fB+fA*A_GL[i],fD-fC*A_GL[j],fF,fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fratss_cub_drtp(fB+fA*A_GL[i],fD,fF-fE*A_GL[j],fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fratss_cub_drtp(fB,fD+fC*A_GL[i],fF-fE*A_GL[j],fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fratss_cub_drtp(fB-fA*A_GL[i],fD+fC*A_GL[j],fF,fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fratss_cub_drtp(fB-fA*A_GL[i],fD,fF+fE*A_GL[j],fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fratss_cub_drtp(fB,fD-fC*A_GL[i],fF+fE*A_GL[j],fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fratss_cub_drtp(fB-fA*A_GL[i],fD-fC*A_GL[j],fF,fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fratss_cub_drtp(fB-fA*A_GL[i],fD,fF-fE*A_GL[j],fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fratss_cub_drtp(fB,fD-fC*A_GL[i],fF-fE*A_GL[j],fval,pam);
                rs += W_GL[0]*W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[0]*W_GL[i]*W_GL[j]*fval[1];
                for (k=1;k<m;k++) {
                    call_opo_Fratss_cub_drtp(fB+fA*A_GL[i],fD+fC*A_GL[j],fF+fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fratss_cub_drtp(fB+fA*A_GL[i],fD+fC*A_GL[j],fF-fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fratss_cub_drtp(fB+fA*A_GL[i],fD-fC*A_GL[j],fF+fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fratss_cub_drtp(fB-fA*A_GL[i],fD+fC*A_GL[j],fF+fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fratss_cub_drtp(fB+fA*A_GL[i],fD-fC*A_GL[j],fF-fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fratss_cub_drtp(fB-fA*A_GL[i],fD+fC*A_GL[j],fF-fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fratss_cub_drtp(fB-fA*A_GL[i],fD-fC*A_GL[j],fF+fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fratss_cub_drtp(fB-fA*A_GL[i],fD-fC*A_GL[j],fF-fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                }
            }
        }
    }
    else  /* n - even */
    {
      for (i=0;i<m;i++) {
            for (j=0;j<m;j++) {
                for (k=0;k<m;k++) {
                    call_opo_Fratss_cub_drtp(fB+fA*A_GL[i],fD+fC*A_GL[j],fF+fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fratss_cub_drtp(fB+fA*A_GL[i],fD+fC*A_GL[j],fF-fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fratss_cub_drtp(fB+fA*A_GL[i],fD-fC*A_GL[j],fF+fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fratss_cub_drtp(fB-fA*A_GL[i],fD+fC*A_GL[j],fF+fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fratss_cub_drtp(fB+fA*A_GL[i],fD-fC*A_GL[j],fF-fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fratss_cub_drtp(fB-fA*A_GL[i],fD+fC*A_GL[j],fF-fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fratss_cub_drtp(fB-fA*A_GL[i],fD-fC*A_GL[j],fF+fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                    call_opo_Fratss_cub_drtp(fB-fA*A_GL[i],fD-fC*A_GL[j],fF-fE*A_GL[k],fval,pam);
                    rs += W_GL[k]*W_GL[i]*W_GL[j]*fval[0];
                    is += W_GL[k]*W_GL[i]*W_GL[j]*fval[1];
                }
            }
        }
    }
    gsl_integration_glfixed_table_free(t);
    return fA*fC*fE*gsl_hypot(rs,is);
}

scalar opo_Fss_GL_3Dtp(void *pam)
  {
	sasfit_param * param;
	param = (sasfit_param *) pam;
    int n,m,i,j,k, ndim=2, fdim=2;
    scalar fval[2],x[2],*A_GL,*W_GL,rs,is, fa,fb,fA,fB,fc,fd,fC,fD;
    gsl_integration_glfixed_table *t;
    n = abs(sasfit_eps_get_robertus_p());
    m = (n+1)>>1;
    t=gsl_integration_glfixed_table_alloc(n);

    fa=-M_PI;fb=M_PI;
    fA=(fb-fa)*0.5;
    fB=(fb+fa)*0.5;
    fc=-M_PI_2;fd=M_PI_2;
    fC=(fd-fc)*0.5;
    fD=(fd+fc)*0.5;
    A_GL = t->x;
    W_GL = t->w;
    rs = 0;
    is = 0;
    if (n&1) /* n - odd */
    {
        call_opo_Fss_cub_dtp(fB,fD,fval,pam);
        rs += gsl_pow_2(W_GL[0])*fval[0];
        is += gsl_pow_2(W_GL[0])*fval[1];
        for (i=1;i<m;i++) {
            call_opo_Fss_cub_dtp(fB+fA*A_GL[i],fD,fval,pam);
            rs += W_GL[0]*W_GL[i]*fval[0];
            is += W_GL[0]*W_GL[i]*fval[1];
            call_opo_Fss_cub_dtp(fB-fA*A_GL[i],fD,fval,pam);
            rs += W_GL[0]*W_GL[i]*fval[0];
            is += W_GL[0]*W_GL[i]*fval[1];
            call_opo_Fss_cub_dtp(fB,fD+fC*A_GL[0],fval,pam);
            rs += W_GL[0]*W_GL[i]*fval[0];
            is += W_GL[0]*W_GL[i]*fval[1];
            call_opo_Fss_cub_dtp(fB,fD-fC*A_GL[0],fval,pam);
            rs += W_GL[0]*W_GL[i]*fval[0];
            is += W_GL[0]*W_GL[i]*fval[1];
            for (j=1;j<m;j++) {
                call_opo_Fss_cub_dtp(fB+fA*A_GL[i],fD+fC*A_GL[j],fval,pam);
                rs += W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fss_cub_dtp(fB-fA*A_GL[i],fD+fC*A_GL[j],fval,pam);
                rs += W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fss_cub_dtp(fB+fA*A_GL[i],fD-fC*A_GL[j],fval,pam);
                rs += W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fss_cub_dtp(fB-fA*A_GL[i],fD-fC*A_GL[j],fval,pam);
                rs += W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[i]*W_GL[j]*fval[1];
            }
        }
    }
    else  /* n - even */
    {
      for (i=0;i<m;i++) {
            for (j=0;j<m;j++) {
                call_opo_Fss_cub_dtp(fB+fA*A_GL[i],fD+fC*A_GL[j],fval,pam);
                rs += W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fss_cub_dtp(fB+fA*A_GL[i],fD-fC*A_GL[j],fval,pam);
                rs += W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fss_cub_dtp(fB-fA*A_GL[i],fD+fC*A_GL[j],fval,pam);
                rs += W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fss_cub_dtp(fB-fA*A_GL[i],fD-fC*A_GL[j],fval,pam);
                rs += W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[i]*W_GL[j]*fval[1];
            }
        }
    }
    gsl_integration_glfixed_table_free(t);
    return fA*fC*gsl_hypot(rs,is);
}

scalar opo_Fsth_GL_3Dtp(void *pam)
  {
	sasfit_param * param;
	param = (sasfit_param *) pam;
    int n,m,i,j,k, ndim=2, fdim=2;
    scalar fval[2],x[2],*A_GL,*W_GL,rs,is, fa,fb,fA,fB,fc,fd,fC,fD;
    gsl_integration_glfixed_table *t;
    n = abs(sasfit_eps_get_robertus_p());
    m = (n+1)>>1;
    t=gsl_integration_glfixed_table_alloc(n);

    fa=-TURNS*M_PI;fb=TURNS*M_PI;
    fA=(fb-fa)*0.5;
    fB=(fb+fa)*0.5;
    fc=-M_PI;fd=M_PI;
    fC=(fd-fc)*0.5;
    fD=(fd+fc)*0.5;
    A_GL = t->x;
    W_GL = t->w;
    rs = 0;
    is = 0;
    if (n&1) /* n - odd */
    {
        call_opo_Fsth_cub_dtp(fB,fD,fval,pam);
        rs += gsl_pow_2(W_GL[0])*fval[0];
        is += gsl_pow_2(W_GL[0])*fval[1];
        for (i=1;i<m;i++) {
            call_opo_Fsth_cub_dtp(fB+fA*A_GL[i],fD,fval,pam);
            rs += W_GL[0]*W_GL[i]*fval[0];
            is += W_GL[0]*W_GL[i]*fval[1];
            call_opo_Fsth_cub_dtp(fB-fA*A_GL[i],fD,fval,pam);
            rs += W_GL[0]*W_GL[i]*fval[0];
            is += W_GL[0]*W_GL[i]*fval[1];
            call_opo_Fsth_cub_dtp(fB,fD+fC*A_GL[0],fval,pam);
            rs += W_GL[0]*W_GL[i]*fval[0];
            is += W_GL[0]*W_GL[i]*fval[1];
            call_opo_Fsth_cub_dtp(fB,fD-fC*A_GL[0],fval,pam);
            rs += W_GL[0]*W_GL[i]*fval[0];
            is += W_GL[0]*W_GL[i]*fval[1];
            for (j=1;j<m;j++) {
                call_opo_Fsth_cub_dtp(fB+fA*A_GL[i],fD+fC*A_GL[j],fval,pam);
                rs += W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fsth_cub_dtp(fB-fA*A_GL[i],fD+fC*A_GL[j],fval,pam);
                rs += W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fsth_cub_dtp(fB+fA*A_GL[i],fD-fC*A_GL[j],fval,pam);
                rs += W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fsth_cub_dtp(fB-fA*A_GL[i],fD-fC*A_GL[j],fval,pam);
                rs += W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[i]*W_GL[j]*fval[1];
            }
        }
    }
    else  /* n - even */
    {
      for (i=0;i<m;i++) {
            for (j=0;j<m;j++) {
                call_opo_Fsth_cub_dtp(fB+fA*A_GL[i],fD+fC*A_GL[j],fval,pam);
                rs += W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fsth_cub_dtp(fB+fA*A_GL[i],fD-fC*A_GL[j],fval,pam);
                rs += W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fsth_cub_dtp(fB-fA*A_GL[i],fD+fC*A_GL[j],fval,pam);
                rs += W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fsth_cub_dtp(fB-fA*A_GL[i],fD-fC*A_GL[j],fval,pam);
                rs += W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[i]*W_GL[j]*fval[1];
            }
        }
    }
    gsl_integration_glfixed_table_free(t);
    return fA*fC*gsl_hypot(rs,is);
}


scalar opo_Fratss_GL_3Dtp(void *pam)
  {
	sasfit_param * param;
	param = (sasfit_param *) pam;
    int n,m,i,j,k, ndim=2, fdim=2;
    scalar fval[2],x[2],*A_GL,*W_GL,rs,is, fa,fb,fA,fB,fc,fd,fC,fD;
    gsl_integration_glfixed_table *t;
    n = abs(sasfit_eps_get_robertus_p());
    m = (n+1)>>1;
    t=gsl_integration_glfixed_table_alloc(n);

    fa=-M_PI;fb=M_PI;
    fA=(fb-fa)*0.5;
    fB=(fb+fa)*0.5;
    fc=-M_PI_2;fd=M_PI_2;
    fC=(fd-fc)*0.5;
    fD=(fd+fc)*0.5;
    A_GL = t->x;
    W_GL = t->w;
    rs = 0;
    is = 0;
    if (n&1) /* n - odd */
    {
        call_opo_Fratss_cub_dtp(fB,fD,fval,pam);
        rs += gsl_pow_2(W_GL[0])*fval[0];
        is += gsl_pow_2(W_GL[0])*fval[1];
        for (i=1;i<m;i++) {
            call_opo_Fratss_cub_dtp(fB+fA*A_GL[i],fD,fval,pam);
            rs += W_GL[0]*W_GL[i]*fval[0];
            is += W_GL[0]*W_GL[i]*fval[1];
            call_opo_Fratss_cub_dtp(fB-fA*A_GL[i],fD,fval,pam);
            rs += W_GL[0]*W_GL[i]*fval[0];
            is += W_GL[0]*W_GL[i]*fval[1];
            call_opo_Fratss_cub_dtp(fB,fD+fC*A_GL[0],fval,pam);
            rs += W_GL[0]*W_GL[i]*fval[0];
            is += W_GL[0]*W_GL[i]*fval[1];
            call_opo_Fratss_cub_dtp(fB,fD-fC*A_GL[0],fval,pam);
            rs += W_GL[0]*W_GL[i]*fval[0];
            is += W_GL[0]*W_GL[i]*fval[1];
            for (j=1;j<m;j++) {
                call_opo_Fratss_cub_dtp(fB+fA*A_GL[i],fD+fC*A_GL[j],fval,pam);
                rs += W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fratss_cub_dtp(fB-fA*A_GL[i],fD+fC*A_GL[j],fval,pam);
                rs += W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fratss_cub_dtp(fB+fA*A_GL[i],fD-fC*A_GL[j],fval,pam);
                rs += W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fratss_cub_dtp(fB-fA*A_GL[i],fD-fC*A_GL[j],fval,pam);
                rs += W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[i]*W_GL[j]*fval[1];
            }
        }
    }
    else  /* n - even */
    {
      for (i=0;i<m;i++) {
            for (j=0;j<m;j++) {
                call_opo_Fratss_cub_dtp(fB+fA*A_GL[i],fD+fC*A_GL[j],fval,pam);
                rs += W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fratss_cub_dtp(fB+fA*A_GL[i],fD-fC*A_GL[j],fval,pam);
                rs += W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fratss_cub_dtp(fB-fA*A_GL[i],fD+fC*A_GL[j],fval,pam);
                rs += W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[i]*W_GL[j]*fval[1];
                call_opo_Fratss_cub_dtp(fB-fA*A_GL[i],fD-fC*A_GL[j],fval,pam);
                rs += W_GL[i]*W_GL[j]*fval[0];
                is += W_GL[i]*W_GL[j]*fval[1];
            }
        }
    }
    gsl_integration_glfixed_table_free(t);
    return fA*fC*gsl_hypot(rs,is);
}

int opo_Fse_cub_dru(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
    scalar DJ2D,w;
	sasfit_param * param;
	param = (sasfit_param *) pam;
	if ((ndim < 2) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	RR=x[0];
	UU=x[1];
	XX = RR*pow(fabs(cos(UU)),PP)*GSL_SIGN(cos(UU));
	YY = RR*pow(fabs(sin(UU)),PP)*GSL_SIGN(sin(UU));
	DJ2D=PP*RR*pow(fabs(cos(UU)*sin(UU)),PP-1);
	w = pow(1 - pow(pow(fabs(XX),2./PP) + pow(fabs(YY),2./PP),PP/QQ),QQ/2.);
    fval[0] = DJ2D*(2*cos(QQX*XX+QQY*YY)*w*opo_sinc(QQZ*w));
//    fval[1] = DJ2D*(2*sin(QQX*XX+QQY*YY)*w*opo_sinc(QQZ*w));
	return 0;
}

int opo_Fse_cub_dxs(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
    scalar w,DJ;
	sasfit_param * param;
	param = (sasfit_param *) pam;
	if ((ndim < 2) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	XX = x[0];
	DJ = pow(1-pow(XX*XX,1./PP),PP/2.);
	YY = DJ*x[1];
	if (fabs(x[0])>1 || fabs(YY)>1 || fabs(x[1])>1) {
		fval[0] = 0;
		return 0;
	}
    w = pow(1. - pow(pow(XX*XX,1./PP) + pow(YY*YY,1./PP),PP/QQ),QQ/2.);
    fval[0] = (2*cos(QQX*XX+ QQY*YY)*w*opo_sinc(QQZ*w))*DJ;
    return 0;
}

int opo_Fse_cub_dxy(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
    scalar w;
	sasfit_param * param;
	param = (sasfit_param *) pam;
	if ((ndim < 2) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	XX = x[0];
	YY = x[1];
	if (fabs(XX)>1 || fabs(YY)>1 || fabs(YY)>pow(1-pow(XX*XX,1/PP),PP/2)) {
		fval[0] = 0;
		return 0;
	}
    w = pow(1 - pow(pow(gsl_pow_2(XX),1/PP) + pow(gsl_pow_2(YY),1/PP),PP/QQ),QQ/2.);
    fval[0] = (2*cos(QQX*XX+ QQY*YY)*w*opo_sinc(QQZ*w));
    return 0;
	}


scalar opo_Fse_cub_dxy_core(const double *x, size_t dim, void * pam)
{
    scalar fval[1];
    if (opo_Fse_cub_dxy(dim, x, pam,1, fval)) sasfit_err("Error in function evaluation opo_Fse_cub_dxy_core\n");
	return fval[0];
}
scalar opo_Fse_cub_dru_core(const double *x, size_t dim, void * pam)
{
    scalar fval[1];
     if (opo_Fse_cub_dru(dim, x, pam,1, fval)) sasfit_err("Error in function evaluation opo_Fse_cub_dru_core\n");
	return fval[0];
}

scalar opo_Fse_cub_dxs_core(const double *x, size_t dim, void * pam)
{
    scalar fval[1];
     if (opo_Fse_cub_dxs(dim, x, pam,1, fval)) sasfit_err("Error in function evaluation opo_Fse_cub_dxs_core\n");
	return fval[0];
}

scalar opo_Fsuperellipsoid(void * pam) {
	sasfit_param * param;
	param = (sasfit_param *) pam;
    scalar cubxmin[3], cubxmax[3], fval[2], ferr[2];
    scalar *aw, res,err;
    int intstrategy, lenaw=4000;
	cubxmax[0] = 1;
	cubxmin[0] = -1;
	int auswahl=1;
    switch (auswahl) {
        case 1:
            cubxmax[1] = 1;
            cubxmin[1] = -1;
            sasfit_cubature(2,cubxmin,cubxmax,&opo_Fse_cub_dxs_core,pam,sasfit_eps_get_aniso(),&res,&err);
            break;
        case 2:
            cubxmax[0] = 1;
            cubxmin[0] = 0;
            cubxmax[1] = M_PI_2;
            cubxmin[1] = 0;
            sasfit_cubature(2,cubxmin,cubxmax,&opo_Fse_cub_dru_core,pam,sasfit_eps_get_aniso(),&res,&err);
            err=4*err;
            res=4*res;
            break;
        case 3:
            cubxmax[0] = 1;
            cubxmin[0] = 0;
            cubxmax[1] = M_PI;
            cubxmin[1] = 0;
            sasfit_cubature(2,cubxmin,cubxmax,&opo_Fse_cub_dru_core,pam,sasfit_eps_get_aniso(),&res,&err);
            err=2*err;
            res=2*res;
            break;
        case 4:
            cubxmax[0] = 1;
            cubxmin[0] = 0;
            cubxmax[1] = M_PI*2;
            cubxmin[1] = 0;
            sasfit_cubature(2,cubxmin,cubxmax,&opo_Fse_cub_dru_core,pam,sasfit_eps_get_aniso(),&res,&err);
            err=1*err;
            res=1*res;
            break;
        case 5:
            cubxmax[0] = 1;
            cubxmin[0] = 0;
            cubxmax[1] = M_PI;
            cubxmin[1] = -M_PI;
            sasfit_cubature(2,cubxmin,cubxmax,&opo_Fse_cub_dru_core,pam,sasfit_eps_get_aniso(),&res,&err);
            err=1*err;
            res=1*res;
            break;
        default:
            cubxmax[1] = 1;
            cubxmin[1] = -1;
            sasfit_cubature(2,cubxmin,cubxmax,&opo_Fse_cub_dxy_core,pam,sasfit_eps_get_aniso(),&res,&err);
            break;
    }
    return res;
}

scalar opo_Fsupershape(void * pam) {
	sasfit_param * param;
	param = (sasfit_param *) pam;
    scalar cubxmin[3], cubxmax[3], fval[2], ferr[2];
    scalar *aw, res,err,sum;
    int intstrategy, lenaw=4000;
	int auswahl;
	intstrategy = sasfit_get_int_strategy();
//    intstrategy=OOURA_DOUBLE_EXP_QUADRATURE;
	switch(intstrategy) {

    case GSL_GAUSSLEGENDRE: {
            if (sasfit_eps_get_robertus_p()>0){
                sum = opo_Fss_GL_3Dtp(pam);
            } else {
                sum = opo_Fss_GL_3Drtp(pam);
            }
            break;
            }
    case H_CUBATURE: {
            if (sasfit_eps_get_robertus_p()>0){
                cubxmax[0] = M_PI;
                cubxmin[0] = -M_PI;
                cubxmax[1] = M_PI_2;
                cubxmin[1] = -M_PI_2;
                hcubature(2, &opo_Fss_cub_dtp,param,2, cubxmin, cubxmax,
                        100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED, fval, ferr);
                sum	= gsl_hypot(fval[0],fval[1]);
            } else {
                cubxmax[0] = 1;
                cubxmin[0] = 0;
                cubxmax[1] = M_PI;
                cubxmin[1] = -M_PI;
                cubxmax[2] = M_PI_2;
                cubxmin[2] = -M_PI_2;
                hcubature(2, &opo_Fss_cub_drtp,param,3, cubxmin, cubxmax,
                        100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED, fval, ferr);
                sum	= gsl_hypot(fval[0],fval[1]);
            }
            break;
            }
    case P_CUBATURE:
    default: {
            if (sasfit_eps_get_robertus_p()>0){
                cubxmax[0] = M_PI;
                cubxmin[0] = -M_PI;
                cubxmax[1] = M_PI_2;
                cubxmin[1] = -M_PI_2;
                pcubature(2, &opo_Fss_cub_dtp,param,2, cubxmin, cubxmax,
                        100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED, fval, ferr);
                sum	= gsl_hypot(fval[0],fval[1]);
            } else {
                cubxmax[0] = 1;
                cubxmin[0] = 0;
                cubxmax[1] = M_PI;
                cubxmin[1] = -M_PI;
                cubxmax[2] = M_PI_2;
                cubxmin[2] = -M_PI_2;
                pcubature(2, &opo_Fss_cub_drtp,param,3, cubxmin, cubxmax,
                        100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED, fval, ferr);
                sum	= gsl_hypot(fval[0],fval[1]);
            }
            }
    }
//    sasfit_out("Re=%lg,Im=%lg\t",fval[0],fval[1]);
	return sum;
}


scalar opo_Fsuper_toroid_helix(void * pam) {
	sasfit_param * param;
	param = (sasfit_param *) pam;
    scalar cubxmin[3], cubxmax[3], fval[2], ferr[2];
    scalar *aw, res,err,sum;
    int intstrategy, lenaw=4000;
	int auswahl;
	intstrategy = sasfit_get_int_strategy();
//    intstrategy=OOURA_DOUBLE_EXP_QUADRATURE;
	switch(intstrategy) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_aniso(), aw);
            sasfit_intde(&opo_Fsth_GL_3DtpOOURA_Re_dtheta, -TURNS*M_PI, TURNS*M_PI, aw, &res, &err, param);
            fval[0]=res;
            ferr[0]=err;
            sasfit_intde(&opo_Fsth_GL_3DtpOOURA_Im_dtheta, -TURNS*M_PI, TURNS*M_PI, aw, &res, &err, param);
            fval[1]=res;
            ferr[1]=err;
            free(aw);
            sum	= gsl_hypot(fval[0],fval[1]);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
            aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
            sasfit_intccini(lenaw, aw);
            sasfit_intcc(&opo_Fsth_GL_3DtpOOURA_Re_dtheta, -TURNS*M_PI, TURNS*M_PI, sasfit_eps_get_aniso(), lenaw, aw, &res, &err,param);
            fval[0]=res;
            ferr[0]=err;
            sasfit_intcc(&opo_Fsth_GL_3DtpOOURA_Im_dtheta, -TURNS*M_PI, TURNS*M_PI, sasfit_eps_get_aniso(), lenaw, aw, &res, &err,param);
            fval[1]=res;
            ferr[1]=err;
            free(aw);
            sum	= gsl_hypot(fval[0],fval[1]);
            break;
            }
    case GSL_GAUSSLEGENDRE: {
            if (sasfit_eps_get_robertus_p()>0){
                sum = opo_Fsth_GL_3Dtp(pam);
            } else {
                sum = opo_Fsth_GL_3Drtp(pam);
            }
            break;
            }
    case H_CUBATURE: {
            if (sasfit_eps_get_robertus_p()>0){
                cubxmax[0] = TURNS*M_PI;
                cubxmin[0] = -TURNS*M_PI;
                cubxmax[1] = M_PI;
                cubxmin[1] = -M_PI;
                hcubature(2, &opo_Fsth_cub_dtp,param,2, cubxmin, cubxmax,
                        100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED, fval, ferr);
                sum	= gsl_hypot(fval[0],fval[1]);
            } else {
                cubxmax[0] = 1;
                cubxmin[0] = 0;
                cubxmax[1] = TURNS*M_PI;
                cubxmin[1] = -TURNS*M_PI;
                cubxmax[2] = M_PI;
                cubxmin[2] = -M_PI;
                hcubature(2, &opo_Fsth_cub_drtp,param,3, cubxmin, cubxmax,
                        100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED, fval, ferr);
                sum	= gsl_hypot(fval[0],fval[1]);
            }
            break;
            }
    case P_CUBATURE:
    default: {
            if (sasfit_eps_get_robertus_p()>0){
                cubxmax[0] = TURNS*M_PI;
                cubxmin[0] = -TURNS*M_PI;
                cubxmax[1] = M_PI;
                cubxmin[1] = -M_PI;
                pcubature(2, &opo_Fsth_cub_dtp,param,2, cubxmin, cubxmax,
                        100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED, fval, ferr);
                sum	= gsl_hypot(fval[0],fval[1]);
            } else {
                cubxmax[0] = 1;
                cubxmin[0] = 0;
                cubxmax[1] = TURNS*M_PI;
                cubxmin[1] = -TURNS*M_PI;
                cubxmax[2] = M_PI;
                cubxmin[2] = -M_PI;
                pcubature(2, &opo_Fsth_cub_drtp,param,3, cubxmin, cubxmax,
                        100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED, fval, ferr);
                sum	= gsl_hypot(fval[0],fval[1]);
            }
            }
    }
//    sasfit_out("Re=%lg,Im=%lg\t",fval[0],fval[1]);
	return sum;
}

scalar opo_Fratsupershape(void * pam) {
	sasfit_param * param;
	param = (sasfit_param *) pam;
    scalar cubxmin[3], cubxmax[3], fval[2], ferr[2];
    scalar *aw, res,err,sum;
    int intstrategy, lenaw=4000;
	int auswahl;
	intstrategy = sasfit_get_int_strategy();
//    intstrategy=OOURA_DOUBLE_EXP_QUADRATURE;
	switch(intstrategy) {

    case GSL_GAUSSLEGENDRE: {
            if (sasfit_eps_get_robertus_p()>0){
                sum = opo_Fratss_GL_3Dtp(pam);
            } else {
                sum = opo_Fratss_GL_3Drtp(pam);
            }
            break;
            }
    case H_CUBATURE: {
            if (sasfit_eps_get_robertus_p()>0){
            cubxmax[0] = M_PI;
            cubxmin[0] = -M_PI;
            cubxmax[1] = M_PI_2;
            cubxmin[1] = -M_PI_2;
            hcubature(2, &opo_Fratss_cub_dtp,param,2, cubxmin, cubxmax,
                        100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED, fval, ferr);
            sum	= gsl_hypot(fval[0],fval[1]);
            } else {
            cubxmax[0] = 1;
            cubxmin[0] = 0;
            cubxmax[1] = M_PI;
            cubxmin[1] = -M_PI;
            cubxmax[2] = M_PI_2;
            cubxmin[2] = -M_PI_2;
            hcubature(2, &opo_Fratss_cub_drtp,param,3, cubxmin, cubxmax,
                        100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED, fval, ferr);
            sum	= gsl_hypot(fval[0],fval[1]);
            }
            break;
            }
    case P_CUBATURE:
    default: {
            if (sasfit_eps_get_robertus_p()>0){
            cubxmax[0] = M_PI;
            cubxmin[0] = -M_PI;
            cubxmax[1] = M_PI_2;
            cubxmin[1] = -M_PI_2;
            pcubature(2, &opo_Fratss_cub_dtp,param,2, cubxmin, cubxmax,
                        100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED, fval, ferr);
            sum	= gsl_hypot(fval[0],fval[1]);

            } else {
            cubxmax[0] = 1;
            cubxmin[0] = 0;
            cubxmax[1] = M_PI;
            cubxmin[1] = -M_PI;
            cubxmax[2] = M_PI_2;
            cubxmin[2] = -M_PI_2;
            pcubature(2, &opo_Fratss_cub_drtp,param,3, cubxmin, cubxmax,
                        100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED, fval, ferr);
            sum	= gsl_hypot(fval[0],fval[1]);
            }
            }
    }
//    sasfit_out("Re=%lg,Im=%lg\t",fval[0],fval[1]);
	return sum;
}
