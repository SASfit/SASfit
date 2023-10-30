/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(14)
SASFIT_PLUGIN_EXP_ADD( ff_rod )
SASFIT_PLUGIN_EXP_ADD( ff_disc )
SASFIT_PLUGIN_EXP_ADD( ff_porodcylinder )
SASFIT_PLUGIN_EXP_ADD( ff_porodcylinder_v2 )
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
SASFIT_PLUGIN_IMP_BEGIN( 2 )
SASFIT_PLUGIN_IMP_ADD( ff_pcs_ellcylsh )
SASFIT_PLUGIN_IMP_ADD( sq_p__q___rod )
SASFIT_PLUGIN_IMP_END

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

