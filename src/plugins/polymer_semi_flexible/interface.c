/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(6)
SASFIT_PLUGIN_EXP_ADD( ff_worms_ps1_ )
SASFIT_PLUGIN_EXP_ADD( ff_worms_ps2_ )
SASFIT_PLUGIN_EXP_ADD( ff_worms_ps3_ )
SASFIT_PLUGIN_EXP_ADD( ff_kholodenko_worm )
SASFIT_PLUGIN_EXP_ADD( ff_freely_joined_chain_of_rods )
SASFIT_PLUGIN_EXP_ADD( ff_koyama_worm )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_BEGIN( 6 )
SASFIT_PLUGIN_IMP_ADD( sq_freely_joined_chain_of_rods )
SASFIT_PLUGIN_IMP_ADD( sq_koyama_worms )
SASFIT_PLUGIN_IMP_ADD( sq_p__q___kholodenko_worm )
SASFIT_PLUGIN_IMP_ADD( sq_p__q___worm_ps1_ )
SASFIT_PLUGIN_IMP_ADD( sq_p__q___worm_ps2_ )
SASFIT_PLUGIN_IMP_ADD( sq_p__q___worm_ps3_ )
SASFIT_PLUGIN_IMP_END

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

