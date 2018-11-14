/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(13)
SASFIT_PLUGIN_EXP_ADD( ff_rod )
SASFIT_PLUGIN_EXP_ADD( ff_disc )
SASFIT_PLUGIN_EXP_ADD( ff_porodcylinder )
SASFIT_PLUGIN_EXP_ADD( ff_longcylinder )
SASFIT_PLUGIN_EXP_ADD( ff_flatcylinder )
SASFIT_PLUGIN_EXP_ADD( ff_cylinder )
SASFIT_PLUGIN_EXP_ADD( ff_longcylshell )
SASFIT_PLUGIN_EXP_ADD( ff_ellcylshell1 )
SASFIT_PLUGIN_EXP_ADD( ff_ellcylshell2 )
SASFIT_PLUGIN_EXP_ADD( ff_cylshell1 )
SASFIT_PLUGIN_EXP_ADD( ff_cylshell2 )
SASFIT_PLUGIN_EXP_ADD( ff_alignedcylshell )
SASFIT_PLUGIN_EXP_ADD( ff_torus )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

