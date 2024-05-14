/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(18)
SASFIT_PLUGIN_EXP_ADD( ff_cyl_chains_rw__nagg )
SASFIT_PLUGIN_EXP_ADD( ff_cyl_chains_rw__rc )
SASFIT_PLUGIN_EXP_ADD( ff_cyl_chains_rw_ )
SASFIT_PLUGIN_EXP_ADD( ff_worm_chains_rw__nagg )
SASFIT_PLUGIN_EXP_ADD( ff_worm_chains_rw__rc )
SASFIT_PLUGIN_EXP_ADD( ff_worm_chains_rw_ )
SASFIT_PLUGIN_EXP_ADD( ff_rod_chains_rw__nagg )
SASFIT_PLUGIN_EXP_ADD( ff_rod_chains_rw__rc )
SASFIT_PLUGIN_EXP_ADD( ff_rod_chains_rw_ )
SASFIT_PLUGIN_EXP_ADD( ff_rod__r__a_nagg )
SASFIT_PLUGIN_EXP_ADD( ff_rod__r__a_rc )
SASFIT_PLUGIN_EXP_ADD( ff_rod__r__a )
SASFIT_PLUGIN_EXP_ADD( ff_rod_gauss_nagg )
SASFIT_PLUGIN_EXP_ADD( ff_rod_gauss_rc )
SASFIT_PLUGIN_EXP_ADD( ff_rod_gauss )
SASFIT_PLUGIN_EXP_ADD( ff_rod_exp_nagg )
SASFIT_PLUGIN_EXP_ADD( ff_rod_exp_rc )
SASFIT_PLUGIN_EXP_ADD( ff_rod_exp )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

