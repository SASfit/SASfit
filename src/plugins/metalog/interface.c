/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(18)
SASFIT_PLUGIN_EXP_ADD( ff_metalog_sphere )
SASFIT_PLUGIN_EXP_ADD( ff_metaloglog_sphere )
SASFIT_PLUGIN_EXP_ADD( ff_metalog_log_profile_sphere )
SASFIT_PLUGIN_EXP_ADD( ff_metalog_log_profile_PcsCyl )
SASFIT_PLUGIN_EXP_ADD( ff_metalog_log_profile_PcsPlanar )
SASFIT_PLUGIN_EXP_ADD( ff_metalog_log_profile )
SASFIT_PLUGIN_EXP_ADD( ff_metalog_logit_profile_sphere )
SASFIT_PLUGIN_EXP_ADD( ff_metalog_logit_profile_PcsCyl )
SASFIT_PLUGIN_EXP_ADD( ff_metalog_logit_profile_PcsPlanar )
SASFIT_PLUGIN_EXP_ADD( ff_metalog_logit_profile )
SASFIT_PLUGIN_EXP_ADD( ff_metalog_clipped_profile_sphere )
SASFIT_PLUGIN_EXP_ADD( ff_metalog_clipped_profile_PcsCyl )
SASFIT_PLUGIN_EXP_ADD( ff_metalog_clipped_profile_PcsPlanar )
SASFIT_PLUGIN_EXP_ADD( ff_metalog_clipped_profile )
SASFIT_PLUGIN_EXP_ADD( ff_metaloglogit_sphere )
SASFIT_PLUGIN_EXP_ADD( sd_metalog_clipped )
SASFIT_PLUGIN_EXP_ADD( sd_metalog_log_clipped )
SASFIT_PLUGIN_EXP_ADD( sd_metalog_logit )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

