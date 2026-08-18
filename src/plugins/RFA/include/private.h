/*
 * Author(s) of this file:
 *   (see top-level README.md for attribution: rfa_piecewise.c/.h
 *    implements the Rational-Function Approximation of Santos, Yuste
 *    and Lopez de Haro, Condens. Matter Phys. 15, 23602 (2012), and
 *    S. Bravo Yuste and A. Santos, J. Chem. Phys. 101, 2355 (1994))
 */

#ifndef RFA_PRIVATE_H
#define RFA_PRIVATE_H

/*
 * Header file for the implementation of the structure factor itself.
 */

// optional, depends on structure factor implementation
#include <gsl/gsl_math.h>

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
#include "sasfit_RFA.h"

// the shared RFA physics engine (portable C99+GSL, no SASfit-specific
// types) used by every model function in this plugin -- see
// include/rfa_piecewise.h. Supports an arbitrary number of piece-wise
// constant potential steps; this plugin exposes the n=1 and n=3 cases
// as four separate models (RFA1, RFA2, RFA1_3step, RFA2_3step) since
// SASfit's parameter system is a fixed-size array per function, not
// suited to a variable step count -- see sasfit_RFA.h.
#include "rfa_piecewise.h"

// this plugin's own shared caching/warm-starting/physicality-check
// logic, common to all four models -- NOT a SASfit convention, just
// this plugin's own code organization (the four model files are
// otherwise identical except for which solver they warm-start with
// and how many steps they expose).
#include "sasfit_sq_RFA_common.h"

//
// add local defines here:
// #define P0 param->p[0]
//

#endif // end of file
