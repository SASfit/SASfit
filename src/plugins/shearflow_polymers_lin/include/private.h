/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SHEARFLOW_POLYMERS_LIN_PRIVATE_H
#define SHEARFLOW_POLYMERS_LIN_PRIVATE_H

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
#include "sasfit_shearflow_polymers_lin.h"

//
// add local defines here:
// #define P0 param->p[0]
//

#define RG	param->p[0]
#define FP	    param->p[1]
#define ALPHA	param->p[2]
#define BETA	param->p[3]
#define GAMMA	param->p[4]
#define A	param->p[5]
#define B	param->p[6]
#define C	param->p[7]
#define PHI_DEG     param->p[8]
#define PSI_0_DEG   param->p[8]
#define THETA_DEG   param->p[9]
#define P2  param->p[9]
#define PSI_DEG     param->p[10]
#define P4  param->p[10]
#define VARPHI_DEG	param->p[11]
#define PHI0_DEG	param->p[12]
#define DELTA_PHI_DEG	param->p[13]
#define THETA0_DEG	param->p[14]
#define I0	        param->p[15]
#define QF          param->p[MAXPAR-1]
#define QV          param->p[MAXPAR-2]
#define QG          param->p[MAXPAR-3]
#define QMOD        param->p[MAXPAR-4]
#define VARPHI      param->p[MAXPAR-5]
#define PHI0	    param->p[MAXPAR-6]
#define DELTA_PHI	param->p[MAXPAR-7]
#define THETA0	    param->p[MAXPAR-8]
#define PHI	        param->p[MAXPAR-9]
#define THETA	    param->p[MAXPAR-10]
#define PSI	        param->p[MAXPAR-11]


void calc_QFVG(sasfit_param *);
scalar sasfit_ff_shearflow_lin_core(sasfit_param *);

#endif // end of file

