/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef CYLINDERS_PRIVATE_H
#define CYLINDERS_PRIVATE_H

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

  // use lookup table for ff_pcs_ellcylsh(q, param)
  #define sasfit_ff_pcs_ellcylsh(q,p) SASFIT_PLUGIN_IMPORTS()->functions[0].func((q),(p))
  #define sasfit_ff_pcs_ellcylsh_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[0].func_f((q),(p))
  #define sasfit_ff_pcs_ellcylsh_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[0].func_v((q),(p),(d))

  // use lookup table for sasfit_sq_p__q___rod(q, param)
  #define sasfit_sq_p__q___rod(q,p) SASFIT_PLUGIN_IMPORTS()->functions[1].func((q),(p))
  #define sasfit_sq_p__q___rod_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[1].func_f((q),(p))
  #define sasfit_sq_p__q___rod_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[1].func_v((q),(p),(d))

#else


#endif

// adjust according to the plugins name
#include "sasfit_cylinders.h"

//
// add local defines here:
// #define P0 param->p[0]
//
scalar AcylSHell(sasfit_param *);
scalar A_theta(scalar, void *);
scalar A_alpha(scalar, void *);
scalar A_alpha_sasfit(scalar, sasfit_param *);
int A_cub2(unsigned, const double *, void *, unsigned, double *);
double A_ellcyl(const double *, size_t, void *);
#endif // end of file

