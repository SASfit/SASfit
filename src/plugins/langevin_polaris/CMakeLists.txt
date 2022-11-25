cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_langevin_polaris)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	sasfit_ff_langevin.c
	sasfit_ff_langevin_static.c
	sasfit_ff_langevin_tau.c
	sasfit_ff_tisane.c
	sasfit_ff_tisane_static.c
	sasfit_ff_tisane_tau.c
	sasfit_ff_langevin_up_up.c
	sasfit_ff_langevin_down_down.c
	sasfit_ff_langevin_up_down.c
	sasfit_ff_langevin_down_up.c
	sasfit_ff_langevin_up.c
	sasfit_ff_langevin_down.c
	sasfit_ff_langevin_static_up_up.c
	sasfit_ff_langevin_static_down_down.c
	sasfit_ff_langevin_static_up_down.c
	sasfit_ff_langevin_static_down_up.c
	sasfit_ff_langevin_static_up.c
	sasfit_ff_langevin_static_down.c
	interface.c
	polaris_utils.c
)

# path to 'Find*.cmake' files in sasfit package, 
# relative to the location of this file
set(REL_CMAKE_MODULE_PATH ../../cmake)

#                                                       #
### no adjustments required below this line (usually) ###
#                                                       #

project(${PRJ_NAME} C)

set(LIB_IS_PLUGIN true)

set(SRC_DIR ${${PRJ_NAME}_SOURCE_DIR})
set(CMAKE_MODULE_PATH ${SRC_DIR}/${REL_CMAKE_MODULE_PATH})

include(SasfitCmakeUtils)
sasfit_cmake_plugin()
