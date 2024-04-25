/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(1)
SASFIT_PLUGIN_EXP_ADD( sq_clusteryukawa )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_BEGIN( 5 )
SASFIT_PLUGIN_IMP_ADD( sq_paracrystal_1d_random )
SASFIT_PLUGIN_IMP_ADD( sq_paracrystal_orig_1d_random )
SASFIT_PLUGIN_IMP_ADD( sq_pc_random_flight )
SASFIT_PLUGIN_IMP_ADD( sq_simple_sq_power_law )
SASFIT_PLUGIN_IMP_ADD( sq_one_yukawa )
SASFIT_PLUGIN_IMP_END

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

