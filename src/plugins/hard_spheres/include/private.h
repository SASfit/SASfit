/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef HARD_SPHERES_PRIVATE_H
#define HARD_SPHERES_PRIVATE_H

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
#include "sasfit_hard_spheres.h"
#include <gsl/gsl_math.h>
#include <gsl/gsl_deriv.h>

//
// add local defines here:
// #define P0 param->p[0]
//

#define R	param->p[0]
#define ETA	param->p[1]
#define CHI param->p[MAXPAR-1]
#define ZHS param->p[MAXPAR-2]
#define CHIPY param->p[MAXPAR-3]
#define ZPY param->p[MAXPAR-4]
#define S1  param->p[MAXPAR-5]
#define S2  param->p[MAXPAR-6]
#define S3  param->p[MAXPAR-7]
#define S4  param->p[MAXPAR-8]

#define L1  param->p[MAXPAR-9]
#define L2  param->p[MAXPAR-10]

scalar ReGfunc(scalar q,sasfit_param *param);
void set_params(sasfit_param * param);
#endif // end of file

