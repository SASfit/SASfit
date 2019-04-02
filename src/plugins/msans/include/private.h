/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef MSANS_PRIVATE_H
#define MSANS_PRIVATE_H

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

  // use lookup table for ff_gz_dab(q, param)
  #define sasfit_ff_gz_dab(q,p) imp_ptr->functions[0].func((q),(p))
  #define sasfit_ff_gz_dab_f(q,p) imp_ptr->functions[0].func_f((q),(p))
  #define sasfit_ff_gz_dab_v(q,p,d) imp_ptr->functions[0].func_v((q),(p),(d))

  // use lookup table for ff_gz_generalized_dab(q, param)
  #define sasfit_ff_gz_generalized_dab(q,p) imp_ptr->functions[1].func((q),(p))
  #define sasfit_ff_gz_generalized_dab_f(q,p) imp_ptr->functions[1].func_f((q),(p))
  #define sasfit_ff_gz_generalized_dab_v(q,p,d) imp_ptr->functions[1].func_v((q),(p),(d))

  // use lookup table for ff_gz_ggc(q, param)
  #define sasfit_ff_gz_ggc(q,p) imp_ptr->functions[2].func((q),(p))
  #define sasfit_ff_gz_ggc_f(q,p) imp_ptr->functions[2].func_f((q),(p))
  #define sasfit_ff_gz_ggc_v(q,p,d) imp_ptr->functions[2].func_v((q),(p),(d))

  // use lookup table for ff_gz_sphere(q, param)
  #define sasfit_ff_gz_sphere(q,p) imp_ptr->functions[3].func((q),(p))
  #define sasfit_ff_gz_sphere_f(q,p) imp_ptr->functions[3].func_f((q),(p))
  #define sasfit_ff_gz_sphere_v(q,p,d) imp_ptr->functions[3].func_v((q),(p),(d))

#else

  #include <sasfit_sesans.h>
  #include <sasfit_sesans.h>
  #include <sasfit_sesans.h>
  #include <sasfit_sesans.h>

#endif

// adjust according to the plugins name
#include "sasfit_msans.h"

//
// add local defines here:
// #define P0 param->p[0]
//

#endif // end of file

