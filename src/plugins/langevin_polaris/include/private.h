/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef LANGEVIN_POLARIS_PRIVATE_H
#define LANGEVIN_POLARIS_PRIVATE_H

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
#include "sasfit_langevin_polaris.h"
//#include "sasfit_langevin_polaris2.h"

#define B0              param->p[0]		// INPUT VAR: amplitude of oscillating field
#define B1              param->p[1]		// INPUT VAR: offset of oscillating magnetic field
#define NU              param->p[2]		// INPUT_VAR: frequency of oscillating magn. field
#define RHO_0           param->p[3]		// INPUT VAR: instrumental phase shift
#define PHASE_PAR       param->p[4]		// INPUT VAR: phase shift parameter
#define PSI_DEG         param->p[5]		// INPUT VAR: angle between scattering vector q and magnetic field B in degree
#define SQ_AMPL         param->p[6]		// INPUT VAR: structure factor amplitude (1 for no structure factor)
#define SQ_PHASE        param->p[7]		// INPUT VAR: phase shift of structure factor
#define SQ_STATIC       param->p[8]		// INPUT VAR: value between [0,1] to describe if the structure factor changes with the magnetic field (SQ_STATIC=0) or not (SQ_STATIC=1)
#define FNUC_FMAG       param->p[9]		// INPUT VAR: ratio between nuclear and magnetic scattering length density contrast
#define MU_KT			param->p[10]	// INPUT VAR: ratio between magnetic moment of particle and kT
#define LAM_CENTER      param->p[11]	// INPUT VAR: neutron wave length in Angstrom
#define D_LAM_LAM       param->p[12]	// INPUT VAR: wavelength spread
#define SD              param->p[13]	// INPUT VAR: sample detector distance
#define P_POL           param->p[14]	// INPUT VAR:	polarization of the incident neutron beam
#define EPSILON_FLIPPER param->p[15]	// VAR: spin flipper efficiency
#define T_PLUS          param->p[16]	// VAR: Transmission of analyser for spin up neutrons 
#define T_MINUS         param->p[17]	// VAR: Transmission of analyser for spin down neutrons
#define B_STATIC        param->p[18]	// VAR: effective field describing the orientation distribution of the fozen moments
#define B_STAT_PHI_DEG	param->p[19]	// VAR: half angle between the two direction of the static field relative to the oscillating field

#define DELTA_T_BOTTOM	param->p[MAXPAR-20]	// VAR: 
#define DELTA_T_TOP		param->p[MAXPAR-19]	// VAR: 
#define ALPHA_PHASE     param->p[MAXPAR-18]	// VAR: phase shift of Langevin parameter
#define LAMBDA			param->p[MAXPAR-17]	// actual wave length used within integration over wave length distribution
#define MU              param->p[MAXPAR-16]	// VAR: magnetic moment of particle
#define DAMP_ALPHA      param->p[MAXPAR-15]	// VAR: damping factor for Langevin factor due to phase shift
#define SQ_TIME         param->p[MAXPAR-14]	// VAR: time dependent structure factor contribution
#define TEMPERATURE     param->p[MAXPAR-13]	// VAR: sample temperature in K
#define AD              param->p[MAXPAR-12]	// VAR: parameter if spin can adiabatically follow (=1) the oscillating field or not (=0)
#define AD_FUNC         param->p[MAXPAR-11]	// VAR: r_ad[0] = (1-ad)*sign(B(t))
#define NONAD_FUNC      param->p[MAXPAR-10]	// VAR: r_ad[1] = ad
#define FNUC            param->p[MAXPAR-9]	// VAR: FMAC = FNUC/FNUC_FMAG
#define FMAG            param->p[MAXPAR-8]	// VAR:
#define S1              param->p[MAXPAR-7]	// VAR:
#define S2              param->p[MAXPAR-6]	// VAR:
#define R				param->p[MAXPAR-5]	// VAR:
#define R_AV			param->p[MAXPAR-4]	// VAR:
#define TIME			param->p[MAXPAR-3]	// VAR:
#define STATIC			param->p[MAXPAR-2]	// VAR:
#define PSI             param->p[MAXPAR-1]	// VAR:

scalar i_measured(scalar t, sasfit_param *param);
scalar i_measured_tisane(scalar t, sasfit_param *param);


#endif // end of file

