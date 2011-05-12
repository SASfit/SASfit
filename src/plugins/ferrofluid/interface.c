/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(16)
SASFIT_PLUGIN_EXP_ADD( ff_saturated_ff__chain__rw____psi )
SASFIT_PLUGIN_EXP_ADD( ff_saturated_ff__chain__rw____radial )
SASFIT_PLUGIN_EXP_ADD( ff_saturated_ff__chain__rw____magnetic )
SASFIT_PLUGIN_EXP_ADD( ff_saturated_ff__chain__rw____cross_term )
SASFIT_PLUGIN_EXP_ADD( ff_saturated_ff__chain__rw____cross_term_radial )
SASFIT_PLUGIN_EXP_ADD( ff_saturated_ff__chain__saw____psi )
SASFIT_PLUGIN_EXP_ADD( ff_saturated_ff__chain__saw____radial )
SASFIT_PLUGIN_EXP_ADD( ff_saturated_ff__chain__saw____magnetic )
SASFIT_PLUGIN_EXP_ADD( ff_saturated_ff__chain__saw____cross_term )
SASFIT_PLUGIN_EXP_ADD( ff_saturated_ff__chain__saw____cross_term_radial )
SASFIT_PLUGIN_EXP_ADD( ff_saturated_ff_chain_parabolic_psi )
SASFIT_PLUGIN_EXP_ADD( ff_saturated_ff_chain_parabolic_radial )
SASFIT_PLUGIN_EXP_ADD( ff_saturated_ff_chain_parabolic_magnetic )
SASFIT_PLUGIN_EXP_ADD( ff_saturated_ff_chain_parabolic_cross_term )
SASFIT_PLUGIN_EXP_ADD( ff_saturated_ff_chain_parabolic_cross_term_radial )
SASFIT_PLUGIN_EXP_ADD( ff_chain_parabolic_profile )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

