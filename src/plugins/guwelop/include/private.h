/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef GUWELOP_PRIVATE_H
#define GUWELOP_PRIVATE_H

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
#include "sasfit_guwelop.h"

//
// add local defines here:
// #define P0 param->p[0]
//
#define N	param->p[0]

#define BL	GSL_MIN(param->p[1],param->p[2])
#define BU	GSL_MAX(param->p[1],param->p[2])
#define DIM	param->p[3]

#define THETA	param->p[4]
#define ALPHA	fabs(param->p[5])
#define BETA	fabs(param->p[6])
#define GAMMA	fabs(param->p[7])
#define LAMBDA 	fabs(param->p[8])

#define Q       param->p[MAXPAR-1]

#endif // end of file

