/*
 * Author(s) of this file:
 *   (see top-level README.md for attribution: rmsa.c/.h implements
 *    Hayter & Penfold, Mol. Phys. 42, 109 (1981) and Hansen & Hayter,
 *    Mol. Phys. 46, 651 (1982))
 */

#ifndef RMSA_PRIVATE_H
#define RMSA_PRIVATE_H

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
#include "sasfit_RMSA.h"

// the shared RMSA physics engine (portable C11+GSL, no SASfit-specific
// types) used by the model function in this plugin -- see
// include/rmsa.h and include/rmsa_physical.h.
#include "rmsa.h"
#include "rmsa_physical.h"

//
// add local defines here:
// #define P0 param->p[0]
//

#endif // end of file
