#include <float.h>
#include <complex.h>
typedef enum
{
//  Proper Euler angles
    X1_Z2_X3,
    X1_Y2_X3,
    Y1_X2_Y3,
    Y1_Z2_Y3,
	Z1_X2_Z3,
             // x-convention (z,x',z'')
             // alpha=EulerAngles[0], beta =EulerAngles[1], gamma=EulerAngles[2]
             // phi  =EulerAngles[0], theta=EulerAngles[1], psi  =EulerAngles[2]
    Z1_Y2_Z3,
             // y-convention (z,y',z'')
             // gamma=EulerAngles[0], beta =EulerAngles[1], alpha=EulerAngles[2]
             // phi  =EulerAngles[0], theta=EulerAngles[1], psi  =EulerAngles[2]

//  Tait-Bryan angles
    X1_Z2_Y3,
    X1_Y2_Z3,
    Y1_X2_Z3,
    Y1_Z2_X3,
    Z1_Y2_X3,
             // alpha=EulerAngles[0], beta =EulerAngles[1], gamma=EulerAngles[2]
             // alpha=yaw=Gier
             // beta=pitch=Nick
             // gama=roll=Roll
    Z1_X2_Y3
} opo_Euler_convention_t;

#define x_convention Z1_X2_Z3
#define y_convention Z1_Y2_Z3
#define yaw_pitch_roll Z1_Y2_X3
#define Gier_Nick_Roll Z1_Y2_X3
#define East_North_Up Z1_Y2_X3
#define North_East_Down Z1_Y2_X3

typedef struct {
    opo_Euler_convention_t convention;
    double RotationMatrix[3][3];
    double RotationMatrixT[3][3];
    double EulerAngles[3];
} opo_rot;

typedef struct {
    double a,b,c;
    double ea[3];
    double eb[3];
    double ec[3];
    double Rot_ea[3];
    double Rot_eb[3];
    double Rot_ec[3];
    double D[3][3];
    double Dinv[3][3];
    double Q[3];
    double Qhat[3];
    double Qmod;
    double detDinv;
    double detD;
    opo_rot Rotation;
    opo_rot RotationInv;
    sasfit_param * param;
} opo_data;

int opo_set_e(double *, double, double, double);
void opo_setConvention(opo_data *, opo_Euler_convention_t );
void opo_setEulerAngles(opo_data *, double , double , double );
void opo_setRotationMatrix(opo_data *);
void opo_setDetDinv(opo_data*);
void opo_setDetD(opo_data*);
void opo_setQhat(opo_data*);
void opo_RotateAxis(opo_data*);
int opo_check_orthogonal(opo_data *);
scalar opo_Fe(opo_data *);
scalar opo_Fp(opo_data *);
scalar opo_Fc(opo_data *);
scalar Fcone_kernel_Re(scalar, sasfit_param *);
scalar Fcone_kernel_Im(scalar, sasfit_param *);
scalar opo_Fcone(opo_data *);
scalar Fcone6_kernel_Re(scalar, sasfit_param *);
scalar Fcone6_kernel_Im(scalar, sasfit_param *);
scalar opo_Fcone6(opo_data *);
scalar Ftetrahedron_kernel_Re(opo_data *);
scalar Ftetrahedron_kernel_Im(opo_data *);
scalar opo_Ftetrahedron(opo_data *);
scalar Fpyramid4_kernel_Re(scalar, sasfit_param *);
scalar Fpyramid4_kernel_Im(scalar, sasfit_param *);
scalar opo_Fpyramid4(opo_data *);
scalar opo_Fpyramid4_Re(opo_data *);
scalar opo_Fpyramid4_Im(opo_data *);
scalar opo_ReFpyramid4(scalar, scalar, scalar, sasfit_param *);
scalar opo_ImFpyramid4(scalar, scalar, scalar, sasfit_param *);
double complex opo_CmplxFpyramid4(scalar, scalar, scalar, sasfit_param *);
scalar opo_Fsuperellipsoid(void *);
scalar opo_Fsupershape(void *);
scalar opo_Fratsupershape(void *);
scalar opo_Fsuper_toroid_helix(void *);
scalar opo_Fse_GQ962D(void *);
scalar opo_FOH(opo_data *);
scalar opo_Cot(scalar);
scalar opo_sinc(scalar);
void opo_init(opo_data *);
