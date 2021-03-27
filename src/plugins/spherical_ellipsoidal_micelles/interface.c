/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(14)
SASFIT_PLUGIN_EXP_ADD( ff_sphere_chains_rw__nagg )
SASFIT_PLUGIN_EXP_ADD( ff_sphere_chains_rw_ )
SASFIT_PLUGIN_EXP_ADD( ff_sphere_chains_rw__rc )
SASFIT_PLUGIN_EXP_ADD( ff_sphere_chains_saw__nagg )
SASFIT_PLUGIN_EXP_ADD( ff_sphere_chains_saw_ )
SASFIT_PLUGIN_EXP_ADD( ff_sphere_chains_saw__rc )
SASFIT_PLUGIN_EXP_ADD( ff_sphere_r__a_nagg )
SASFIT_PLUGIN_EXP_ADD( ff_sphere_r__a )
SASFIT_PLUGIN_EXP_ADD( ff_sphere_r__a_rc )
SASFIT_PLUGIN_EXP_ADD( ff_sphere_smooth_interface_r__a_nagg )
SASFIT_PLUGIN_EXP_ADD( ff_sphere_2nd_r__a_rc )
SASFIT_PLUGIN_EXP_ADD( ff_ell_chains_rw__nagg )
SASFIT_PLUGIN_EXP_ADD( ff_ell_chains_rw_ )
SASFIT_PLUGIN_EXP_ADD( ff_ell_chains_rw__rc )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_BEGIN( 7 )
SASFIT_PLUGIN_IMP_ADD( sq_freely_joined_chain_of_rods )
SASFIT_PLUGIN_IMP_ADD( sq_koyama_worms )
SASFIT_PLUGIN_IMP_ADD( sq_p__q___kholodenko_worm )
SASFIT_PLUGIN_IMP_ADD( sq_p__q___worm_ps1_ )
SASFIT_PLUGIN_IMP_ADD( sq_p__q___worm_ps2_ )
SASFIT_PLUGIN_IMP_ADD( sq_p__q___worm_ps3_ )
SASFIT_PLUGIN_IMP_ADD( ff_generalized_gaussian_coil )
SASFIT_PLUGIN_IMP_END

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

