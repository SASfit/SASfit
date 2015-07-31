/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef TRIAX_ELLIP_SHELL_PRIVATE_H
#define TRIAX_ELLIP_SHELL_PRIVATE_H

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

  // use lookup table
  #define sasfit_sq_baba_ahmed_syl(q,p) imp_ptr->functions[0].func((q),(p))
#else
  #include <sasfit_baba_ahmed.h>
#endif

// adjust according to the plugins name
#include "sasfit_triax_ellip_shell.h"

//
// add local defines here:
// #define P0 param->p[0]
//

#endif // end of file

