/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(8)
SASFIT_PLUGIN_EXP_ADD( ff_boolean_intersection_model )
SASFIT_PLUGIN_EXP_ADD( ff_boolean_union_model )
SASFIT_PLUGIN_EXP_ADD( ff_dead_leaves_model )
SASFIT_PLUGIN_EXP_ADD( ff_clipped_random_waves_1 )
SASFIT_PLUGIN_EXP_ADD( ff_clipped_random_waves_2 )
SASFIT_PLUGIN_EXP_ADD( ff_clipped_random_waves_3 )
SASFIT_PLUGIN_EXP_ADD( ff_clipped_random_waves_4 )
SASFIT_PLUGIN_EXP_ADD( ff_clipped_random_waves_5 )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

