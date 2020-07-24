/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(1)
SASFIT_PLUGIN_EXP_ADD( sq_extended_cluster_sq )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_BEGIN( 7 )
SASFIT_PLUGIN_IMP_ADD( ff_dab_epsilon )
SASFIT_PLUGIN_IMP_ADD( sq_hard_sphere__4_3_ )
SASFIT_PLUGIN_IMP_ADD( sq_hard_sphere__cs_ )
SASFIT_PLUGIN_IMP_ADD( sq_hard_sphere__gh_ )
SASFIT_PLUGIN_IMP_ADD( sq_hard_sphere__lhr_ )
SASFIT_PLUGIN_IMP_ADD( sq_hard_sphere__mv_ )
SASFIT_PLUGIN_IMP_ADD( sq_hard_sphere__py_ )
SASFIT_PLUGIN_IMP_END

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

