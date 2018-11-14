/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(3)
SASFIT_PLUGIN_EXP_ADD( ff_deformed_ring_polymer__bmb_ )
SASFIT_PLUGIN_EXP_ADD( ff_deformed_generalized_gaussian_chain )
SASFIT_PLUGIN_EXP_ADD( ff_deformed_ring_polymer__bz_ )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_BEGIN( 4 )
SASFIT_PLUGIN_IMP_ADD( ff_generalized_gaussian_coil )
SASFIT_PLUGIN_IMP_ADD( ff_ringpolymerBMB )
SASFIT_PLUGIN_IMP_ADD( ff_ringpolymerBZ )
SASFIT_PLUGIN_IMP_ADD( ff_ringpolymerCasassa )
SASFIT_PLUGIN_IMP_END

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

