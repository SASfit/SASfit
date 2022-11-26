/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(1)
SASFIT_PLUGIN_EXP_ADD( sq_clusteryukawa )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_BEGIN( 17 )
SASFIT_PLUGIN_IMP_ADD( sq_cluster__linear_chain_ )
SASFIT_PLUGIN_IMP_ADD( sq_cluster__tetrahedron_ )
SASFIT_PLUGIN_IMP_ADD( sq_dlca_incl_nn_pertubation )
SASFIT_PLUGIN_IMP_ADD( sq_freely_joined_chain_of_rods )
SASFIT_PLUGIN_IMP_ADD( sq_hcp_iso_gaussian )
SASFIT_PLUGIN_IMP_ADD( sq_hcp_iso_lorentzian )
SASFIT_PLUGIN_IMP_ADD( sq_hcp_iso_pearson )
SASFIT_PLUGIN_IMP_ADD( sq_macro_ion_rmsa_1 )
SASFIT_PLUGIN_IMP_ADD( sq_macro_ion_rmsa_2 )
SASFIT_PLUGIN_IMP_ADD( sq_macro_ion_rmsa_3 )
SASFIT_PLUGIN_IMP_ADD( sq_one_d_hard_spheres_random_orient )
SASFIT_PLUGIN_IMP_ADD( sq_paracrystal_1d_random )
SASFIT_PLUGIN_IMP_ADD( sq_paracrystal_orig_1d_random )
SASFIT_PLUGIN_IMP_ADD( sq_pc_random_flight )
SASFIT_PLUGIN_IMP_ADD( sq_simple_sq_power_law )
SASFIT_PLUGIN_IMP_ADD( sq_two_yukawa )
SASFIT_PLUGIN_IMP_ADD( sq_one_yukawa )
SASFIT_PLUGIN_IMP_END

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

