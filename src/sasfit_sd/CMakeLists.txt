cmake_minimum_required(VERSION 2.6.2)

# the project name
set(PRJ_NAME sasfit_sd)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# required internal (sasfit_) libs
set(LIBS_INT
	sasfit_common
)

# for source file listings, see SasfitSourceFiles.cmake

# type of library to build: SHARED or STATIC
#set(LIB_TYPE SHARED)

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

