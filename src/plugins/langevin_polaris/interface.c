/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
/*
SASFIT_PLUGIN_EXP_BEGIN(6)
SASFIT_PLUGIN_EXP_ADD( ff_tisane )
SASFIT_PLUGIN_EXP_ADD( ff_tisane_static )
SASFIT_PLUGIN_EXP_ADD( ff_tisane_tau )
SASFIT_PLUGIN_EXP_ADD( ff_langevin )
SASFIT_PLUGIN_EXP_ADD( ff_langevin_static )
SASFIT_PLUGIN_EXP_ADD( ff_langevin_tau )
*/

SASFIT_PLUGIN_EXP_BEGIN(18)
SASFIT_PLUGIN_EXP_ADD( ff_tisane )
SASFIT_PLUGIN_EXP_ADD( ff_tisane_static )
SASFIT_PLUGIN_EXP_ADD( ff_tisane_tau )
SASFIT_PLUGIN_EXP_ADD( ff_langevin )
SASFIT_PLUGIN_EXP_ADD( ff_langevin_static )
SASFIT_PLUGIN_EXP_ADD( ff_langevin_tau )
SASFIT_PLUGIN_EXP_ADD( ff_langevin_up_up )
SASFIT_PLUGIN_EXP_ADD( ff_langevin_static_up_up )
SASFIT_PLUGIN_EXP_ADD( ff_langevin_down_down )
SASFIT_PLUGIN_EXP_ADD( ff_langevin_static_down_down )
SASFIT_PLUGIN_EXP_ADD( ff_langevin_up_down )
SASFIT_PLUGIN_EXP_ADD( ff_langevin_static_up_down )
SASFIT_PLUGIN_EXP_ADD( ff_langevin_down_up )
SASFIT_PLUGIN_EXP_ADD( ff_langevin_static_down_up )
SASFIT_PLUGIN_EXP_ADD( ff_langevin_up )
SASFIT_PLUGIN_EXP_ADD( ff_langevin_static_up )
SASFIT_PLUGIN_EXP_ADD( ff_langevin_down )
SASFIT_PLUGIN_EXP_ADD( ff_langevin_static_down )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

