# cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_hyperbranched_polymer)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	sasfit_ff_fract_hyperbranched_polym.c
	sasfit_ff_f_funct_homo_polym.c
	sasfit_ff_comb_regular.c
	sasfit_ff_comb_random.c
	sasfit_ff_comb_hetero.c
	sasfit_ff_ideal_dendrimer.c
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
