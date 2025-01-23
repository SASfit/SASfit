/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(20)
SASFIT_PLUGIN_EXP_ADD( sd_fgld_ )
SASFIT_PLUGIN_EXP_ADD( ff_fgld_profile )
SASFIT_PLUGIN_EXP_ADD( ff_fgld_profile_sphere )
SASFIT_PLUGIN_EXP_ADD( ff_fgld_profile_PcsCyl )
SASFIT_PLUGIN_EXP_ADD( ff_fgld_profile_PcsPlanar )
SASFIT_PLUGIN_EXP_ADD( ff_fgld_sphere )
SASFIT_PLUGIN_EXP_ADD( sd_egu_w_l )
SASFIT_PLUGIN_EXP_ADD( ff_egu_w_l_profile )
SASFIT_PLUGIN_EXP_ADD( ff_egu_w_l_profile_sphere )
SASFIT_PLUGIN_EXP_ADD( ff_egu_w_l_profile_PcsCyl )
SASFIT_PLUGIN_EXP_ADD( ff_egu_w_l_profile_PcsPlanar )
SASFIT_PLUGIN_EXP_ADD( ff_egu_w_l_sphere ) 
SASFIT_PLUGIN_EXP_ADD( sd_g_c_e__ )
SASFIT_PLUGIN_EXP_ADD( sd_ll_l_gl__ )
SASFIT_PLUGIN_EXP_ADD( sd_p_w_gl__ )
SASFIT_PLUGIN_EXP_ADD( sd_n_u_gl__ )
SASFIT_PLUGIN_EXP_ADD( sd_u_e_gl__ )
SASFIT_PLUGIN_EXP_ADD( sd_w_l_e_ )
SASFIT_PLUGIN_EXP_ADD( sd_n_l_c_ )
SASFIT_PLUGIN_EXP_ADD( sd_e_l_ll_ )
SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

