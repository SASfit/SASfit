/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(6)
SASFIT_PLUGIN_EXP_ADD( sd_p_guwelop )
SASFIT_PLUGIN_EXP_ADD( ff_guwelop_sphere )
SASFIT_PLUGIN_EXP_ADD( ff_guwelop_profile )
SASFIT_PLUGIN_EXP_ADD( sd_egu_w_l )
SASFIT_PLUGIN_EXP_ADD( ff_egu_w_l_sphere )
SASFIT_PLUGIN_EXP_ADD( ff_egu_w_l_profile )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

