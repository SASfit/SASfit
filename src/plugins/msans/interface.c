/*
 * Author(s) of this file::
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(4)
SASFIT_PLUGIN_EXP_ADD( ff_msas_spheres )
SASFIT_PLUGIN_EXP_ADD( ff_msas_polydisp_spheres )
SASFIT_PLUGIN_EXP_ADD( ff_msas_gdab )
SASFIT_PLUGIN_EXP_ADD( ff_msas_ggc )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_BEGIN( 4 )
SASFIT_PLUGIN_IMP_ADD( ff_gz_dab )
SASFIT_PLUGIN_IMP_ADD( ff_gz_generalized_dab )
SASFIT_PLUGIN_IMP_ADD( ff_gz_ggc )
SASFIT_PLUGIN_IMP_ADD( ff_gz_sphere )
SASFIT_PLUGIN_IMP_END

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

