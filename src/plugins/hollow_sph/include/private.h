/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef HOLLOW_SPH_PRIVATE_H
#define HOLLOW_SPH_PRIVATE_H

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

  // use lookup table for ff_sphere(q, param)
  #define sasfit_ff_sphere(q,p) SASFIT_PLUGIN_IMPORTS()->functions[0].func((q),(p))
  #define sasfit_ff_sphere_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[0].func_f((q),(p))
  #define sasfit_ff_sphere_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[0].func_v((q),(p),(d))

#else

  #include <sasfit_sph.h>

#endif

// adjust according to the plugins name
#include "sasfit_hollow_sph.h"

//
// add local defines here:
// #define P0 param->p[0]
//

#endif // end of file

