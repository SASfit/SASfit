/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef VESICLE_PEG_PRIVATE_H
#define VESICLE_PEG_PRIVATE_H

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

  // use lookup table for ff_pcs_bilayergauss(q, param)
  #define sasfit_ff_pcs_bilayergauss(q,p) SASFIT_PLUGIN_IMPORTS()->functions[0].func((q),(p))
  #define sasfit_ff_pcs_bilayergauss_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[0].func_f((q),(p))
  #define sasfit_ff_pcs_bilayergauss_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[0].func_v((q),(p),(d))

  // use lookup table for ff_pcs_homogeneouscyl(q, param)
  #define sasfit_ff_pcs_homogeneouscyl(q,p) SASFIT_PLUGIN_IMPORTS()->functions[1].func((q),(p))
  #define sasfit_ff_pcs_homogeneouscyl_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[1].func_f((q),(p))
  #define sasfit_ff_pcs_homogeneouscyl_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[1].func_v((q),(p),(d))

  // use lookup table for ff_pcs_homogeneousplate(q, param)
  #define sasfit_ff_pcs_homogeneousplate(q,p) SASFIT_PLUGIN_IMPORTS()->functions[2].func((q),(p))
  #define sasfit_ff_pcs_homogeneousplate_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[2].func_f((q),(p))
  #define sasfit_ff_pcs_homogeneousplate_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[2].func_v((q),(p),(d))

  // use lookup table for ff_pcs_plate_chains_rw_(q, param)
  #define sasfit_ff_pcs_plate_chains_rw_(q,p) SASFIT_PLUGIN_IMPORTS()->functions[3].func((q),(p))
  #define sasfit_ff_pcs_plate_chains_rw__f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[3].func_f((q),(p))
  #define sasfit_ff_pcs_plate_chains_rw__v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[3].func_v((q),(p),(d))

  // use lookup table for sq_p__q___thin_disc(q, param)
  #define sasfit_sq_p__q___thin_disc(q,p) SASFIT_PLUGIN_IMPORTS()->functions[4].func((q),(p))
  #define sasfit_sq_p__q___thin_disc_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[4].func_f((q),(p))
  #define sasfit_sq_p__q___thin_disc_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[4].func_v((q),(p),(d))

  // use lookup table for sq_p__q___thin_ellipsoidal_shell(q, param)
  #define sasfit_sq_p__q___thin_ellipsoidal_shell(q,p) SASFIT_PLUGIN_IMPORTS()->functions[5].func((q),(p))
  #define sasfit_sq_p__q___thin_ellipsoidal_shell_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[5].func_f((q),(p))
  #define sasfit_sq_p__q___thin_ellipsoidal_shell_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[5].func_v((q),(p),(d))

  // use lookup table for sq_p__q___thin_spherical_shell(q, param)
  #define sasfit_sq_p__q___thin_spherical_shell(q,p) SASFIT_PLUGIN_IMPORTS()->functions[6].func((q),(p))
  #define sasfit_sq_p__q___thin_spherical_shell_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[6].func_f((q),(p))
  #define sasfit_sq_p__q___thin_spherical_shell_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[6].func_v((q),(p),(d))

#else

  #include <sasfit_thin_objects.h>

#endif

// adjust according to the plugins name
#include "sasfit_vesicle_peg.h"

//
// add local defines here:
// #define P0 param->p[0]
//

#endif // end of file

