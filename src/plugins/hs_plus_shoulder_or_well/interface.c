/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(6)
SASFIT_PLUGIN_EXP_ADD( sq_sticky_hard_sphere )
SASFIT_PLUGIN_EXP_ADD( sq_sticky_hard_sphere_2 )
SASFIT_PLUGIN_EXP_ADD( sq_square_well_potential )
SASFIT_PLUGIN_EXP_ADD( sq_square_well_potential_2 )
SASFIT_PLUGIN_EXP_ADD( sq_thin_square_well_potential )
SASFIT_PLUGIN_EXP_ADD( sq_two_piecewise_constant )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

