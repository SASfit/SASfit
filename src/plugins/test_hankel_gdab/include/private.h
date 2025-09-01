/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef TEST_HANKEL_GDAB_PRIVATE_H
#define TEST_HANKEL_GDAB_PRIVATE_H

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

  // use lookup table for ff_g_dab(q, param)
  #define sasfit_ff_g_dab(q,p) SASFIT_PLUGIN_IMPORTS()->functions[0].func((q),(p))
  #define sasfit_ff_g_dab_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[0].func_f((q),(p))
  #define sasfit_ff_g_dab_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[0].func_v((q),(p),(d))

#else

  #include <sasfit_non_particulate.h>

#endif

// adjust according to the plugins name
#include "sasfit_test_hankel_gdab.h"

//
// add local defines here:
// #define P0 param->p[0]
//

#endif // end of file

