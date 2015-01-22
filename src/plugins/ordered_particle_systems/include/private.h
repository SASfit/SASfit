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

#define PHI         ospparam->phi
#define THETA       ospparam->theta
#define PSI         ospparam->psi

#define ALPHA       ospparam->alpha
#define BETA        ospparam->beta
#define GAMMA       ospparam->gamma

#define MR          ospparam->mR
#define MM          ospparam->mM
#define MN          ospparam->mN
#define MAXHKL      ospparam->maxhkl

#define VUC         ospparam->vd     // volume of unit cell

#define PEAKSHAPE   ospParameter.peak_shape_type_Selector
#define ORDERTYPE   ospParameter.order_type_Selector
#define QMODVAL     ospParameter.Qmod
#define LAMBDAVAL   ospParameter.wavelength    // wavelength of the beam
#define VARPHIVAL   ospParameter.varphi
#define VARTHETAVAL ospParameter.vartheta
#define ADVAL       ospParameter.ad
#define DELTAVAL    ospParameter.delta
#define DELTAQVAL   ospParameter.delta_q
#define DELTAPSIVAL ospParameter.delta_psi
#define CLVAL       ospParameter.cL
#define ALPHAVAL    ospParameter.alpha
#define BETAVAL     ospParameter.beta
#define GAMMAVAL    ospParameter.gamma
#define MAXHKLVAL   ospParameter.maxhkl

/**
 * A set of parameters used by every model function in sasfit.
 */

typedef enum
{
	SC,
	BCC,
	BCCTWIN,
	FCC,
	FCCTWIN,
	HCP,
	HCPTWIN,
	BCT,
	HEX,
	SQ,
	CREC,
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
    scalar     a[3];        // direct space bases vectors
    scalar     b[3];
    scalar     c[3];
    scalar     ast[3];      // reciprocal space basis vectors
    scalar     bst[3];
    scalar     cst[3];
    scalar     ad, bd, cd;  // lattice constants
    scalar     Vd;          // volume of unit cell
    scalar     vd;          // projected d-dimension volume
    scalar     dim;         // dimension of periodic structure
    scalar     n;           // number of particle per unit cell
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
	scalar     mR[3][3];        //
	scalar     mM[3][3],mMT[3][3],mN[3][3],mNT[3][3], mNTm1[3][3], mMTm1[3][3];
	scalar     psi_hkl, abar;
	scalar     delta;       // width parameter of peak function
	scalar     delta_q;     // width parameter for radial peak width of anisotropic and aligned structures
	scalar     delta_psi;   // width parameter for ayimuthal peak width of anisotropic and aligned structures
	scalar     nu;          // shape parameter for peak function
	int        f_hkl, m_hkl, maxhkl;
	order_type       order_type_Selector;
	peak_shape_type  peak_shape_type_Selector;
} ordered_particles_param;

ordered_particles_param ospParameter;

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
#endif // end of file

