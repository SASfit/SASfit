/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef PARACRYST_HEX_CYL_PRIVATE_H
#define PARACRYST_HEX_CYL_PRIVATE_H

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

  // use lookup table for sq_p__q___rod(q, param)
  #define sasfit_sq_p__q___rod(q,p) SASFIT_PLUGIN_IMPORTS()->functions[0].func((q),(p))
  #define sasfit_sq_p__q___rod_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[0].func_f((q),(p))
  #define sasfit_sq_p__q___rod_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[0].func_v((q),(p),(d))
  // use lookup table for peak_gaussian_area(q, param)
  #define sasfit_peak_gaussian_area(q,p) SASFIT_PLUGIN_IMPORTS()->functions[0].func((q),(p))
  #define sasfit_peak_gaussian_area_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[0].func_f((q),(p))
  #define sasfit_peak_gaussian_area_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[0].func_v((q),(p),(d))

#else

  #include <sasfit_thin_objects.h>
  #include <sasfit_peak.h>

#endif

// adjust according to the plugins name
#include "sasfit_paracryst_hex_cyl.h"

//
// add local defines here:
// #define P0 param->p[0]
//

#endif // end of file

