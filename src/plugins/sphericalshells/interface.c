/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(10)
SASFIT_PLUGIN_EXP_ADD( ff_multilamellar_vesicle )
SASFIT_PLUGIN_EXP_ADD( ff_spherical_shell_iii )
SASFIT_PLUGIN_EXP_ADD( ff_spherical_shell_ii )
SASFIT_PLUGIN_EXP_ADD( ff_spherical_shell_i )
SASFIT_PLUGIN_EXP_ADD( ff_sphere )
SASFIT_PLUGIN_EXP_ADD( ff_rnd_multilamellar_vesicle )
SASFIT_PLUGIN_EXP_ADD( ff_rnd_multilamellar_vesicle_2 )
SASFIT_PLUGIN_EXP_ADD( ff_mlv_frielinghaus )
SASFIT_PLUGIN_EXP_ADD( ff_bilayered_vesicle )
SASFIT_PLUGIN_EXP_ADD( ff_hollow_sphere )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

