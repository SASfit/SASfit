cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_spherical_ellipsoidal_micelles)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	sasfit_ff_sphere_chains_rw__nagg.c
	sasfit_ff_sphere_chains_rw_.c
	sasfit_ff_sphere_chains_rw__rc.c
	sasfit_ff_sphere_chains_saw__nagg.c
	sasfit_ff_sphere_chains_saw_.c
	sasfit_ff_sphere_chains_saw__rc.c
	sasfit_ff_sphere_r__a_nagg.c
	sasfit_ff_sphere_r__a.c
	sasfit_ff_sphere_r__a_rc.c
	sasfit_ff_sphere_smooth_interface_r__a_nagg.c
	sasfit_ff_sphere_2nd_r__a_rc.c
	sasfit_ff_ell_chains_rw__nagg.c
	sasfit_ff_ell_chains_rw_.c
	sasfit_ff_ell_chains_rw__rc.c
	interface.c
	sph_ell_mic_utils.c
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
