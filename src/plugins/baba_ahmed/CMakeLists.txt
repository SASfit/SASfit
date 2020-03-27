cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_baba_ahmed)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	sasfit_sq_baba_ahmed_1.c
	sasfit_sq_baba_ahmed_2.c
	sasfit_sq_baba_ahmed_syl.c
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

