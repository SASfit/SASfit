# cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_two_dim_fluid)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	sasfit_sq__d_hard_disks__rosenfeld_aligned.c
	sasfit_sq__d_hard_disks__rosenfeld_random.c
	sasfit_sq__d_hard_disks__guo_aligned.c
	sasfit_sq__d_hard_disks__guo_random.c
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
