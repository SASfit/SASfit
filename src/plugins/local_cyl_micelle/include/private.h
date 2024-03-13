/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef LOCAL_CYL_MICELLE_PRIVATE_H
#define LOCAL_CYL_MICELLE_PRIVATE_H

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

#else


#endif

// adjust according to the plugins name
#include "sasfit_local_cyl_micelle.h"

//
// add local defines here:
// #define P0 param->p[0]
//
#define PARAM0 param->p[0]
#define PARAM1 param->p[1]
#define V_BRUSH param->p[2]
#define ETA_CORE param->p[3]
#define ETA_BRUSH param->p[4]
#define ETA_SOLV param->p[5]
#define XSOLV_CORE param->p[6]
#define RG param->p[7]
#define D param->p[8]
#define H  param->p[9]
#define NAGG param->p[MAXPAR-5]
#define N_AGG param->p[MAXPAR-4]
#define V_CORE param->p[MAXPAR-3]
#define R param->p[MAXPAR-2]
#define Q param->p[MAXPAR-1]

#endif // end of file

