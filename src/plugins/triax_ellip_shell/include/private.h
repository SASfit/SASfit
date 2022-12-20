/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef TRIAX_ELLIP_SHELL_PRIVATE_H
#define TRIAX_ELLIP_SHELL_PRIVATE_H

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

  // use lookup table
  #define sasfit_sq_baba_ahmed_syl(q,p) SASFIT_PLUGIN_IMPORTS()->functions[0].func((q),(p))
#else
  #include <sasfit_baba_ahmed.h>
#endif

// adjust according to the plugins name

#include "sasfit_triax_ellip_shell.h"

typedef scalar sasfit_cubature_f(sasfit_param *);
typedef scalar sasfit_cubature_p(scalar , sasfit_param *);

typedef struct
{
	sasfit_param *param;
	sasfit_cubature_f *func; //!< Selects the kernel function to use, when used with gsl functions.
	sasfit_cubature_p *p1; //!< Selects the kernel function to use, when used with gsl functions.
	sasfit_cubature_p *p2; //!< Selects the kernel function to use, when used with gsl functions.
	sasfit_cubature_p *p3; //!< Selects the kernel function to use, when used with gsl functions.
	scalar *cubxmin;
	scalar *cubxmax;
	int ndim;
} cubature_param;

//
// add local defines here:
// #define P0 param->p[0]
//
#define	Q	param->p[MAXPAR-1]
#define	P	param->p[MAXPAR-2] // pow
#define	X	param->p[MAXPAR-3] // param from outer integration
#define	Y	param->p[MAXPAR-4] // param from middle integration
#define NU	param->p[MAXPAR-5] // param from innermost integration
#define LNDISTR	param->p[MAXPAR-6]
#define NUMIN	param->p[MAXPAR-7]
#define NUMAX	param->p[MAXPAR-8]

#define SIGMA	param->p[4]
#define ETA_C	param->p[5]
#define ETA_SH	param->p[6]
#define ETA_SOL	param->p[7]

scalar triax_ellip_shell_cubature(const double *x, size_t ndim, void *pam);
#endif // end of file

