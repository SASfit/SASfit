/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(6)
SASFIT_PLUGIN_EXP_ADD( ff_q_weibull__area_ )
SASFIT_PLUGIN_EXP_ADD( ff_q_weibull__ampl_ )
SASFIT_PLUGIN_EXP_ADD( ff_q_gaussian__area_ )
SASFIT_PLUGIN_EXP_ADD( ff_q_gaussian__ampl_ )
SASFIT_PLUGIN_EXP_ADD( ff_q_exponential__area_ )
SASFIT_PLUGIN_EXP_ADD( ff_q_exponential__ampl_ )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

