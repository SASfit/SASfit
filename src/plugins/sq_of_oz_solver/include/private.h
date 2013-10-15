/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SQ_OF_OZ_SOLVER_PRIVATE_H
#define SQ_OF_OZ_SOLVER_PRIVATE_H

/*
 * Header file for the implementation of the form factor itself.
 */

// optional, depends on form factor implementation
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_spline.h>

gsl_interp_accel *acc_spline1;
gsl_interp_accel *acc_spline2;
gsl_interp_accel *acc_spline3;
gsl_interp_accel *acc_spline4;
gsl_interp_accel *acc_spline5;
gsl_interp_accel *acc_spline6;
gsl_spline * sq1akima_T;
gsl_spline * sq1cspline_T;
gsl_spline * sq2akima_T;
gsl_spline * sq2cspline_T;
gsl_spline * sq3akima_T;
gsl_spline * sq3cspline_T;
gsl_spline * sq4akima_T;
gsl_spline * sq4cspline_T;
gsl_spline * sq5akima_T;
gsl_spline * sq5cspline_T;
gsl_spline * sq6akima_T;
gsl_spline * sq6cspline_T;

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
#include "sasfit_sq_of_oz_solver.h"

//
// add local defines here:
// #define P0 param->p[0]
//

#endif // end of file

