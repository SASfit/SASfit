/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef DEFORMED_POLYMERS_PRIVATE_H
#define DEFORMED_POLYMERS_PRIVATE_H

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

  // use lookup table for ff_ringpolymerBMB(q, param)
  #define sasfit_ff_ringpolymerBMB(q,p) SASFIT_PLUGIN_IMPORTS()->functions[1].func((q),(p))
  #define sasfit_ff_ringpolymerBMB_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[1].func_f((q),(p))
  #define sasfit_ff_ringpolymerBMB_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[1].func_v((q),(p),(d))

  // use lookup table for ff_ringpolymerBZ(q, param)
  #define sasfit_ff_ringpolymerBZ(q,p) SASFIT_PLUGIN_IMPORTS()->functions[2].func((q),(p))
  #define sasfit_ff_ringpolymerBZ_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[2].func_f((q),(p))
  #define sasfit_ff_ringpolymerBZ_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[2].func_v((q),(p),(d))

  // use lookup table for ff_ringpolymerCasassa(q, param)
  #define sasfit_ff_ringpolymerCasassa(q,p) SASFIT_PLUGIN_IMPORTS()->functions[3].func((q),(p))
  #define sasfit_ff_ringpolymerCasassa_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[3].func_f((q),(p))
  #define sasfit_ff_ringpolymerCasassa_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[3].func_v((q),(p),(d))

#else

  #include <sasfit_polymer_chains.h>
  #include <sasfit_polymer_rings.h>
  #include <sasfit_polymer_rings.h>
  #include <sasfit_polymer_rings.h>

#endif

// adjust according to the plugins name
#include "sasfit_deformed_polymers.h"

//
// add local defines here:
// #define P0 param->p[0]
//

#endif // end of file

