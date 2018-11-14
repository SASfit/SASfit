/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(6)
SASFIT_PLUGIN_EXP_ADD( ff_a_bsin2_csin4__deg_ )
SASFIT_PLUGIN_EXP_ADD( ff_a_bsin2_csin4__rad_ )
SASFIT_PLUGIN_EXP_ADD( ff_maiersaupe__deg_ )
SASFIT_PLUGIN_EXP_ADD( ff_maiersaupe__rad_ )
SASFIT_PLUGIN_EXP_ADD( ff_elliptically_averaged_deg_ )
SASFIT_PLUGIN_EXP_ADD( ff_elliptically_averaged_rad_ )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

