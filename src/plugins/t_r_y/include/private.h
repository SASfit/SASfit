/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef T_R_Y_PRIVATE_H
#define T_R_Y_PRIVATE_H

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
#include "sasfit_t_r_y.h"

//
// add local defines here:
// #define P0 param->p[0]
//

#define N	param->p[0]

#define BL	GSL_MIN(param->p[1],param->p[2])
#define BU	GSL_MAX(param->p[1],param->p[2])

#define DIM	param->p[3]

#define KAPPA	fabs(param->p[4])
#define THETA	param->p[4]
#define S	    param->p[4]
#define W	    param->p[4]
#define LAMBDA2	param->p[4]

#define ALPHA	param->p[5]
#define C	    param->p[5]
#define LAMBDA3	param->p[5]

#define BETA	param->p[6]
#define LAMBDA4	param->p[6]

#define GAMMA	param->p[7]
#define MU		param->p[7]

#define DELTA	param->p[8]
#define SIGMA	param->p[8]
#define LAMBDA 	param->p[8]



// #define LAMBDA3	fabs(param->p[5])
// #define LAMBDA4	fabs(param->p[6])
// #define THETA	param->p[4]

#define Q   param->p[MAXPAR-1]

scalar KWm1(scalar, scalar);
scalar Lindley_f_R(scalar, scalar);
scalar Lindley_F_R(scalar, scalar);

#endif // end of file

