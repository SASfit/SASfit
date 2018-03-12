/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(11)
SASFIT_PLUGIN_EXP_ADD( ff_spheroid_R )
SASFIT_PLUGIN_EXP_ADD( ff_spheroid_nu )
SASFIT_PLUGIN_EXP_ADD( ff_spheroid_V )
SASFIT_PLUGIN_EXP_ADD( ff_ellipsoidal_shell_0 )
SASFIT_PLUGIN_EXP_ADD( ff_ellipsoidal_shell_0t )
SASFIT_PLUGIN_EXP_ADD( ff_ellipsoidal_shell_re )
SASFIT_PLUGIN_EXP_ADD( ff_ellipsoidal_shell_re_t )
SASFIT_PLUGIN_EXP_ADD( ff_ellipsoidal_shell_rp )
SASFIT_PLUGIN_EXP_ADD( ff_ellipsoidal_shell_rp_t )
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

