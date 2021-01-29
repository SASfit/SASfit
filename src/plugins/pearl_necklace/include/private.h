/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef PEARL_NECKLACE_PRIVATE_H
#define PEARL_NECKLACE_PRIVATE_H

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
#include "sasfit_pearl_necklace.h"

//
// add local defines here:
// #define P0 param->p[0]
//

scalar Lambda(scalar, sasfit_param *);
scalar LambdaExt(scalar, scalar, sasfit_param *);
scalar Psi(scalar);
#endif // end of file

