/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(10)
SASFIT_PLUGIN_EXP_ADD( ff_fuzzysphere )
SASFIT_PLUGIN_EXP_ADD( ff_radial_profile_of_fuzzysphere )
SASFIT_PLUGIN_EXP_ADD( ff_coreshellmicrogel )
SASFIT_PLUGIN_EXP_ADD( ff_radial_profile_of_coreshellmicrogel )
SASFIT_PLUGIN_EXP_ADD( ff_expshell )
SASFIT_PLUGIN_EXP_ADD( ff_radial_profile_of_expshell )
SASFIT_PLUGIN_EXP_ADD( ff_linshell )
SASFIT_PLUGIN_EXP_ADD( ff_radial_profile_of_linshell )
SASFIT_PLUGIN_EXP_ADD( ff_linshell2 )
SASFIT_PLUGIN_EXP_ADD( ff_radial_profile_of_linshell2 )
SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

