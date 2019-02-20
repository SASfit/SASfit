/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(28)
SASFIT_PLUGIN_EXP_ADD( ff_a_bsin2_csin4__deg_ )
SASFIT_PLUGIN_EXP_ADD( ff_a_bsin2_csin4__rad_ )
SASFIT_PLUGIN_EXP_ADD( ff_maiersaupe__deg_ )
SASFIT_PLUGIN_EXP_ADD( ff_maiersaupe__rad_ )
SASFIT_PLUGIN_EXP_ADD( ff_elliptically_averaged_deg_ )
SASFIT_PLUGIN_EXP_ADD( ff_elliptically_averaged_rad_ )
SASFIT_PLUGIN_EXP_ADD( ff_azimuthal_sheared_cylinders_boltzmann )
SASFIT_PLUGIN_EXP_ADD( ff_azimuthal_sheared_cylinders_gauss )
SASFIT_PLUGIN_EXP_ADD( ff_azimuthal_sheared_cylinders_hayterpenfold )
SASFIT_PLUGIN_EXP_ADD( ff_azimuthal_sheared_cylinders_heavyside )
SASFIT_PLUGIN_EXP_ADD( ff_azimuthal_sheared_cylinders_maier_saupe )
SASFIT_PLUGIN_EXP_ADD( ff_azimuthal_sheared_cylinders_onsager )
SASFIT_PLUGIN_EXP_ADD( ff_azimuthal_sheared_spheroid_boltzmann )
SASFIT_PLUGIN_EXP_ADD( ff_azimuthal_sheared_spheroid_gauss )
SASFIT_PLUGIN_EXP_ADD( ff_azimuthal_sheared_spheroid_hayterpenfold )
SASFIT_PLUGIN_EXP_ADD( ff_azimuthal_sheared_spheroid_heavyside )
SASFIT_PLUGIN_EXP_ADD( ff_azimuthal_sheared_spheroid_maier_saupe )
SASFIT_PLUGIN_EXP_ADD( ff_azimuthal_sheared_spheroid_onsager )
SASFIT_PLUGIN_EXP_ADD( ff_longstructures__kratky_ms_deg_ )
SASFIT_PLUGIN_EXP_ADD( ff_longstructures__kratky_ms_rad_ )
SASFIT_PLUGIN_EXP_ADD( ff_longstructures__kratky_onsager_deg_ )
SASFIT_PLUGIN_EXP_ADD( ff_longstructures__kratky_onsager_rad_ )
SASFIT_PLUGIN_EXP_ADD( ff_longstructures__ln_ms_deg_ )
SASFIT_PLUGIN_EXP_ADD( ff_longstructures__ln_ms_rad_ )
SASFIT_PLUGIN_EXP_ADD( ff_longstructures__ln_onsager_deg_ )
SASFIT_PLUGIN_EXP_ADD( ff_longstructures__ln_onsager_rad_ )
SASFIT_PLUGIN_EXP_ADD( ff_affine_shrinkage_deg )
SASFIT_PLUGIN_EXP_ADD( ff_affine_shrinkage_rad )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_BEGIN( 12 )
SASFIT_PLUGIN_IMP_ADD( ff_sheared_cylinders__boltzmann_ )
SASFIT_PLUGIN_IMP_ADD( ff_sheared_cylinders__gauss_ )
SASFIT_PLUGIN_IMP_ADD( ff_sheared_cylinders__hayterpenfold_ )
SASFIT_PLUGIN_IMP_ADD( ff_sheared_cylinders__heavyside_ )
SASFIT_PLUGIN_IMP_ADD( ff_sheared_cylinders__maier_saupe_ )
SASFIT_PLUGIN_IMP_ADD( ff_sheared_cylinders__onsager_ )
SASFIT_PLUGIN_IMP_ADD( ff_sheared_spheroids__boltzmann_ )
SASFIT_PLUGIN_IMP_ADD( ff_sheared_spheroids__gauss_ )
SASFIT_PLUGIN_IMP_ADD( ff_sheared_spheroids__hayterpenfold_ )
SASFIT_PLUGIN_IMP_ADD( ff_sheared_spheroids__heavyside_ )
SASFIT_PLUGIN_IMP_ADD( ff_sheared_spheroids__maier_saupe_ )
SASFIT_PLUGIN_IMP_ADD( ff_sheared_spheroids__onsager_ )
SASFIT_PLUGIN_IMP_END

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

