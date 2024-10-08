/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef ORDERED_PARTICLE_SYSTEMS_PRIVATE_H
#define ORDERED_PARTICLE_SYSTEMS_PRIVATE_H

/*
 * Header file for the implementation of the form factor itself.
 */

// optional, depends on form factor implementation
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>

// mandatory, no adjustments necessary
#include <sasfit_common.h>

// mandatory, no adjustments necessary
#ifdef MAKE_SASFIT_PLUGIN
  // mandatory, no adjustments necessary
  #include <sasfit_plugin.h>

  SASFIT_PLUGIN_INFO_DECL;

#else


#endif

// adjust according to the plugins name
#include "sasfit_ordered_particle_systems.h"

//
// add local defines here:
#define PAD         param->p[0]
#define PBD         param->p[1]
#define PCD         param->p[2]

#define SIGMA_A     param->p[3]
#define PDELTA      param->p[4]
#define PDELTAQ     param->p[4]

#define PDELTAPSI   param->p[5]
#define PCL         param->p[6]
#define PYAW        param->p[7]
#define PPITCH      param->p[8]
#define PROLL       param->p[9]
#define PSI_DEG     param->p[10]
#define PLAMBDA     param->p[11]
#define PMAXHKL     param->p[12]
#define PNU     param->p[13]


#define KI          ospparam->ki
#define KS          ospparam->ks
#define LAMBDA      ospparam->wavelength    // wavelength of the beam
#define VARPHI      ospparam->varphi
#define VARTHETA    ospparam->vartheta
#define Q           ospparam->q
#define QMOD        ospparam->Qmod
#define L           ospparam->l             // distance vector on detector
#define LMOD        ospparam->lmod
#define D           ospparam->d             // detector distance
#define ABAR        ospparam->abar
#define CL          ospparam->cL
#define VARNU          ospparam->nu

#define PHI         ospparam->phi
#define THETA       ospparam->theta
#define PSI         ospparam->psi

#define ALPHA       ospparam->alpha
#define BETA        ospparam->beta
#define GAMMA       ospparam->gamma

#define MR         ospparam->EulerR.RotationMatrix
#define MRI        ospparam->EulerR.RotationMatrixT
#define MM        ospparam->mM
#define MN         ospparam->mN
#define MAXHKL      ospparam->maxhkl

#define VUC         ospparam->vd     // volume of unit cell

#define PEAKSHAPE   ospParameter.peak_shape_type_Selector
#define ORDERTYPE   ospParameter.order_type_Selector
#define QMODVAL     ospParameter.Qmod
#define LAMBDAVAL   ospParameter.wavelength    // wavelength of the beam
#define VARPHIVAL   ospParameter.varphi
#define VARTHETAVAL ospParameter.vartheta

#define ADVAL       ospParameter.ad
#define BDVAL       ospParameter.bd
#define CDVAL       ospParameter.cd

#define DELTAVAL    ospParameter.delta
#define DELTAQVAL   ospParameter.delta_q
#define DELTAPSIVAL ospParameter.delta_psi
#define CLVAL       ospParameter.cL
#define ALPHAVAL    ospParameter.alpha
#define BETAVAL     ospParameter.beta
#define GAMMAVAL    ospParameter.gamma
#define MAXHKLVAL   ospParameter.maxhkl
#define NUVAL   ospParameter.nu

/**
 * A set of parameters used by every model function in sasfit.
 */


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
} ops_Euler_convention_t;

//    #define DEFAULT_CONVENTION Z1_X2_Z3
    #define DEFAULT_CONVENTION Z1_Y2_X3

typedef struct {
    ops_Euler_convention_t convention;
    double RotationMatrix[3][3];
    double RotationMatrixT[3][3];
    double EulerAngles[3];
} ops_rot;

typedef enum
{
	SC, Pm3m,
	BCC, Im3m,
	BCCTWIN,
	FCC, Fm3m,
	FCCTWIN,
	HCP, P6_mmc,
	HCPTWIN,
	BCT, I4_mm,
	HEX, P6_mm,
	SQ, P4_mm,
	CREC, cmm2,
	LAM,
	la3d,
	lm3m,
	Pn3m
} order_type;

typedef enum
{
	LORENTZIANPEAK,
	GAUSSIANPEAK,
	BURGERMICHA,
	PSEUDOVOIGT,
	PEARSON
} peak_shape_type;


typedef struct
{
    scalar     wavelength;
    scalar     ex[3];
    scalar     ey[3];
    scalar     ez[3];
    scalar     ecx[3];
    scalar     ecy[3];
    scalar     ecz[3];
    scalar     ad, bd, cd;  // lattice constants
    scalar     a[3];        // direct space bases vectors
    scalar     b[3];
    scalar     c[3];
    scalar     lalpha;      // angle betwee  b and c
    scalar     lbeta;        // agle betwee c and a
    scalar     lgamma;   //  angle between a and b
    scalar     n;             // number of particle per unit cell
    scalar     positions[4][3];  // position of particles in unit cell in terms of direct cell . Maximum allowed positions aree 4 at the moment
    scalar     ast[3];      // reciprocal space basis vectors
    scalar     bst[3];
    scalar     cst[3];
    scalar     Vd;          // volume of unit cell
    scalar     vd;          // projected d-dimension volume
    int        dim;         // dimension of periodic structure
    scalar     Omega;       // Solid Angle
    scalar     cL;          // constant of order unity whose exact value is calculated using the Porod invariant
    scalar     fphi;        // particle volume fraction
    scalar     fphimax;     // maximum volume fraction

	scalar     ki[3];   // incident wave vector
	scalar     ks[3];   // scattered wave vector
	scalar     q[3];    // scattering vector
	scalar     Qmod;    // modulus of q
	scalar     ghkl[3]; // lattice vectors
	scalar     ghklmod; // modulus of lattice vectors
	scalar     d;       // detector distance
	scalar     l[3];    // distance vector on detector
	scalar     lmod;    // sample to pixel distance
	scalar     varphi, vartheta;    // polar coordinates of scattering and wave vectors

	scalar     phi,theta,psi;       // Euler coordinate system using the "x-convention“ (z, x’, z’’)
	scalar     alpha, beta, gamma;  // Euler coordinate system using the "z, y′, x''-convention“ (z, y’, x’’)
                                    // this coordinate system is also called Tait-Bryan angles and are also known as nautical angles
                                    // (alpha,      beta,       gamma),
                                    // (phi,        theta,      psi),
                                    // (yaw,        pitch,      roll) angle,
                                    // (Gier-,      Nick-,      Roll-) Winkel,
                                    // (heading,    elevation,  bank) angle
                                    //  (-pi:pi], [-pi/2:pi/2], (-pi:pi]
	ops_rot  EulerR;     //
	scalar     mM[3][3],mMT[3][3],mN[3][3],mNT[3][3], mNTm1[3][3], mMTm1[3][3];
	scalar     psi_hkl, abar;
	scalar     delta;       // width parameter of peak function
	scalar     delta_q;     // width parameter for radial peak width of anisotropic and aligned structures
	scalar     delta_psi;   // width parameter for ayimuthal peak width of anisotropic and aligned structures
	scalar     nu;          // shape parameter for peak function
	int        m_hkl, maxhkl;
    scalar     f_hkl;
	order_type       order_type_Selector;
	peak_shape_type  peak_shape_type_Selector;
} ordered_particles_param;

static ordered_particles_param ospParameter;

void set_ki(ordered_particles_param *, sasfit_param *);
void set_ks(ordered_particles_param *, sasfit_param *);
void set_l (ordered_particles_param *, sasfit_param *);
void set_q (ordered_particles_param *, sasfit_param *);
void set_R (ordered_particles_param *, sasfit_param *);
void abar  (ordered_particles_param *, sasfit_param *);
void f_hkl   (int, int, int, ordered_particles_param *, sasfit_param *);
void psi_hkl (int, int, int, ordered_particles_param *, sasfit_param *);
scalar G(ordered_particles_param *, sasfit_param *);
scalar Lattice_Factor_iso(ordered_particles_param *, sasfit_param *);
scalar Lattice_Factor_aniso(ordered_particles_param *, sasfit_param *);
scalar K(scalar, ordered_particles_param *, sasfit_param *);
void ops_setEulerAngles(ordered_particles_param *, double , double ,double );
void ops_setConvention (ordered_particles_param *, ops_Euler_convention_t);
void init_osp(ordered_particles_param *, sasfit_param *);
#endif // end of file

