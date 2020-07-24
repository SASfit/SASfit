/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef EXTENDED_CLUSTER_STRUCTURE_FACTOR_PRIVATE_H
#define EXTENDED_CLUSTER_STRUCTURE_FACTOR_PRIVATE_H

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

  // use lookup table for ff_dab_epsilon(q, param)
  #define sasfit_ff_dab_epsilon(q,p) SASFIT_PLUGIN_IMPORTS()->functions[0].func((q),(p))
  #define sasfit_ff_dab_epsilon_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[0].func_f((q),(p))
  #define sasfit_ff_dab_epsilon_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[0].func_v((q),(p),(d))

  // use lookup table for sq_hard_sphere__4_3_(q, param)
  #define sasfit_sq_hard_sphere__4_3_(q,p) SASFIT_PLUGIN_IMPORTS()->functions[1].func((q),(p))
  #define sasfit_sq_hard_sphere__4_3__f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[1].func_f((q),(p))
  #define sasfit_sq_hard_sphere__4_3__v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[1].func_v((q),(p),(d))

  // use lookup table for sq_hard_sphere__cs_(q, param)
  #define sasfit_sq_hard_sphere__cs_(q,p) SASFIT_PLUGIN_IMPORTS()->functions[2].func((q),(p))
  #define sasfit_sq_hard_sphere__cs__f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[2].func_f((q),(p))
  #define sasfit_sq_hard_sphere__cs__v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[2].func_v((q),(p),(d))

  // use lookup table for sq_hard_sphere__gh_(q, param)
  #define sasfit_sq_hard_sphere__gh_(q,p) SASFIT_PLUGIN_IMPORTS()->functions[3].func((q),(p))
  #define sasfit_sq_hard_sphere__gh__f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[3].func_f((q),(p))
  #define sasfit_sq_hard_sphere__gh__v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[3].func_v((q),(p),(d))

  // use lookup table for sq_hard_sphere__lhr_(q, param)
  #define sasfit_sq_hard_sphere__lhr_(q,p) SASFIT_PLUGIN_IMPORTS()->functions[4].func((q),(p))
  #define sasfit_sq_hard_sphere__lhr__f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[4].func_f((q),(p))
  #define sasfit_sq_hard_sphere__lhr__v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[4].func_v((q),(p),(d))

  // use lookup table for sq_hard_sphere__mv_(q, param)
  #define sasfit_sq_hard_sphere__mv_(q,p) SASFIT_PLUGIN_IMPORTS()->functions[5].func((q),(p))
  #define sasfit_sq_hard_sphere__mv__f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[5].func_f((q),(p))
  #define sasfit_sq_hard_sphere__mv__v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[5].func_v((q),(p),(d))

  // use lookup table for sq_hard_sphere__py_(q, param)
  #define sasfit_sq_hard_sphere__py_(q,p) SASFIT_PLUGIN_IMPORTS()->functions[6].func((q),(p))
  #define sasfit_sq_hard_sphere__py__f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[6].func_f((q),(p))
  #define sasfit_sq_hard_sphere__py__v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[6].func_v((q),(p),(d))

#else

  #include <sasfit_dab_epsilon.h>
  #include <sasfit_hard_spheres.h>
  #include <sasfit_hard_spheres.h>
  #include <sasfit_hard_spheres.h>
  #include <sasfit_hard_spheres.h>
  #include <sasfit_hard_spheres.h>
  #include <sasfit_hard_spheres.h>

#endif

// adjust according to the plugins name
#include "sasfit_extended_cluster_structure_factor.h"

//
// add local defines here:
// #define P0 param->p[0]
//

#endif // end of file

