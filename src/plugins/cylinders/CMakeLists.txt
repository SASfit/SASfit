cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_cylinders)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	sasfit_ff_rod.c
	sasfit_ff_disc.c
	sasfit_ff_porodcylinder.c
	sasfit_ff_longcylinder.c
	sasfit_ff_flatcylinder.c
	sasfit_ff_cylinder.c
	sasfit_ff_longcylshell.c
	sasfit_ff_ellcylshell1.c
	sasfit_ff_ellcylshell2.c
	sasfit_ff_cylshell1.c
	sasfit_ff_cylshell2.c
	sasfit_ff_alignedcylshell.c
	sasfit_ff_torus.c
	sasfit_cyl_utils.c
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
