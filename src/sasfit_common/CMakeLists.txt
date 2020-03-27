cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_common)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
	FFTW
)

# required internal (sasfit_) libs
set(LIBS_INT
)

# for source file listings, see SasfitSourceFiles.cmake

# path to 'Find*.cmake' files in sasfit package,
# relative to the location of this file
set(REL_CMAKE_MODULE_PATH ../cmake)

set(LIB_IS_PLUGIN false)

#                                                        #
### not adjustments required below this line (usually) ###
#                                                        #

project(${PRJ_NAME})

set(SRC_DIR ${${PRJ_NAME}_SOURCE_DIR})
set(CMAKE_MODULE_PATH ${SRC_DIR}/${REL_CMAKE_MODULE_PATH})

include(SasfitCmakeUtils)
sasfit_cmake_library()


# determine platform specific properties
add_definitions(
		-DSASFIT_LIB_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}"
		-DSASFIT_LIB_PREFIX="${CMAKE_SHARED_LIBRARY_PREFIX}"
)

