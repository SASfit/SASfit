/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef CYLINDERS_PRIVATE_H
#define CYLINDERS_PRIVATE_H

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
#include "sasfit_cylinders.h"

//
// add local defines here:
// #define P0 param->p[0]
//
scalar AcylSHell(sasfit_param *);
scalar A_theta(scalar, void *);
scalar A_alpha(scalar, void *);
scalar A_alpha_sasfit(scalar, sasfit_param *);
int A_cub2(unsigned, const double *, void *, unsigned, double *);
#endif // end of file

