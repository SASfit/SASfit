/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(1)
SASFIT_PLUGIN_EXP_ADD( ff_def_rnd_oriented_gGc )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_BEGIN( 1 )
SASFIT_PLUGIN_IMP_ADD( ff_generalized_gaussian_coil )
SASFIT_PLUGIN_IMP_END

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

