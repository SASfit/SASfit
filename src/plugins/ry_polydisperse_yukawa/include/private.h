/*
 * Author(s) of this file:
 *   (see README.md)
 */

#ifndef RY_POLYDISPERSE_YUKAWA_PRIVATE_H
#define RY_POLYDISPERSE_YUKAWA_PRIVATE_H

/*
 * Header file for the implementation of the structure factor itself.
 */

// optional, depends on structure factor implementation
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>

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
#include "sasfit_ry_polydisperse_yukawa.h"

// the numerical multicomponent Ornstein-Zernike solver used by this plugin
// (plain C99 + GSL + SUNDIALS KINSOL, no SASfit types, unit-testable
// standalone -- same separation robertus_shs_core uses)
#include "ry_polydisperse_core.h"

//
// add local defines here:
// #define P0 param->p[0]
//

#endif // end of file
