cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_non_particulate)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	sasfit_ff_generalized_guinier_porod_law.c
	sasfit_ff_extended_guinier_law.c
	sasfit_ff_beaucage_exppowlaw_2.c
	sasfit_ff_beaucage_exppowlaw.c
	sasfit_ff_spinodal.c
	sasfit_ff_spinodal2.c
	sasfit_ff_debye_anderson_brumberger.c
	sasfit_ff_g_dab.c
	sasfit_ff_def_g_dab.c
	sasfit_ff_ornstein_zernike.c
	sasfit_ff_broad_peak.c
	sasfit_ff_teubner_strey.c
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
