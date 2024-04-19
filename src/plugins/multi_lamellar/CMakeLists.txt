# cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_multi_lamellar)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	sasfit_sq_modifiedcaille.c
	sasfit_sq_thermaldisorder.c
	sasfit_sq_paracrystalline.c
	sasfit_sq_perfect_ordered.c
	sasfit_sq_modifiedcaille_sd.c
	sasfit_sq_thermaldisorder_sd.c
	sasfit_sq_paracrystalline_sd.c
	sasfit_sq_perfect_ordered_sd.c
	sasfit_sq_modifiedcaille_sum.c
	sasfit_sq_thermaldisorder_sum.c
	sasfit_sq_paracrystalline_sum.c
	sasfit_sq_perfect_ordered_sum.c
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
