/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(8)
SASFIT_PLUGIN_EXP_ADD( ff_spheroid_R )
SASFIT_PLUGIN_EXP_ADD( ff_spheroid_V )
SASFIT_PLUGIN_EXP_ADD( ff_ellipsoidal_shell_0 )
SASFIT_PLUGIN_EXP_ADD( ff_ellipsoidal_shell_0t )
SASFIT_PLUGIN_EXP_ADD( ff_ellipsoidal_shell_1 )
SASFIT_PLUGIN_EXP_ADD( ff_ellipsoidal_shell_1t )
SASFIT_PLUGIN_EXP_ADD( ff_ellipsoidal_shell_2 )
SASFIT_PLUGIN_EXP_ADD( ff_ellipsoidal_shell_2t )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

