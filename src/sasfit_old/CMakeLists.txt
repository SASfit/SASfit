cmake_minimum_required(VERSION 2.6.2)

# the project name
set(PRJ_NAME sasfit_old)

# required external libs which are installed on the system
set(LIBS_EXT
	TCL
	GSL
)

# required internal (sasfit_) libs
set(LIBS_INT
	f2c
	sasfit_common
	sasfit_sd
	sasfit_sq
	sasfit_ff
	sasfit_core
)

# for source file listings, see SasfitSourceFiles.cmake

# type of library to build: SHARED or STATIC
#set(LIB_TYPE SHARED)

# path to 'Find*.cmake' files in sasfit package,
# relative to the location of this file
set(REL_CMAKE_MODULE_PATH ..)

set(LIB_IS_PLUGIN false)

#                                                        #
### not adjustments required below this line (usually) ###
#                                                        #

project(${PRJ_NAME})

set(SRC_DIR ${${PRJ_NAME}_SOURCE_DIR})
set(CMAKE_MODULE_PATH ${SRC_DIR}/${REL_CMAKE_MODULE_PATH})

# MS VC6 doesnt support the __FUNCTION__ Macro
if(MSVC60)
	add_definitions(-DNO_FUNCTION_MACRO)
endif(MSVC60)

# suppress warnings about usage of old non-limited string functions
if(WIN32)
	add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
endif(WIN32)

include(SasfitCmakeUtils)
sasfit_cmake_library()

