/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef KAPPA_GENERALIZED_DISTRIBUTION_PRIVATE_H
#define KAPPA_GENERALIZED_DISTRIBUTION_PRIVATE_H

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
#include "sasfit_kappa_generalized_distribution.h"

//
// add local defines here:
#define A	param->p[0]
#define SIGMA param->p[1]
#define XI  param->p[2]
#define KAPPA	param->p[3]
#define ALPHA	param->p[4]
#define BETA	param->p[5]
#define NU	param->p[6]
//
scalar exp_kappa(scalar, scalar, sasfit_param *);
scalar mode_kappa(sasfit_param *);

#endif // end of file

