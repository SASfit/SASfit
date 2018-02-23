/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef PARALLEL_EPIPED_PRIVATE_H
#define PARALLEL_EPIPED_PRIVATE_H

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
#include "sasfit_parallel_epiped.h"

//
// add local defines here:
// #define P0 param->p[0]
//
#define A	param->p[0]
#define B	param->p[1]
#define C	param->p[2]
#define SIGMA param->p[3]
#define ETA	param->p[4]
#define Q	param->p[MAXPAR-1]
#define ALPHA	param->p[MAXPAR-2]
#define BETA	param->p[MAXPAR-3]
#define NUA 	param->p[MAXPAR-4]
#define NUB 	param->p[MAXPAR-5]
#define NUC 	param->p[MAXPAR-6]
#define NUDIM 	param->p[MAXPAR-7]

scalar sinc(scalar);
scalar Kernel_P(const double *, void *);
int K_cubature(unsigned , const double *, void *, unsigned , double *);

#endif // end of file

