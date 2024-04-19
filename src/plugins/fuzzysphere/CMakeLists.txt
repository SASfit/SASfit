# cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_fuzzysphere)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	sasfit_ff_fuzzysphere.c
	sasfit_ff_radial_profile_of_fuzzysphere.c
	sasfit_ff_coreshellmicrogel.c
	sasfit_ff_radial_profile_of_coreshellmicrogel.c
	sasfit_ff_ExpShell.c
	sasfit_ff_radial_profile_of_ExpShell.c
	sasfit_ff_LinShell.c
	sasfit_ff_radial_profile_of_LinShell.c
	sasfit_ff_LinShell2.c
	sasfit_ff_radial_profile_of_LinShell2.c
	interface.c
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
