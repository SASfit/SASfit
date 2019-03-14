/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(6)
SASFIT_PLUGIN_EXP_ADD( ff_polymer_under_shearflow )
SASFIT_PLUGIN_EXP_ADD( ff_polymer_under_shearflow1 )
SASFIT_PLUGIN_EXP_ADD( ff_polymer_under_shearflow2 )
SASFIT_PLUGIN_EXP_ADD( ff_ring_under_shearflow )
SASFIT_PLUGIN_EXP_ADD( ff_ring_under_shearflow1 )
SASFIT_PLUGIN_EXP_ADD( ff_ring_under_shearflow2 )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

