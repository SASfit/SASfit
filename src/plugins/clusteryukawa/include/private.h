/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef CLUSTERYUKAWA_PRIVATE_H
#define CLUSTERYUKAWA_PRIVATE_H

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

  // use lookup table for sq_paracrystal_1d_random(q, param)
  #define sasfit_sq_paracrystal_1d_random(q,p) SASFIT_PLUGIN_IMPORTS()->functions[11].func((q),(p))
  #define sasfit_sq_paracrystal_1d_random_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[11].func_f((q),(p))
  #define sasfit_sq_paracrystal_1d_random_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[11].func_v((q),(p),(d))

  // use lookup table for sq_paracrystal_orig_1d_random(q, param)
  #define sasfit_sq_paracrystal_orig_1d_random(q,p) SASFIT_PLUGIN_IMPORTS()->functions[12].func((q),(p))
  #define sasfit_sq_paracrystal_orig_1d_random_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[12].func_f((q),(p))
  #define sasfit_sq_paracrystal_orig_1d_random_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[12].func_v((q),(p),(d))

  // use lookup table for sq_pc_random_flight(q, param)
  #define sasfit_sq_pc_random_flight(q,p) SASFIT_PLUGIN_IMPORTS()->functions[13].func((q),(p))
  #define sasfit_sq_pc_random_flight_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[13].func_f((q),(p))
  #define sasfit_sq_pc_random_flight_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[13].func_v((q),(p),(d))

  // use lookup table for sq_simple_sq_power_law(q, param)
  #define sasfit_sq_simple_sq_power_law(q,p) SASFIT_PLUGIN_IMPORTS()->functions[14].func((q),(p))
  #define sasfit_sq_simple_sq_power_law_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[14].func_f((q),(p))
  #define sasfit_sq_simple_sq_power_law_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[14].func_v((q),(p),(d))

  // use lookup table for sq_one_yukawa(q, param)
  #define sasfit_sq_one_yukawa(q,p) SASFIT_PLUGIN_IMPORTS()->functions[16].func((q),(p))
  #define sasfit_sq_one_yukawa_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[16].func_f((q),(p))
  #define sasfit_sq_one_yukawa_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[16].func_v((q),(p),(d))

#else

  #include <sasfit_sq_1d.h>
  #include <sasfit_random_flight.h>
  #include <sasfit_powerlawsq.h>
  #include <sasfit_twoyukawa.h>

#endif

// adjust according to the plugins name
#include "sasfit_clusteryukawa.h"

//
// add local defines here:
// #define P0 param->p[0]
//

#endif // end of file

