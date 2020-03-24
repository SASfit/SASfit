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
    return sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
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
    Qxy = sqrt(opod->Qhat[0]*opod->Qhat[0]+opod->Qhat[1]*opod->Qhat[1]);
    return 8*M_PI*opod->detDinv
                *opo_sinc(opod->Qhat[2])
                *opo_J1x_x(Qxy);
}

scalar opo_Foctahedral(opo_data *opod) {
    scalar Qx, Qy, Qz, Qx2_Qy2, Qx2_Qz2, Qy2_Qz2;
    Qx = opod->Qhat[0];
    Qy = opod->Qhat[1];
    Qz = opod->Qhat[2];
	Qx2_Qy2 = Qx*Qx-Qy*Qy;
	Qx2_Qz2 = Qx*Qx-Qz*Qz;
	Qy2_Qz2 = Qy*Qy-Qz*Qz;
	if (Qx2_Qy2 == 0) {
		if (Qy == 0)  {
			if (Qz == 0)  {
				return opod->detDinv * 4./3.;
			} else {
				return opod->detDinv * 8. * (Qz - sin(Qz))/gsl_pow_3(Qz);
			}
		} else {
			if (Qy2_Qz2 ==0 ) {
				return opod->detDinv *
					(-Qz*cos(Qz) + (1 + Qz*Qz)*sin(Qz))/(Qz*Qz*Qz);
			} else {
				return opod->detDinv *
					4*(Qy*(-Qy*Qy + Qz*Qz)*cos(Qy) + (Qy*Qy + Qz*Qz)*sin(Qy) -2*Qy*Qz*sin(Qz)) /
							(Qy*gsl_pow_2(Qy2_Qz2));
			}
		}
	}

	if (Qx2_Qz2 ==0) {
		if (Qz==0)  {
			if (Qy == 0)  {
				return opod->detDinv * 4./3.;
			} else {
				return opod->detDinv * 8. * (Qy - sin(Qy))/gsl_pow_3(Qy);
			}
		} else {
			if (Qy2_Qz2 ==0 ) {
				return opod->detDinv *
					(-Qz*cos(Qz) + (1 + Qz*Qz)*sin(Qz))/(Qz*Qz*Qz);
			} else {
				return opod->detDinv *
					(4*(Qz*(Qy*Qy - Qz*Qz)*cos(Qz) - 2*Qy*Qz*sin(Qy) + (Qy*Qy + Qz*Qz)*sin(Qz)) )/
						(Qz*gsl_pow_2(Qy2_Qz2));
			}
		}
	}

	if (Qy2_Qz2 ==0) {
		if (Qz==0)  {
			if (Qz == 0)  {
				return opod->detDinv * 4./3.;
			} else {
				return opod->detDinv * 8. * (Qy - sin(Qy))/gsl_pow_3(Qy);
			}
		}else {
			if (Qx2_Qz2 ==0 ) {
				return opod->detDinv *
					(-Qz*cos(Qz) + (1 + Qz*Qz)*sin(Qz))/(Qz*Qz*Qz);
			} else {
				return opod->detDinv *
				(4*(Qz*(Qx*Qx - Qz*Qz)*cos(Qz) - 2*Qx*Qz*sin(Qx) + (Qx*Qx + Qz*Qz)*sin(Qz)) )/
					(Qz*gsl_pow_2(Qx2_Qz2));
			}
		}
	}

    return opod->detDinv /(Qx*Qx - Qy*Qy)*(
	           (8*Qy*sin(Qy) - 8*Qz*sin(Qz))/(Qy*Qy - Qz*Qz)
			 + (8*Qz*sin(Qz) - 8*Qx*sin(Qx))/(Qx*Qx - Qz*Qz));
}

scalar opo_Fprism3(opo_data *opod) {
    scalar Qx, Qy, Qz, Qx2_Qy2, Qx2_Qz2, Qy2_Qz2;
    Qx = opod->Qhat[0];
    Qy = opod->Qhat[1];
    Qz = opod->Qhat[2];
}

scalar opo_Fprism6(opo_data *opod) {
    scalar Qx, Qy, Qz, Qx2_Qy2, Qx2_Qz2, Qy2_Qz2;
    Qx = opod->Qhat[0];
    Qy = opod->Qhat[1];
    Qz = opod->Qhat[2];
}

scalar opo_Fcone(opo_data *opod) {
    scalar Qx, Qy, Qz, Qx2_Qy2, Qx2_Qz2, Qy2_Qz2;
    Qx = opod->Qhat[0];
    Qy = opod->Qhat[1];
    Qz = opod->Qhat[2];
}

scalar opo_Fcone6(opo_data *opod) {
    scalar Qx, Qy, Qz, Qx2_Qy2, Qx2_Qz2, Qy2_Qz2;
    Qx = opod->Qhat[0];
    Qy = opod->Qhat[1];
    Qz = opod->Qhat[2];
}

scalar opo_Fpyramid(opo_data *opod) {
    scalar Qx, Qy, Qz, Qx2_Qy2, Qx2_Qz2, Qy2_Qz2;
    Qx = opod->Qhat[0];
    Qy = opod->Qhat[1];
    Qz = opod->Qhat[2];
}

scalar opo_Ftetrahedron(opo_data *opod) {
    scalar Qx, Qy, Qz, Qx2_Qy2, Qx2_Qz2, Qy2_Qz2;
    Qx = opod->Qhat[0];
    Qy = opod->Qhat[1];
    Qz = opod->Qhat[2];
}

scalar opo_Fcubooctahedron(opo_data *opod) {
    scalar Qx, Qy, Qz, Qx2_Qy2, Qx2_Qz2, Qy2_Qz2;
    Qx = opod->Qhat[0];
    Qy = opod->Qhat[1];
    Qz = opod->Qhat[2];
}

int opo_Fse_cub_dxyz(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
    scalar SEimplicit;
	sasfit_param * param;
	param = (sasfit_param *) pam;
	if ((ndim < 3) || (fdim < 2)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	XX = x[0];
	YY = x[1];
	ZZ = x[2];
	if (fabs(XX)>1 || fabs(YY)>1 || fabs(ZZ)>1) {
		fval[0] = 0;
		fval[1] = 0;
		return 0;
	}
	SEimplicit = pow(pow(gsl_pow_2(XX),1/QQ) + pow(gsl_pow_2(YY),1/QQ),QQ/PP) + pow(gsl_pow_2(ZZ),1/PP)-1;
	if (SEimplicit>0) {
		fval[0] = 0;
		fval[1] = 0;
		return 0;
	} else {
		fval[0] = cos(QQX*XX + QQY*YY + QQZ*ZZ);
		fval[1] = sin(QQX*XX + QQY*YY + QQZ*ZZ);
		return 0;
	}
	return 1;
}

int opo_Fse_cub_dru(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
    scalar DJ2D,r,u,w;
	sasfit_param * param;
	param = (sasfit_param *) pam;
	if ((ndim < 2) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	r=x[0];
	u=x[1];
	XX = r*pow(gsl_pow_2(cos(u)),PP/2)*GSL_SIGN(cos(u));
	YY = r*pow(gsl_pow_2(sin(u)),PP/2)*GSL_SIGN(sin(u));
	DJ2D=PP*r*pow(fabs(cos(u)*sin(u)),PP/2);
	w = pow(1 - pow(pow(gsl_pow_2(XX),1/PP) + pow(gsl_pow_2(YY),1/PP),PP/QQ),QQ/2.);
    fval[0] = DJ2D*(2*cos(QQX*XX+ QQY*YY)*w*opo_sinc(QQZ*w));
	return 0;
}

double opo_Fse_G962D_dxy(double x, double y, void *pam) {
	sasfit_param * param;
	param = (sasfit_param *) pam;
    scalar w;
	if (fabs(x)>1 || fabs(y)>1 || fabs(y)>pow(1-pow(x*x,1/PP),PP/2)) {
		return 0;
	}
    w = pow(1 - pow(pow(gsl_pow_2(x),1/PP) + pow(gsl_pow_2(y),1/PP),PP/QQ),QQ/2.);
    return (2*cos(QQX*x+ QQY*y)*w*opo_sinc(QQZ*w));
}

scalar opo_Fse_GQ962D(void *pam)
  {
	sasfit_param * param;
	param = (sasfit_param *) pam;
      /* 96x96-pt 2-D Gauss qaudrature integrates
               F(x,y) from y=Y1(x) to Y2(x) and x=a to b */
  int i,j,k;
  double cx,cy,dx,dy,q,w,x,y1,y2,a,b;
  double A96[96]={                   /* abscissas for 96-point Gauss quadrature */
  0.016276744849603,0.048812985136050,0.081297495464426,0.113695850110666,
  0.145973714654897,0.178096882367619,0.210031310460567,0.241743156163840,
  0.273198812591049,0.304364944354496,0.335208522892625,0.365696861472314,
  0.395797649828909,0.425478988407301,0.454709422167743,0.483457973920596,
  0.511694177154668,0.539388108324358,0.566510418561397,0.593032364777572,
  0.618925840125469,0.644163403784967,0.668718310043916,0.692564536642172,
  0.715676812348968,0.738030643744400,0.759602341176648,0.780369043867433,
  0.800308744139141,0.819400310737932,0.837623511228187,0.854959033434602,
  0.871388505909297,0.886894517402421,0.901460635315852,0.915071423120898,
  0.927712456722309,0.939370339752755,0.950032717784438,0.959688291448743,
  0.968326828463264,0.975939174585137,0.982517263563015,0.988054126329624,
  0.992543900323763,0.995981842987209,0.998364375863182,0.999689503883231};

  double W96[96]={                     /* weights for 96-point Gauss quadrature */
  0.032550614492363,0.032516118713869,0.032447163714064,0.032343822568576,
  0.032206204794030,0.032034456231993,0.031828758894411,0.031589330770727,
  0.031316425596861,0.031010332586314,0.030671376123669,0.030299915420828,
  0.029896344136328,0.029461089958168,0.028994614150555,0.028497411065085,
  0.027970007616848,0.027412962726029,0.026826866725592,0.026212340735672,
  0.025570036005349,0.024900633222484,0.024204841792365,0.023483399085926,
  0.022737069658329,0.021966644438744,0.021172939892191,0.020356797154333,
  0.019519081140145,0.018660679627411,0.017782502316045,0.016885479864245,
  0.015970562902562,0.015038721026995,0.014090941772315,0.013128229566962,
  0.012151604671088,0.011162102099839,0.010160770535008,0.009148671230783,
  0.008126876925699,0.007096470791154,0.006058545504236,0.005014202742928,
  0.003964554338445,0.002910731817935,0.001853960788947,0.000796792065552};

  a = -1;
  b = 1;
  cx=(a+b)/2;
  dx=(b-a)/2;
  q=0;
  for(i=0;i<48;i++)
    {
    for(k=-1;k<=1;k+=2)
      {
      x=cx+k*dx*A96[i];
      y1=pow(1-pow(x*x,1/PP),PP/2);
      y2=-y1;
      cy=(y1+y2)/2;
      dy=(y2-y1)/2;
      w=dy*W96[i];
      for(j=0;j<48;j++)
        q+=w*W96[j]*(opo_Fse_G962D_dxy(x,cy-dy*A96[j],pam)+opo_Fse_G962D_dxy(x,cy+dy*A96[j],pam));
      }
    }
  return(q*dx);
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

scalar opo_Fse_kernel_dy(scalar y, void *pam) {
    scalar w;
	sasfit_param * param;
	param = (sasfit_param *) pam;
	w = pow(1 - pow(pow(gsl_pow_2(XX),1/PP) + pow(gsl_pow_2(y),1/PP),PP/QQ),QQ/2.);
	return (2*cos(QQX*XX+ QQY*y)*w*opo_sinc(QQZ*w));
}
scalar opo_Fse_core_dy(scalar y, sasfit_param * param) {
	return opo_Fse_kernel_dy(y,param);
}

int opo_Fse_cub_dy(unsigned ndim, const double *y, void *pam,
      unsigned fdim, double *fval) {
	sasfit_param * param;
	param = (sasfit_param *) pam;
	if ((ndim < 1) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	fval[0]=opo_Fse_kernel_dy(y[0],param);
	return 0;
}

scalar opo_Fse_kernel_dx(scalar x, void * pam) {
	sasfit_param * param;
	param = (sasfit_param *) pam;
    scalar cubxmin[1], cubxmax[1], fval[1], ferr[1];
    scalar *aw, res,err,sum;
    int intstrategy, lenaw=4000;
	cubxmax[0] = pow(1-pow(x*x,1/PP),PP/2);
	cubxmin[0] = -cubxmax[0];
	if (cubxmax[0] == cubxmin[0]) return 0;
	XX = x;
	intstrategy = sasfit_get_int_strategy();
    intstrategy=OOURA_DOUBLE_EXP_QUADRATURE;
	switch(intstrategy) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_aniso(), aw);
            sasfit_intde(&opo_Fse_kernel_dy,cubxmin[0], cubxmax[0], aw, &res, &err, param);
            sum = res;
            free(aw);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
            aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
            sasfit_intccini(lenaw, aw);
            sasfit_intcc(&opo_Fse_kernel_dy,cubxmin[0], cubxmax[0], sasfit_eps_get_aniso(), lenaw, aw, &res, &err,param);
            sum = res;
            free(aw);
            break;
            }
    case H_CUBATURE: {
            hcubature(1, &opo_Fse_cub_dy,param,1, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED,
				fval, ferr);
            sum	= fval[0];
            break;
            }
    case P_CUBATURE: {
            pcubature(1, &opo_Fse_cub_dy,param,1, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED,
				fval, ferr);
            sum	= fval[0];
            break;
            }
    default: {
            sum=sasfit_integrate(cubxmin[0], cubxmax[0], opo_Fse_core_dy, param);
            break;
            }
    }
	return sum;
}

scalar opo_Fse_core_dx(scalar x, sasfit_param * param) {
	return opo_Fse_kernel_dx(x,param);
}

int opo_Fse_cub_dx(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
	sasfit_param * param;
	param = (sasfit_param *) pam;
	if ((ndim < 1) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	fval[0]=opo_Fse_kernel_dx(x[0],param);
	return 0;
}

scalar opo_Fsuperellipsoid(void * pam) {
	sasfit_param * param;
	param = (sasfit_param *) pam;
    scalar cubxmin[3], cubxmax[3], fval[2], ferr[2];
    scalar *aw, res,err,sum;
    int intstrategy, lenaw=4000;
	cubxmax[0] = 1;
	cubxmin[0] = 0;
	if (cubxmax[0] == cubxmin[0]) return 0;
	intstrategy = sasfit_get_int_strategy();
    intstrategy=OOURA_DOUBLE_EXP_QUADRATURE;
	switch(intstrategy) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
            cubxmax[0] = 1;
            if (sasfit_eps_get_robertus_p()>0) {
                cubxmin[0] = 0;
            } else {
                cubxmin[0] = -1;
            }
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_aniso(), aw);
            sasfit_intde(&opo_Fse_kernel_dx,cubxmin[0], cubxmax[0], aw, &res, &err, param);
            sum = (2+cubxmin[0])*res;
            free(aw);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
            cubxmax[0] = 1;
            if (sasfit_eps_get_robertus_p()>0) {
                cubxmin[0] = 0;
            } else {
                cubxmin[0] = -1;
            }
            aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
            sasfit_intccini(lenaw, aw);
            sasfit_intcc(&opo_Fse_kernel_dx,cubxmin[0], cubxmax[0], sasfit_eps_get_aniso(), lenaw, aw, &res, &err,param);
            sum = (2+cubxmin[0])*res;
            free(aw);
            break;
            }
    case H_CUBATURE: {
            switch (abs(sasfit_eps_get_robertus_p())) {
                case 1: {
                    cubxmax[0] = 1;
                    if (sasfit_eps_get_robertus_p()>0) {
                        cubxmin[0] = 0;
                    } else {
                        cubxmin[0] = -1;
                    }
                    hcubature(1, &opo_Fse_cub_dx,param,1, cubxmin, cubxmax,
                        100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED,
                        fval, ferr);
                    sum	= (2+cubxmin[0])*fval[0];
                    break;
                    }
                case 2: {
                    cubxmax[0] = 1;
                    if (sasfit_eps_get_robertus_p()>0) {
                        cubxmin[0] = 0;
                    } else {
                        cubxmin[0] = -1;
                    }
                    cubxmax[1] = 1;
                    cubxmin[1] = -1;
                    hcubature(1, &opo_Fse_cub_dxy,param,2, cubxmin, cubxmax,
                        100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED,
                    fval, ferr);
                    sum	= (2+cubxmin[0])*fval[0];
                    break;
                    }
                case 3:{
                    cubxmax[0] = 1;
                    cubxmin[0] = 0;
                    cubxmax[1] = M_PI_4;
                    cubxmin[1] = 0;
                    hcubature(1, &opo_Fse_cub_dru,param,2, cubxmin, cubxmax,
                        100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED,
                    fval, ferr);
                    sum	= 4*fval[0];
                    break;
                    }
                default: {
                    cubxmax[0] = 1;
                    cubxmin[0] = -1;
                    cubxmax[1] = 1;
                    cubxmin[1] = -1;
                    cubxmax[2] = 1;
                    cubxmin[2] = -1;
                    hcubature(2, &opo_Fse_cub_dxyz,param,3, cubxmin, cubxmax,
                        100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
                        fval, ferr);
                    sum	= gsl_hypot(fval[0],fval[1]);
                    break;
                    }
            }
            }
    case P_CUBATURE: {
            switch (abs(sasfit_eps_get_robertus_p())) {
                case 1: {
                    cubxmax[0] = 1;
                    if (sasfit_eps_get_robertus_p()>0) {
                        cubxmin[0] = 0;
                    } else {
                        cubxmin[0] = -1;
                    }
                    pcubature(1, &opo_Fse_cub_dx,param,1, cubxmin, cubxmax,
                        100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
                        fval, ferr);
                    sum	= (2+cubxmin[0])*fval[0];
                    break;
                    }
                case 2: {
                    cubxmax[0] = 1;
                    if (sasfit_eps_get_robertus_p()>0) {
                        cubxmin[0] = 0;
                    } else {
                        cubxmin[0] = -1;
                    }
                    cubxmax[1] = 1;
                    cubxmin[1] = -1;
                    pcubature(1, &opo_Fse_cub_dxy,param,2, cubxmin, cubxmax,
                        100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED,
                    fval, ferr);
                    sum	= (2+cubxmin[0])*fval[0];
                    break;
                    }
                case 3: {
                    cubxmax[0] = 1;
                    cubxmin[0] = 0;
                    cubxmax[1] = M_PI_4;
                    cubxmin[1] = 0;
                    pcubature(1, &opo_Fse_cub_dru,param,2, cubxmin, cubxmax,
                        100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED,
                    fval, ferr);
                    sum	= 4*fval[0];
                    break;
                    }
                default: {
                    cubxmax[0] = 1;
                    cubxmin[0] = -1;
                    cubxmax[1] = 1;
                    cubxmin[1] = -1;
                    cubxmax[2] = 1;
                    cubxmin[2] = -1;
                    pcubature(2, &opo_Fse_cub_dxyz,param,3, cubxmin, cubxmax,
                        100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED,
                        fval, ferr);
                    sum	= gsl_hypot(fval[0],fval[1]);
                    break;
                    }
            }
            }
    case GSL_CQUAD: {
            sum = opo_Fse_GQ962D(pam);
            break;
            }
    default: {
            cubxmax[0] = 1;
            if (sasfit_eps_get_robertus_p()>0) {
                cubxmin[0] = 0;
            } else {
                cubxmin[0] = -1;
            }
            sum=(2+cubxmin[0])*sasfit_integrate(cubxmin[0], cubxmax[0], opo_Fse_core_dx, param);
            break;
            }
    }
	return sum;
}
