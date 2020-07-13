/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(12)
SASFIT_PLUGIN_EXP_ADD( sq_modifiedcaille )
SASFIT_PLUGIN_EXP_ADD( sq_thermaldisorder )
SASFIT_PLUGIN_EXP_ADD( sq_paracrystalline )
SASFIT_PLUGIN_EXP_ADD( sq_perfect_ordered )
SASFIT_PLUGIN_EXP_ADD( sq_modifiedcaille_sd )
SASFIT_PLUGIN_EXP_ADD( sq_thermaldisorder_sd )
SASFIT_PLUGIN_EXP_ADD( sq_paracrystalline_sd )
SASFIT_PLUGIN_EXP_ADD( sq_perfect_ordered_sd )
SASFIT_PLUGIN_EXP_ADD( sq_modifiedcaille_sum )
SASFIT_PLUGIN_EXP_ADD( sq_thermaldisorder_sum )
SASFIT_PLUGIN_EXP_ADD( sq_paracrystalline_sum )
SASFIT_PLUGIN_EXP_ADD( sq_perfect_ordered_sum )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

