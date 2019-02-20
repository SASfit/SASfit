/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef AZIMUTHAL_PRIVATE_H
#define AZIMUTHAL_PRIVATE_H

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

  // use lookup table for ff_sheared_cylinders__boltzmann_(q, param)
  #define sasfit_ff_sheared_cylinders__boltzmann_(q,p) imp_ptr->functions[0].func((q),(p))
  #define sasfit_ff_sheared_cylinders__boltzmann__f(q,p) imp_ptr->functions[0].func_f((q),(p))
  #define sasfit_ff_sheared_cylinders__boltzmann__v(q,p,d) imp_ptr->functions[0].func_v((q),(p),(d))

  // use lookup table for ff_sheared_cylinders__gauss_(q, param)
  #define sasfit_ff_sheared_cylinders__gauss_(q,p) imp_ptr->functions[1].func((q),(p))
  #define sasfit_ff_sheared_cylinders__gauss__f(q,p) imp_ptr->functions[1].func_f((q),(p))
  #define sasfit_ff_sheared_cylinders__gauss__v(q,p,d) imp_ptr->functions[1].func_v((q),(p),(d))

  // use lookup table for ff_sheared_cylinders__hayterpenfold_(q, param)
  #define sasfit_ff_sheared_cylinders__hayterpenfold_(q,p) imp_ptr->functions[2].func((q),(p))
  #define sasfit_ff_sheared_cylinders__hayterpenfold__f(q,p) imp_ptr->functions[2].func_f((q),(p))
  #define sasfit_ff_sheared_cylinders__hayterpenfold__v(q,p,d) imp_ptr->functions[2].func_v((q),(p),(d))

  // use lookup table for ff_sheared_cylinders__heavyside_(q, param)
  #define sasfit_ff_sheared_cylinders__heavyside_(q,p) imp_ptr->functions[3].func((q),(p))
  #define sasfit_ff_sheared_cylinders__heavyside__f(q,p) imp_ptr->functions[3].func_f((q),(p))
  #define sasfit_ff_sheared_cylinders__heavyside__v(q,p,d) imp_ptr->functions[3].func_v((q),(p),(d))

  // use lookup table for ff_sheared_cylinders__maier_saupe_(q, param)
  #define sasfit_ff_sheared_cylinders__maier_saupe_(q,p) imp_ptr->functions[4].func((q),(p))
  #define sasfit_ff_sheared_cylinders__maier_saupe__f(q,p) imp_ptr->functions[4].func_f((q),(p))
  #define sasfit_ff_sheared_cylinders__maier_saupe__v(q,p,d) imp_ptr->functions[4].func_v((q),(p),(d))

  // use lookup table for ff_sheared_cylinders__onsager_(q, param)
  #define sasfit_ff_sheared_cylinders__onsager_(q,p) imp_ptr->functions[5].func((q),(p))
  #define sasfit_ff_sheared_cylinders__onsager__f(q,p) imp_ptr->functions[5].func_f((q),(p))
  #define sasfit_ff_sheared_cylinders__onsager__v(q,p,d) imp_ptr->functions[5].func_v((q),(p),(d))

  // use lookup table for ff_sheared_spheroids__boltzmann_(q, param)
  #define sasfit_ff_sheared_spheroids__boltzmann_(q,p) imp_ptr->functions[6].func((q),(p))
  #define sasfit_ff_sheared_spheroids__boltzmann__f(q,p) imp_ptr->functions[6].func_f((q),(p))
  #define sasfit_ff_sheared_spheroids__boltzmann__v(q,p,d) imp_ptr->functions[6].func_v((q),(p),(d))

  // use lookup table for ff_sheared_spheroids__gauss_(q, param)
  #define sasfit_ff_sheared_spheroids__gauss_(q,p) imp_ptr->functions[7].func((q),(p))
  #define sasfit_ff_sheared_spheroids__gauss__f(q,p) imp_ptr->functions[7].func_f((q),(p))
  #define sasfit_ff_sheared_spheroids__gauss__v(q,p,d) imp_ptr->functions[7].func_v((q),(p),(d))

  // use lookup table for ff_sheared_spheroids__hayterpenfold_(q, param)
  #define sasfit_ff_sheared_spheroids__hayterpenfold_(q,p) imp_ptr->functions[8].func((q),(p))
  #define sasfit_ff_sheared_spheroids__hayterpenfold__f(q,p) imp_ptr->functions[8].func_f((q),(p))
  #define sasfit_ff_sheared_spheroids__hayterpenfold__v(q,p,d) imp_ptr->functions[8].func_v((q),(p),(d))

  // use lookup table for ff_sheared_spheroids__heavyside_(q, param)
  #define sasfit_ff_sheared_spheroids__heavyside_(q,p) imp_ptr->functions[9].func((q),(p))
  #define sasfit_ff_sheared_spheroids__heavyside__f(q,p) imp_ptr->functions[9].func_f((q),(p))
  #define sasfit_ff_sheared_spheroids__heavyside__v(q,p,d) imp_ptr->functions[9].func_v((q),(p),(d))

  // use lookup table for ff_sheared_spheroids__maier_saupe_(q, param)
  #define sasfit_ff_sheared_spheroids__maier_saupe_(q,p) imp_ptr->functions[10].func((q),(p))
  #define sasfit_ff_sheared_spheroids__maier_saupe__f(q,p) imp_ptr->functions[10].func_f((q),(p))
  #define sasfit_ff_sheared_spheroids__maier_saupe__v(q,p,d) imp_ptr->functions[10].func_v((q),(p),(d))

  // use lookup table for ff_sheared_spheroids__onsager_(q, param)
  #define sasfit_ff_sheared_spheroids__onsager_(q,p) imp_ptr->functions[11].func((q),(p))
  #define sasfit_ff_sheared_spheroids__onsager__f(q,p) imp_ptr->functions[11].func_f((q),(p))
  #define sasfit_ff_sheared_spheroids__onsager__v(q,p,d) imp_ptr->functions[11].func_v((q),(p),(d))

#else

  #include <sasfit_sheared_objects.h>

#endif

// adjust according to the plugins name
#include "sasfit_azimuthal.h"

//
// add local defines here:
// #define P0 param->p[0]
//
scalar MaierSaupeODF(scalar psi, scalar kappa);
scalar psi_quarter_rad(scalar psi);
scalar psi_quarter_deg(scalar psi);
#endif // end of file

