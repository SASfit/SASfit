/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(4)
SASFIT_PLUGIN_EXP_ADD( ff_vesicle_peg_piecew_const )
SASFIT_PLUGIN_EXP_ADD( ff_vesicle_peg_piecew_const_thin_approx )
SASFIT_PLUGIN_EXP_ADD( ff_vesicle_peg_gaussian )
SASFIT_PLUGIN_EXP_ADD( ff_vesicle_peg_gaussian_thin_approx )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_BEGIN( 7 )
SASFIT_PLUGIN_IMP_ADD( ff_pcs_bilayergauss )
SASFIT_PLUGIN_IMP_ADD( ff_pcs_homogeneouscyl )
SASFIT_PLUGIN_IMP_ADD( ff_pcs_homogeneousplate )
SASFIT_PLUGIN_IMP_ADD( ff_pcs_plate_chains_rw_ )
SASFIT_PLUGIN_IMP_ADD( sq_p__q___thin_disc )
SASFIT_PLUGIN_IMP_ADD( sq_p__q___thin_ellipsoidal_shell )
SASFIT_PLUGIN_IMP_ADD( sq_p__q___thin_spherical_shell )
SASFIT_PLUGIN_IMP_END

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif
