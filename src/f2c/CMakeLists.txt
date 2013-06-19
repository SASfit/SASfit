cmake_minimum_required(VERSION 2.6.2)

# the project name
set(PRJ_NAME f2c)

# required external libs which are installed on the system
set(LIBS_EXT
)

# required internal (sasfit-) libs
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


# platform specific properties
IF(WIN32) # for Windows

    ADD_DEFINITIONS(
    	-DUSE_CLOCK	 # see etime_.c
	-DMSDOS
	-DNO_ONEXIT
	-DNO_FPINIT
	-DCONFLICTING_ISATTY
	-DNO_FUNCTION_MACRO
        -D_CRT_SECURE_NO_DEPRECATE
    )
    
    IF(MSVC80)
        ADD_DEFINITIONS(-DINTEGER_STAR_8) # see f2c/f2c.h
    ELSE(MSVC80) # NOT MSVC80
        ADD_DEFINITIONS(-DNO_LONGLONG_AND_NO_LONGINT)
    ENDIF(MSVC80)
    
ENDIF(WIN32) # for Windows

# for Unix/Linux
IF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	ADD_DEFINITIONS(-DINTEGER_STAR_8) # see f2c/f2c.h
ENDIF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)

