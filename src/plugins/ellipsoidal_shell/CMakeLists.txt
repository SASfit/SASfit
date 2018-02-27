cmake_minimum_required(VERSION 2.6)

# the project name
set(PRJ_NAME sasfit_ellipsoidal_shell)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	sasfit_ff_spheroid_R.c
	sasfit_ff_spheroid_V.c
	sasfit_ff_ellipsoidal_shell_0.c
	sasfit_ff_ellipsoidal_shell_0t.c
	sasfit_ff_ellipsoidal_shell_1.c
	sasfit_ff_ellipsoidal_shell_1t.c
	sasfit_ff_ellipsoidal_shell_2.c
	sasfit_ff_ellipsoidal_shell_2t.c
	sasfit_ff_ellipsoidal_shell_utils.c
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
