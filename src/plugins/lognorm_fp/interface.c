/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(4)
SASFIT_PLUGIN_EXP_ADD( sd_std_lognorm )
SASFIT_PLUGIN_EXP_ADD( sd_shifted_lognorm )
SASFIT_PLUGIN_EXP_ADD( sd_lognorm_fp )
SASFIT_PLUGIN_EXP_ADD( sd_bilognorm )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

