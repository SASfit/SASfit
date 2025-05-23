# cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_pcs_fuzzy_cyl)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	sasfit_ff_boucher_cyl.c
	sasfit_ff_profile_boucher_cyl.c
	sasfit_ff_boucher2_cyl.c
	sasfit_ff_profile_boucher2_cyl.c
	sasfit_ff_fuzzy_cyl.c
	sasfit_ff_profile_fuzzy_cyl.c
	sasfit_ff_linsh_cyl.c
	sasfit_ff_profile_linsh_cyl.c
	sasfit_ff_rc_exp_sh_cyl.c
	sasfit_ff_rc_exp_sh_cyl_profile.c
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
