/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef ELLIPSOIDAL_SHELL_PRIVATE_H
#define ELLIPSOIDAL_SHELL_PRIVATE_H

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
#include "sasfit_ellipsoidal_shell.h"

//
// add local defines here:
// #define P0 param->p[0]
//
typedef scalar sasfit_cubature_f(sasfit_param *);

typedef struct
{
	sasfit_param *param;
	sasfit_cubature_f *func; //!< Selects the kernel function to use, when used with gsl functions.
	scalar *cubxmin;
	scalar *cubxmax;
	int ndim;
} cubature_param;

#define SIGMA	param->p[4]
#define ETA_CORE	param->p[5]
#define ETA_SHELL	param->p[6]
#define ETA_SOLV	param->p[7]
#define Q	param->p[MAXPAR-1]
#define P	param->p[MAXPAR-2]
#define NU	param->p[MAXPAR-3]
#define MU	param->p[MAXPAR-4]
#define A	param->p[MAXPAR-5]
#define B	param->p[MAXPAR-6]
#define TNU	param->p[MAXPAR-7]
#define LNDISTR	param->p[MAXPAR-8]
#define NUMIN	param->p[MAXPAR-9] 
#define NUMAX	param->p[MAXPAR-10] 

scalar F_EllSh(sasfit_param *param);
int ellip_shell_cubature(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval);
scalar Kernel_P_OOURA1(scalar x, void * pam);

#endif // end of file

