/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(5)
SASFIT_PLUGIN_EXP_ADD( ff_metaloglog_sphere )
SASFIT_PLUGIN_EXP_ADD( ff_metaloglogit_sphere )
SASFIT_PLUGIN_EXP_ADD( sd_metalog_0_inf )
SASFIT_PLUGIN_EXP_ADD( sd_metalog_0_bu )
SASFIT_PLUGIN_EXP_ADD( sd_metalog_bl_bu )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

