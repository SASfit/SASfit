# cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_azimuthal)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	sasfit_ff_a_bsin2_csin4__deg_.c
	sasfit_ff_a_bsin2_csin4__rad_.c
	sasfit_ff_maiersaupe__deg_.c
	sasfit_ff_maiersaupe__rad_.c
	sasfit_ff_elliptically_averaging_deg_.c
	sasfit_ff_elliptically_averaging_rad_.c
	sasfit_ff_azimuthal_sheared_cylinders_boltzmann.c
	sasfit_ff_azimuthal_sheared_cylinders_gauss.c
	sasfit_ff_azimuthal_sheared_cylinders_hayterpenfold.c
	sasfit_ff_azimuthal_sheared_cylinders_heavyside.c
	sasfit_ff_azimuthal_sheared_cylinders_maier_saupe.c
	sasfit_ff_azimuthal_sheared_cylinders_onsager.c
	sasfit_ff_azimuthal_sheared_spheroid_boltzmann.c
	sasfit_ff_azimuthal_sheared_spheroid_gauss.c
	sasfit_ff_azimuthal_sheared_spheroid_hayterpenfold.c
	sasfit_ff_azimuthal_sheared_spheroid_heavyside.c
	sasfit_ff_azimuthal_sheared_spheroid_maier_saupe.c
	sasfit_ff_azimuthal_sheared_spheroid_onsager.c
	sasfit_ff_azimuthal_utils.c
	sasfit_ff_longstructures__kratky_ms_deg_.c
	sasfit_ff_longstructures__kratky_ms_rad_.c
	sasfit_ff_longstructures__kratky_onsager_deg_.c
	sasfit_ff_longstructures__kratky_onsager_rad_.c
	sasfit_ff_longstructures__ln_ms_deg_.c
	sasfit_ff_longstructures__ln_ms_rad_.c 
	sasfit_ff_longstructures__ln_onsager_deg_.c
	sasfit_ff_longstructures__ln_onsager_rad_.c
	sasfit_ff_affine_shrinkage_deg.c
	sasfit_ff_affine_shrinkage_rad.c
	interface.c
)

# add plugin path for finding 'Find*.cmake' files for new <LIBS_EXT>
set(REL_CMAKE_MODULE_PATH .)

#                                                       #
### no adjustments required below this line (usually) ###
#                                                       #

project(${PRJ_NAME} C)

set(LIB_IS_PLUGIN true)

set(SRC_DIR ${${PRJ_NAME}_SOURCE_DIR})
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${SRC_DIR}/${MODULE_PATH}")

include(SasfitCmakeUtils)
sasfit_cmake_plugin()
