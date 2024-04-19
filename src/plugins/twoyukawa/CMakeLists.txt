# cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_twoyukawa)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	sasfit_sq_two_yukawa.c
	sasfit_sq_two_yukawa_potential.c
	sasfit_sq_two_yukawa_gr.c
	sasfit_sq_one_yukawa.c
	interface.c
	2Y_cpoly.c
	2Y_OneYukawa.c
	2Y_PairCorrelation.c
	2Y_TwoYukawa.c
	2Y_utility.c
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
