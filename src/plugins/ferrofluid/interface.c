/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(31)
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_rw_psi )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_rw_radial )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_rw_magnetic )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_rw_cross_term )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_rw_cross_term_radial )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_rw_psi_POLARIS )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_rw_psi_pp )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_rw_psi_mm )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_rw_psi_pm )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_rw_psi_mp )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_saw_psi )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_saw_radial )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_saw_magnetic )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_saw_cross_term )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_saw_cross_term_radial )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_saw_psi_POLARIS )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_saw_psi_pp )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_saw_psi_mm )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_saw_psi_pm )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_saw_psi_mp )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_parabolic_psi )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_parabolic_radial )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_parabolic_magnetic )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_parabolic_cross_term )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_parabolic_cross_term_radial )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_parabolic_psi_POLARIS )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_parabolic_psi_pp )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_parabolic_psi_mm )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_parabolic_psi_pm )
SASFIT_PLUGIN_EXP_ADD( ff_ff_chain_parabolic_psi_mp )
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

