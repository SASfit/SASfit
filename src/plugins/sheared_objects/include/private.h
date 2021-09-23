/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SHEARED_OBJECTS_PRIVATE_H
#define SHEARED_OBJECTS_PRIVATE_H

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
#include "sasfit_sheared_objects.h"

//
// add local defines here:
// #define P0 param->p[0]
//


#define NR	param->p[MAXPAR-9]
#define NT	param->p[MAXPAR-10]
#define NL	param->p[MAXPAR-11]

#define R	(param->p[0]*NR)
#define B   (param->p[0]*NR)
#define T	(param->p[1]*NT)
#define L	(param->p[2]*NL)
#define A	(param->p[2]*NL)
#define ETA_CORE	param->p[3]
#define ETA_SHELL	param->p[4]
#define ETA_SOLV	param->p[5]
#define PSI_DEG	param->p[6]
#define SIGMA	param->p[7]
#define KAPPA	param->p[8]


#define	Q	param->p[MAXPAR-1]
#define	P	param->p[MAXPAR-2] // pow
#define	THETA	param->p[MAXPAR-3] // param from outer integration
#define	PHI	param->p[MAXPAR-4] // param from middle integration
#define NU	param->p[MAXPAR-5] // param from innermost integration
#define LNDISTR	param->p[MAXPAR-6]
#define NUMIN	param->p[MAXPAR-7]
#define NUMAX	param->p[MAXPAR-8]


typedef scalar sasfit_cubature_g(scalar, scalar, scalar);
typedef scalar sasfit_cubature_f(sasfit_cubature_g *,sasfit_param *);
typedef scalar sasfit_cubature_p(scalar, scalar, sasfit_param *);

typedef struct
{
	sasfit_param *param;
	sasfit_cubature_f *func; //!< Selects the kernel function to use, when used with gsl functions.
	sasfit_cubature_g *gam;
	sasfit_cubature_p *p1; //!< Selects the kernel function to use, when used with gsl functions.
	sasfit_cubature_p *p2; //!< Selects the kernel function to use, when used with gsl functions.
	sasfit_cubature_p *p3; //!< Selects the kernel function to use, when used with gsl functions.
	scalar *cubxmin;
	scalar *cubxmax;
	int ndim;
} cubature_param;



scalar pMaierSaupe(scalar x, scalar y, sasfit_param * param);
scalar pGauss(scalar x, scalar y, sasfit_param * param);
scalar pBoltzmann(scalar x, scalar y, sasfit_param * param);
scalar pOnsager(scalar x, scalar y, sasfit_param * param);
scalar pHayterPenfold(scalar x, scalar y, sasfit_param * param);
scalar pHeavysidePi(scalar x, scalar y, sasfit_param * param);
scalar pLegendre(scalar x, scalar y, sasfit_param * param);

int partly_aligned_cubature(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval);

scalar partly_aligned_sasfit_cubature(const double *x, size_t ndim, void *pam);

int partly_aligned_cubature_u_phi(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval);
int random_oriented_cubature(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval);
scalar alignedCylShell(sasfit_cubature_g *gam, sasfit_param * param);
scalar alignedEllSh(sasfit_cubature_g *gam, sasfit_param * param);
scalar gamHPplus(scalar psi, scalar theta, scalar phi);
scalar gamHPminus(scalar psi, scalar theta, scalar phi);
scalar gamOthers(scalar psi, scalar theta, scalar phi);
scalar gamTheta(scalar psi, scalar theta, scalar phi);
#endif // end of file

