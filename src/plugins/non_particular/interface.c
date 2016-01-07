/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(9)
SASFIT_PLUGIN_EXP_ADD( ff_generalized_guinier_porod_law )
SASFIT_PLUGIN_EXP_ADD( ff_extended_guinier_law )
SASFIT_PLUGIN_EXP_ADD( ff_beaucage_exppowlaw_2 )
SASFIT_PLUGIN_EXP_ADD( ff_beaucage_exppowlaw )
SASFIT_PLUGIN_EXP_ADD( ff_spinodal )
SASFIT_PLUGIN_EXP_ADD( ff_debye_anderson_brumberger )
SASFIT_PLUGIN_EXP_ADD( ff_ornstein_zernike )
SASFIT_PLUGIN_EXP_ADD( ff_broad_peak )
SASFIT_PLUGIN_EXP_ADD( ff_teubner_strey )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

