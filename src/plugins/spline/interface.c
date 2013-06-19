/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(4)
SASFIT_PLUGIN_EXP_ADD( sd_akima8 )
SASFIT_PLUGIN_EXP_ADD( sd_cspline8 )
SASFIT_PLUGIN_EXP_ADD( ff_akima8 )
SASFIT_PLUGIN_EXP_ADD( ff_cspline8 )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
	acc_cspline = gsl_interp_accel_alloc ();
	sdakima8_T = gsl_spline_alloc (gsl_interp_akima, 10);
	sdcspline8_T = gsl_spline_alloc (gsl_interp_cspline , 10);
	ffakima8_T = gsl_spline_alloc (gsl_interp_akima, 10);
	ffcspline8_T = gsl_spline_alloc (gsl_interp_cspline , 10);
}

#endif

