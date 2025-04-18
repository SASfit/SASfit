/*
 * Author(s) of this file:
 *  Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *  10. Oct. 2017
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(5)
SASFIT_PLUGIN_EXP_ADD( ff_fan_helix )
SASFIT_PLUGIN_EXP_ADD( ff_round_helix )
SASFIT_PLUGIN_EXP_ADD( ff_beads_helix )
SASFIT_PLUGIN_EXP_ADD( ff_superhelices_straight )
SASFIT_PLUGIN_EXP_ADD( ff_superhelices_coiled )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

