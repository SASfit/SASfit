/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SPHERICAL_ELLIPSOIDAL_MICELLES_PRIVATE_H
#define SPHERICAL_ELLIPSOIDAL_MICELLES_PRIVATE_H

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

  // use lookup table for sq_freely_joined_chain_of_rods(q, param)
  #define sasfit_sq_freely_joined_chain_of_rods(q,p) SASFIT_PLUGIN_IMPORTS()->functions[0].func((q),(p))
  #define sasfit_sq_freely_joined_chain_of_rods_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[0].func_f((q),(p))
  #define sasfit_sq_freely_joined_chain_of_rods_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[0].func_v((q),(p),(d))

  // use lookup table for sq_koyama_worms(q, param)
  #define sasfit_sq_koyama_worms(q,p) SASFIT_PLUGIN_IMPORTS()->functions[1].func((q),(p))
  #define sasfit_sq_koyama_worms_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[1].func_f((q),(p))
  #define sasfit_sq_koyama_worms_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[1].func_v((q),(p),(d))

  // use lookup table for sq_p__q___kholodenko_worm(q, param)
  #define sasfit_sq_p__q___kholodenko_worm(q,p) SASFIT_PLUGIN_IMPORTS()->functions[2].func((q),(p))
  #define sasfit_sq_p__q___kholodenko_worm_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[2].func_f((q),(p))
  #define sasfit_sq_p__q___kholodenko_worm_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[2].func_v((q),(p),(d))

  // use lookup table for sq_p__q___worm_ps1_(q, param)
  #define sasfit_sq_p__q___worm_ps1_(q,p) SASFIT_PLUGIN_IMPORTS()->functions[3].func((q),(p))
  #define sasfit_sq_p__q___worm_ps1__f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[3].func_f((q),(p))
  #define sasfit_sq_p__q___worm_ps1__v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[3].func_v((q),(p),(d))

  // use lookup table for sq_p__q___worm_ps2_(q, param)
  #define sasfit_sq_p__q___worm_ps2_(q,p) SASFIT_PLUGIN_IMPORTS()->functions[4].func((q),(p))
  #define sasfit_sq_p__q___worm_ps2__f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[4].func_f((q),(p))
  #define sasfit_sq_p__q___worm_ps2__v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[4].func_v((q),(p),(d))

  // use lookup table for sq_p__q___worm_ps3_(q, param)
  #define sasfit_sq_p__q___worm_ps3_(q,p) SASFIT_PLUGIN_IMPORTS()->functions[5].func((q),(p))
  #define sasfit_sq_p__q___worm_ps3__f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[5].func_f((q),(p))
  #define sasfit_sq_p__q___worm_ps3__v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[5].func_v((q),(p),(d))

  // use lookup table for sasfit_ff_generalized_gaussian_coil(q, param)
  #define sasfit_ff_generalized_gaussian_coil(q,p) SASFIT_PLUGIN_IMPORTS()->functions[6].func((q),(p))
  #define sasfit_ff_generalized_gaussian_coil_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[6].func_f((q),(p))
  #define sasfit_ff_generalized_gaussian_coil_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[6].func_v((q),(p),(d))


#else

  #include <sasfit_thin_objects.h>

#endif

// adjust according to the plugins name
#include "sasfit_spherical_ellipsoidal_micelles.h"

//
// add local defines here:
// #define P0 param->p[0]
//
scalar sasfit_ff_Sphere_RWbrush(scalar, sasfit_param *);
scalar sasfit_ff_Sphere_R_ma_Profile(scalar, sasfit_param *);
scalar sasfit_ff_ellip_rwbrush(scalar, sasfit_param *);
scalar sasfit_ff_ellip_rwbrush_rc(scalar, sasfit_param *);
scalar sasfit_ff_ellip_rwbrush_nagg(scalar, sasfit_param *);
scalar sasfit_sphere_SAWbrush(scalar, sasfit_param *);
#endif // end of file

