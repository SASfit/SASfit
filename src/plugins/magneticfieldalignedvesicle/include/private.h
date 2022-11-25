/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef MAGNETICFIELDALIGNEDVESICLE_PRIVATE_H
#define MAGNETICFIELDALIGNEDVESICLE_PRIVATE_H

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
#include "sasfit_magneticfieldalignedvesicle.h"

scalar m_kernel_real(scalar z, sasfit_param *param);
scalar m_kernel_imag(scalar z, sasfit_param *param);
void sasfit_capped_sphere(scalar q, sasfit_param *param, scalar r, 
                          scalar theta1, scalar theta2, scalar *f_real, 
                          scalar *F_imag);
void sasfit_cylinder_for_capped_sphere(scalar q, scalar r, scalar a, 
                                       scalar b, scalar alpha, 
                                       scalar *f_real, scalar *f_imag);
scalar sasfit_ff_magneticfieldalignedvesicle_util_psi(scalar q, sasfit_param * param);

#endif // end of file

