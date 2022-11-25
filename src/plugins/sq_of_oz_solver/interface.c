/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(6)
SASFIT_PLUGIN_EXP_ADD( sq_sq_oz_1 )
SASFIT_PLUGIN_EXP_ADD( sq_sq_oz_2 )
SASFIT_PLUGIN_EXP_ADD( sq_sq_oz_3 )
SASFIT_PLUGIN_EXP_ADD( sq_sq_oz_4 )
SASFIT_PLUGIN_EXP_ADD( sq_sq_oz_5 )
SASFIT_PLUGIN_EXP_ADD( sq_sq_oz_6 )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
	acc_spline1 = gsl_interp_accel_alloc ();
	acc_spline2 = gsl_interp_accel_alloc ();
	acc_spline3 = gsl_interp_accel_alloc ();
	acc_spline4 = gsl_interp_accel_alloc ();
	acc_spline5 = gsl_interp_accel_alloc ();
	acc_spline6 = gsl_interp_accel_alloc ();

    sq1akima_T=NULL;
    sq1cspline_T=NULL;
    sq2akima_T=NULL;
    sq2cspline_T=NULL;
    sq3akima_T=NULL;
    sq3cspline_T=NULL;
    sq4akima_T=NULL;
    sq4cspline_T=NULL;
    sq5akima_T=NULL;
    sq5cspline_T=NULL;
    sq6akima_T=NULL;
    sq6cspline_T=NULL;
}

#endif

