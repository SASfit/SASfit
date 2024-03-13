/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(34)
SASFIT_PLUGIN_EXP_ADD( ff_disc_sd_homoxs )
SASFIT_PLUGIN_EXP_ADD( ff_ellsh_sd_homoxs )
SASFIT_PLUGIN_EXP_ADD( ff_cylsh_sd_homoxs )
SASFIT_PLUGIN_EXP_ADD( ff_disc_sd_bilayergauss )
SASFIT_PLUGIN_EXP_ADD( ff_ellsh_sd_bilayergauss )
SASFIT_PLUGIN_EXP_ADD( ff_cylsh_sd_bilayergauss )

SASFIT_PLUGIN_EXP_ADD( ff_rod_sd_ellcylsh )
SASFIT_PLUGIN_EXP_ADD( ff_worm_ps1_sd_ellcylsh )
SASFIT_PLUGIN_EXP_ADD( ff_worm_ps2_sd_ellcylsh )
SASFIT_PLUGIN_EXP_ADD( ff_worm_ps3_sd_ellcylsh )
SASFIT_PLUGIN_EXP_ADD( ff_worm_kholodenko_sd_ellcylsh )

SASFIT_PLUGIN_EXP_ADD( ff_pcs_homogeneousplate )
SASFIT_PLUGIN_EXP_ADD( ff_pcs_plate_chains_rw_ )
SASFIT_PLUGIN_EXP_ADD( ff_pcs_layeredcentrosymmetricxs )
SASFIT_PLUGIN_EXP_ADD( ff_pcs_bilayergauss )
SASFIT_PLUGIN_EXP_ADD( ff_pcs_twoinfinitelythinlayers )

SASFIT_PLUGIN_EXP_ADD( ff_pcs_homogeneouscyl )
SASFIT_PLUGIN_EXP_ADD( ff_pcs_ellcylsh )

SASFIT_PLUGIN_EXP_ADD( sq_p__q___thin_disc )
SASFIT_PLUGIN_EXP_ADD( sq_p__q___thin_spherical_shell )
SASFIT_PLUGIN_EXP_ADD( sq_p__q___thin_ellipsoidal_shell )
SASFIT_PLUGIN_EXP_ADD( sq_p__q___thin_hollow_cylinder )

SASFIT_PLUGIN_EXP_ADD( sq_p__q___rod )
SASFIT_PLUGIN_EXP_ADD( sq_p__q___worm_ps1_ )
SASFIT_PLUGIN_EXP_ADD( sq_p__q___worm_ps2_ )
SASFIT_PLUGIN_EXP_ADD( sq_p__q___worm_ps3_ )
SASFIT_PLUGIN_EXP_ADD( sq_p__q___worm_ps3_RPA )
SASFIT_PLUGIN_EXP_ADD( sq_p__q___worm_ps3_PRISM1 )
SASFIT_PLUGIN_EXP_ADD( sq_p__q___worm_ps3_PRISM2 )
SASFIT_PLUGIN_EXP_ADD( sq_p__q___worm_ps3_PRISM3 )
SASFIT_PLUGIN_EXP_ADD( sq_p__q___kholodenko_worm )
SASFIT_PLUGIN_EXP_ADD( sq_freely_joined_chain_of_rods )
SASFIT_PLUGIN_EXP_ADD( sq_koyama_worms )
SASFIT_PLUGIN_EXP_ADD( sq_gGC )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_BEGIN( 1 )
SASFIT_PLUGIN_IMP_ADD( ff_generalized_gaussian_coil )
SASFIT_PLUGIN_IMP_END

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

