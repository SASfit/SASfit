/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(6)
SASFIT_PLUGIN_EXP_ADD( ff_benoitstar )
SASFIT_PLUGIN_EXP_ADD( ff_polydispersestar )
SASFIT_PLUGIN_EXP_ADD( ff_dozierstar )
SASFIT_PLUGIN_EXP_ADD( ff_dozierstar2 )
SASFIT_PLUGIN_EXP_ADD( ff_broken_rods_star )
SASFIT_PLUGIN_EXP_ADD( ff_broken_worms_star )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

