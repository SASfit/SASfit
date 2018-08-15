/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(7)
SASFIT_PLUGIN_EXP_ADD( ff_gauss )
SASFIT_PLUGIN_EXP_ADD( ff_gauss2 )
SASFIT_PLUGIN_EXP_ADD( ff_gauss3 )
SASFIT_PLUGIN_EXP_ADD( ff_gauspoly )
SASFIT_PLUGIN_EXP_ADD( ff_generalized_gaussian_coil )
SASFIT_PLUGIN_EXP_ADD( ff_generalized_gaussian_coil_2 )
SASFIT_PLUGIN_EXP_ADD( ff_generalized_gaussian_coil_3 )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

