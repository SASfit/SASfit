/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(17)
SASFIT_PLUGIN_EXP_ADD( sq_mass_fractal__exp__x_a__cut_off_ )
SASFIT_PLUGIN_EXP_ADD( sq_mass_fractal__overapsph_cut_off_ )
SASFIT_PLUGIN_EXP_ADD( sq_mass_fractal__gaussian_cut_off_ )
SASFIT_PLUGIN_EXP_ADD( sq_mass_fractal__exp_cut_off_ )
SASFIT_PLUGIN_EXP_ADD( ff_fisher_burford )
SASFIT_PLUGIN_EXP_ADD( ff_massfractexp )
SASFIT_PLUGIN_EXP_ADD( ff_massfractgauss )
SASFIT_PLUGIN_EXP_ADD( ff_aggregate__exp__x_a__cut_off_ )
SASFIT_PLUGIN_EXP_ADD( ff_aggregate__overlapsph_cut_off_ )
SASFIT_PLUGIN_EXP_ADD( ff_dlcaggregation )
SASFIT_PLUGIN_EXP_ADD( ff_rlcaggregation )
SASFIT_PLUGIN_EXP_ADD( ff_stackdiscs )
SASFIT_PLUGIN_EXP_ADD( ff_dumbbellshell )
SASFIT_PLUGIN_EXP_ADD( ff_two_attached_spheres )
SASFIT_PLUGIN_EXP_ADD( ff_two_attached_spheres_w_distr )
SASFIT_PLUGIN_EXP_ADD( ff_doubleshellchain )
SASFIT_PLUGIN_EXP_ADD( ff_tetrahedrondoubleshell )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

