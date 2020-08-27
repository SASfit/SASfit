/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(8)
SASFIT_PLUGIN_EXP_ADD( ff_trapez__area )
SASFIT_PLUGIN_EXP_ADD( ff_symm__trapez__area )
SASFIT_PLUGIN_EXP_ADD( ff_general__trapez__area )
SASFIT_PLUGIN_EXP_ADD( ff_symm__general__trapez__area )
SASFIT_PLUGIN_EXP_ADD( ff_trapez__ampl_ )
SASFIT_PLUGIN_EXP_ADD( ff_symm__trapez__ampl_ )
SASFIT_PLUGIN_EXP_ADD( ff_general__trapez__ampl_ )
SASFIT_PLUGIN_EXP_ADD( ff_symm__general__trapez__ampl_ )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

