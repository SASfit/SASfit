/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(6)
SASFIT_PLUGIN_EXP_ADD( ff_fract_hyperbranched_polym )
SASFIT_PLUGIN_EXP_ADD( ff_f_funct_homo_polym )
SASFIT_PLUGIN_EXP_ADD( ff_comb_regular )
SASFIT_PLUGIN_EXP_ADD( ff_comb_random )
SASFIT_PLUGIN_EXP_ADD( ff_comb_hetero )
SASFIT_PLUGIN_EXP_ADD( ff_ideal_dendrimer )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

