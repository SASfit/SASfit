/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef METALOG_PRIVATE_H
#define METALOG_PRIVATE_H

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
#include "sasfit_metalog.h"

//
// add local defines here:
#define N	param->p[0]
#define BL	GSL_MIN(param->p[1],param->p[2])
#define BU	GSL_MAX(param->p[1],param->p[2])
#define ALPHA	param->p[3]
#define A1	param->p[4]
#define A2	param->p[5]
#define A3	param->p[6]
#define A4	param->p[7]
#define A5	param->p[8]
#define A6	param->p[9]
#define A7	param->p[10]
#define A8	param->p[11]
#define A9	param->p[12]
#define A10	param->p[13]
#define Q   param->p[MAXPAR-1]

scalar Mk(int , scalar , scalar *, size_t , sasfit_param *);
scalar MLog(int , scalar , scalar *, size_t , scalar, sasfit_param *);
scalar MNLog(int , scalar , scalar *, size_t , scalar, sasfit_param *);
scalar MLogit(int , scalar , scalar *, size_t , scalar, scalar, sasfit_param *);

scalar mk(int , scalar , scalar *, size_t , sasfit_param *);
scalar mLog(int , scalar , scalar *, size_t , sasfit_param *);
scalar mNLog(int , scalar , scalar *, size_t , sasfit_param *);
scalar mLogit(int , scalar , scalar *, size_t ,  scalar, scalar, sasfit_param *);


#endif // end of file

