/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(4)
SASFIT_PLUGIN_EXP_ADD( ff_kappa_generalized_gamma_distribution__area_ )
SASFIT_PLUGIN_EXP_ADD( ff_kappa_generalized_gamma_distribution__ampl_ )
SASFIT_PLUGIN_EXP_ADD( ff_kappa_logistic_distribution__area_ )
SASFIT_PLUGIN_EXP_ADD( ff_kappa_logistic_distribution__ampl_ )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

