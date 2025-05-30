/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(10)
SASFIT_PLUGIN_EXP_ADD( ff_generalized_hyperbolic__ampl_ )
SASFIT_PLUGIN_EXP_ADD( ff_generalized_hyperbolic__area_ )
SASFIT_PLUGIN_EXP_ADD( ff_generalized_extreme_value__ampl_ )
SASFIT_PLUGIN_EXP_ADD( ff_generalized_extreme_value__area_ )
SASFIT_PLUGIN_EXP_ADD( ff_exponentiated_generalized_pareto__ampl_ )
SASFIT_PLUGIN_EXP_ADD( ff_exponentiated_generalized_pareto__area_ )
SASFIT_PLUGIN_EXP_ADD( ff_generalized_normal__ampl_ )
SASFIT_PLUGIN_EXP_ADD( ff_generalized_normal__area_ )
SASFIT_PLUGIN_EXP_ADD( ff_generalized_inverse_gaussian__ampl_ )
SASFIT_PLUGIN_EXP_ADD( ff_generalized_inverse_gaussian__area_ )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

