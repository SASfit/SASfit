/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef CSPLINE_PRIVATE_H
#define CSPLINE_PRIVATE_H

/*
 * Header file for the implementation of the form factor itself.
 */

// optional, depends on form factor implementation
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_spline.h>

gsl_interp_accel *acc_cspline;
gsl_spline * ffakima8_T;
gsl_spline * ffcspline8_T;
gsl_spline * sdakima8_T;
gsl_spline * sdcspline8_T;

// mandatory, no adjustments necessary
#include <sasfit_common.h>

// mandatory, no adjustments necessary
#ifdef MAKE_SASFIT_PLUGIN
  // mandatory, no adjustments necessary
  #include <sasfit_plugin.h>

  SASFIT_PLUGIN_INFO_DECL;

#else


#endif

// adjust according to the plugins name
#include "sasfit_spline.h"

//
// add local defines here:
// #define P0 param->p[0]
//

#endif // end of file

