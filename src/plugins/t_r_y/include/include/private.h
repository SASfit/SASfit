/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef T_LINDLEY_Y_PRIVATE_H
#define T_LINDLEY_Y_PRIVATE_H

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
#include "sasfit_t_lindley_y.h"

//
// add local defines here:
// #define P0 param->p[0]
//

#define N	param->p[0]
#define BL	param->p[1]
#define BU	param->p[2]
#define A	param->p[3]
#define THETA	param->p[4]
#define ALPHA	param->p[5]
#define BETA	param->p[6]
#define MU		param->p[7]
#define SIGMA	param->p[8]

scalar KWm1(scalar, scalar);
scalar Lindley_f_R(scalar, scalar);
scalar Lindley_F_R(scalar, scalar);
#endif // end of file

