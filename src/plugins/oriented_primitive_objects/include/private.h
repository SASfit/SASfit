/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef ORIENTED_PRIMITIVE_OBJECTS_PRIVATE_H
#define ORIENTED_PRIMITIVE_OBJECTS_PRIVATE_H

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
#include "sasfit_oriented_primitive_objects.h"

//
// add local defines here:
// #define P0 param->p[0]
//

#define A	    param->p[0]
#define EA_X	param->p[1]
#define EA_Y	param->p[2]
#define EA_Z	param->p[3]
#define B	    param->p[4]
#define EB_X	param->p[5]
#define EB_Y	param->p[6]
#define EB_Z	param->p[7]
#define C	    param->p[8]
#define EC_X	param->p[9]
#define EC_Y	param->p[10]
#define EC_Z	param->p[11]
#define ETA_P	param->p[12]
#define ETA_M	param->p[13]
#define ALPHA   param->p[14]*M_PI/180.
#define BETA    param->p[15]*M_PI/180.
#define GAMMA   param->p[16]*M_PI/180.
#define PSI_DEG param->p[17]
#define TILT  param->p[18]*M_PI/180.
#define H_R  param->p[19]
#define PP   param->p[18]
#define QQ   param->p[19]
#define EPS1 param->p[18]
#define EPS2 param->p[19]
#define FMM  param->p[18]
#define FALPHA  param->p[19]
#define FN1  param->p[20]
#define FN2  param->p[21]
#define FN3  param->p[22]
#define FAA  param->p[23]
#define FBB  param->p[24]
#define SMM  param->p[25]
#define SALPHA  param->p[26]
#define SN1  param->p[27]
#define SN2  param->p[28]
#define SN3  param->p[29]
#define SAA  param->p[30]
#define SBB  param->p[31]
#define RADIUS  param->p[32]
#define PITCH  param->p[33]
#define SGAMMA  param->p[34]
#define TURNS  param->p[35]
#define XX param->p[MAXPAR-1]
#define YY param->p[MAXPAR-2]
#define ZZ param->p[MAXPAR-3]
#define QQX param->p[MAXPAR-4]
#define QQY param->p[MAXPAR-5]
#define QQZ param->p[MAXPAR-6]
#define RR param->p[MAXPAR-7]
#define THETA param->p[MAXPAR-7]
#define UU param->p[MAXPAR-8]
#define PHI param->p[MAXPAR-8]

#define EPS_OH 1e-4

#endif // end of file

