/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(3)
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

