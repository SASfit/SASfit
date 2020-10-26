/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef TWOYUKAWA_PRIVATE_H
#define TWOYUKAWA_PRIVATE_H

/*
 * Header file for the implementation of the form factor itself.
 */

// optional, depends on form factor implementation
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_const_num.h>
#include "2Y_TwoYukawa.h"
#include "2Y_PairCorrelation.h"
#include "2Y_OneYukawa.h"
#include <gsl/gsl_spline.h>

#define NR 65536
//#define NR 16384

static gsl_interp_accel * acc_spline=NULL;
static gsl_spline * grakima_T;
static gsl_spline * grcspline_T;
static gsl_spline * grsteffen_T=NULL;
static gsl_spline * grlinear_T;

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
#include "sasfit_twoyukawa.h"

//
// add local defines here:
// #define P0 param->p[0]
//

#endif // end of file

