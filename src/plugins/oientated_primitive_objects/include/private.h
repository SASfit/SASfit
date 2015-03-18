/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef OIENTATED_PRIMITIVE_OBJECTS_PRIVATE_H
#define OIENTATED_PRIMITIVE_OBJECTS_PRIVATE_H

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
#include "sasfit_oientated_primitive_objects.h"

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
#define ALPHA   param->p[14]
#define BETA    param->p[15]
#define GAMMA   param->p[16]
#define PSI_DEG param->p[17]

#endif // end of file

