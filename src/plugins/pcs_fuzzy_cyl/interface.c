/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(8)
SASFIT_PLUGIN_EXP_ADD( ff_boucher_cyl )
SASFIT_PLUGIN_EXP_ADD( ff_profile_boucher_cyl )
SASFIT_PLUGIN_EXP_ADD( ff_boucher2_cyl )
SASFIT_PLUGIN_EXP_ADD( ff_profile_boucher2_cyl )
SASFIT_PLUGIN_EXP_ADD( ff_fuzzy_cyl )
SASFIT_PLUGIN_EXP_ADD( ff_profile_fuzzy_cyl )
SASFIT_PLUGIN_EXP_ADD( ff_linsh_cyl )
SASFIT_PLUGIN_EXP_ADD( ff_profile_linsh_cyl )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

