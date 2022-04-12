/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(34)
SASFIT_PLUGIN_EXP_ADD( ff_cylinder_opo )
SASFIT_PLUGIN_EXP_ADD( ff_cylinder_opo_random )
SASFIT_PLUGIN_EXP_ADD( ff_parallelepiped_opo )
SASFIT_PLUGIN_EXP_ADD( ff_parallelepiped_opo_random )
SASFIT_PLUGIN_EXP_ADD( ff_ellipsoid_opo )
SASFIT_PLUGIN_EXP_ADD( ff_ellipsoid_opo_random )
SASFIT_PLUGIN_EXP_ADD( ff_superegg_opo )
SASFIT_PLUGIN_EXP_ADD( ff_superegg_opo_random )
SASFIT_PLUGIN_EXP_ADD( ff_superellipsoid_opo )
SASFIT_PLUGIN_EXP_ADD( ff_superellipsoid_opo_random )
SASFIT_PLUGIN_EXP_ADD( ff_superquadrics_opo )
SASFIT_PLUGIN_EXP_ADD( ff_superquadrics_opo_random )
SASFIT_PLUGIN_EXP_ADD( ff_supershape_opo )
SASFIT_PLUGIN_EXP_ADD( ff_supershape_opo_random )
SASFIT_PLUGIN_EXP_ADD( ff_supertoroid_helix_opo )
SASFIT_PLUGIN_EXP_ADD( ff_supertoroid_helix_opo_random )
SASFIT_PLUGIN_EXP_ADD( ff_rationalsupershape_opo )
SASFIT_PLUGIN_EXP_ADD( ff_rationalsupershape_opo_random )
SASFIT_PLUGIN_EXP_ADD( ff_tetrahedra__opo_ )
SASFIT_PLUGIN_EXP_ADD( ff_tetrahedra__opo_random_ )
SASFIT_PLUGIN_EXP_ADD( ff_octahedra__opo_ )
SASFIT_PLUGIN_EXP_ADD( ff_octahedra__opo_random_ )
SASFIT_PLUGIN_EXP_ADD( ff_rhombic_dodecahedra__opo_ )
SASFIT_PLUGIN_EXP_ADD( ff_rhombic_dodecahedra__opo_random_ )
SASFIT_PLUGIN_EXP_ADD( ff_icosahedra__opo_ )
SASFIT_PLUGIN_EXP_ADD( ff_icosahedra__opo_random_ )
SASFIT_PLUGIN_EXP_ADD( ff_hexahedra__opo_ )
SASFIT_PLUGIN_EXP_ADD( ff_hexahedra__opo_random_ )
SASFIT_PLUGIN_EXP_ADD( ff_cone_opo )
SASFIT_PLUGIN_EXP_ADD( ff_cone_opo_random )
SASFIT_PLUGIN_EXP_ADD( ff_pyramid4_opo )
SASFIT_PLUGIN_EXP_ADD( ff_pyramid4_opo_random )
SASFIT_PLUGIN_EXP_ADD( ff_cone6_opo )
SASFIT_PLUGIN_EXP_ADD( ff_cone6_opo_random )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

