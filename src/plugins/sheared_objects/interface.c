/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(12)
SASFIT_PLUGIN_EXP_ADD( ff_sheared_cylinders__maier_saupe_ )
SASFIT_PLUGIN_EXP_ADD( ff_sheared_cylinders__heavyside_ )
SASFIT_PLUGIN_EXP_ADD( ff_sheared_cylinders__boltzmann_ )
SASFIT_PLUGIN_EXP_ADD( ff_sheared_cylinders__gauss_ )
SASFIT_PLUGIN_EXP_ADD( ff_sheared_cylinders__hayterpenfold_ )
SASFIT_PLUGIN_EXP_ADD( ff_sheared_cylinders__onsager_ )

SASFIT_PLUGIN_EXP_ADD( ff_sheared_spheroids__maier_saupe_ )
SASFIT_PLUGIN_EXP_ADD( ff_sheared_spheroids__heavyside_ )
SASFIT_PLUGIN_EXP_ADD( ff_sheared_spheroids__boltzmann_ )
SASFIT_PLUGIN_EXP_ADD( ff_sheared_spheroids__gauss_ )
SASFIT_PLUGIN_EXP_ADD( ff_sheared_spheroids__hayterpenfold_ )
SASFIT_PLUGIN_EXP_ADD( ff_sheared_spheroids__onsager_ )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

