/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(12)
SASFIT_PLUGIN_EXP_ADD( ff_disc_chain_rw__nagg )
SASFIT_PLUGIN_EXP_ADD( ff_disc_chain_rw__lc )
SASFIT_PLUGIN_EXP_ADD( ff_disc_chain_rw_ )
SASFIT_PLUGIN_EXP_ADD( ff_sphulv_chain_rw__nagg )
SASFIT_PLUGIN_EXP_ADD( ff_sphulv_chain_rw__tc )
SASFIT_PLUGIN_EXP_ADD( ff_sphulv_chain_rw_ )
SASFIT_PLUGIN_EXP_ADD( ff_ellulv_chain_rw__nagg )
SASFIT_PLUGIN_EXP_ADD( ff_ellulv_chain_rw__tc )
SASFIT_PLUGIN_EXP_ADD( ff_ellulv_chain_rw_ )
SASFIT_PLUGIN_EXP_ADD( ff_cylulv_chain_rw__nagg )
SASFIT_PLUGIN_EXP_ADD( ff_cylulv_chain_rw__tc )
SASFIT_PLUGIN_EXP_ADD( ff_cylulv_chain_rw_ )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

