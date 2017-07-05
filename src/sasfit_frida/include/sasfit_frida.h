/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */
/**
This package does xxx
**/

#ifndef SASFIT_FRIDA
#define SASFIT_FRIDA

#include <stdio.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_sf.h>
#include <tcl.h>
#include <sasfit_core.h>

void SASFITfridaGlobal(Tcl_Interp *, float **, float **, float **, float **, float **, float **,
		int *, int, sasfit_analytpar *, sasfit_commonpar *, int, struct sasfit_CData *,
		float *, void (*)(), float *, bool *);

#endif // SASFIT_FRIDA
