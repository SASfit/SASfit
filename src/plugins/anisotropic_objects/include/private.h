/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef ANISOTROPIC_OBJECTS_PRIVATE_H
#define ANISOTROPIC_OBJECTS_PRIVATE_H

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
  SASFIT_PLUGIN_INFO_DECL;

#else


#endif

// adjust according to the plugins name
#include "sasfit_anisotropic_objects.h"

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


#define R_SQ_ELLSH	param->p[0]
#define EPSILON_SQ_ELLSH	param->p[1]
#define SIGMA_R_SQ_ELLSH	param->p[2]

#define LB		param->p[0]
#define B		param->p[0]
#define L		param->p[1]
#define EXVOL	param->p[2]

#define T_ELLSH	        param->p[0]
#define SIGMA_T_ELLSH	fabs(param->p[1])
#define R0_ELLSH	    param->p[2]
#define SIGMA_R0_ELLSH	fabs(param->p[3])
#define EPSILON_ELLSH	param->p[4]



#define Q	param->p[MAXPAR-1]
#define P	param->p[MAXPAR-2]

#define NU1	        param->p[MAXPAR-3]
#define LNDISTR1	param->p[MAXPAR-4]
#define NU1MIN      param->p[MAXPAR-5]
#define NU1MAX	    param->p[MAXPAR-6]

#define NU2	        param->p[MAXPAR-7]
#define LNDISTR2	param->p[MAXPAR-8]
#define NU2MIN	    param->p[MAXPAR-9]
#define NU2MAX	    param->p[MAXPAR-10]

#define ALPHA   param->p[MAXPAR-11]
#define CLB		param->p[MAXPAR-12]
#define	N	param->p[MAXPAR-13]

scalar fRg2(scalar, scalar);
scalar falpha2(scalar, scalar);
scalar IDebye(scalar, scalar);
scalar S_SB(scalar, scalar, sasfit_param *);
scalar S_EXV(scalar, scalar, sasfit_param *);
scalar fcorr (scalar, scalar, sasfit_param *);
scalar IRod(scalar, scalar);
scalar ThinEllShell_R_core(scalar, sasfit_param *);
scalar ThinEllShell_S_core(scalar, sasfit_param *);
scalar thin_ellipsoidal_shell(scalar, scalar, scalar, scalar, sasfit_param *);

#endif // end of file

