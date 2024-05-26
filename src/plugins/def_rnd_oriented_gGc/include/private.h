/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef DEFORMED_RANDOM_ORIENTED_GENERALIZED_GAUSSIAN_COIL_PRIVATE_H
#define DEFORMED_RANDOM_ORIENTED_GENERALIZED_GAUSSIAN_COIL_PRIVATE_H

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

  // use lookup table for ff_generalized_gaussian_coil(q, param)
  #define sasfit_ff_generalized_gaussian_coil(q,p) SASFIT_PLUGIN_IMPORTS()->functions[0].func((q),(p))
  #define sasfit_ff_generalized_gaussian_coil_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[0].func_f((q),(p))
  #define sasfit_ff_generalized_gaussian_coil_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[0].func_v((q),(p),(d))

#else

  #include <sasfit_polymer_chains.h>

#endif

// adjust according to the plugins name
#include "sasfit_def_rnd_oriented_gGc.h"

//
// add local defines here:
// #define P0 param->p[0]
//

#endif // end of file

