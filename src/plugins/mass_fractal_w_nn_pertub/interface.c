/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(3)
SASFIT_PLUGIN_EXP_ADD( sq_mass_fractal_incl__nn_pertubation )
SASFIT_PLUGIN_EXP_ADD( sq_dlca_incl_nn_pertubation )
SASFIT_PLUGIN_EXP_ADD( sq_rlca_incl_nn_pertubation )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif
