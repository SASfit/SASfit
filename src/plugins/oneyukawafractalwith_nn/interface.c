/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(1)
SASFIT_PLUGIN_EXP_ADD( sq_oneyukawa_on_fractal_incl_nn )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_BEGIN( 2 )
SASFIT_PLUGIN_IMP_ADD( sq_mass_fractal_incl__nn_pertubation )
SASFIT_PLUGIN_IMP_ADD( sq_one_yukawa )
SASFIT_PLUGIN_IMP_END

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

