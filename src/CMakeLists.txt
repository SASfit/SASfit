cmake_minimum_required(VERSION 2.6.2)

if(CMAKE_HOST_APPLE)
        set(CMAKE_C_COMPILER /usr/bin/gcc)
        set(CMAKE_CXX_COMPILER /usr/bin/g++)
endif()

#
# get our current location and set the distribution directory name
#
project(sasfit C)

# we assume this script resides in sasfit/src
get_filename_component(CURRENT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
set(SASFIT_ROOT_DIR ${CURRENT_DIR}/..)
get_filename_component(SASFIT_ROOT_DIR "${SASFIT_ROOT_DIR}" REALPATH)
set(LIBRARY_OUTPUT_PATH ${CURRENT_DIR}/lib)

set(CMAKE_MODULE_PATH ${CURRENT_DIR}/cmake)
include(SasfitCmakeUtils)
include(SasfitSourceFiles)

message(STATUS "System Name: '${CMAKE_SYSTEM_NAME}' '${CMAKE_SYSTEM}'")
#message("## system apple: ${CMAKE_HOST_APPLE}")
message(STATUS "System Processor: '${CMAKE_SYSTEM_PROCESSOR}'")
#message("## exe extensi: '${CMAKE_EXECUTABLE_SUFFIX}'")
#message("## install Prefix: '${CMAKE_INSTALL_PREFIX}'")
#message("## generator: '${CMAKE_GENERATOR}'")

# updates the version number in some files
set(SASFIT_VERSION "0.94.11")
sasfit_update_version()
# this can be set on the command line too:
#cmake . -DCREATE_SOURCE=TRUE

# decides which packages to build afterwards
if(NOT DEFINED CREATE_SOURCE)
	set(CREATE_SOURCE FALSE)
endif(NOT DEFINED CREATE_SOURCE)
if(NOT DEFINED CREATE_BINARY)
	set(CREATE_BINARY TRUE)
endif(NOT DEFINED CREATE_BINARY)

 #############################################################
### determine platform specific properties                  ###
### get the static list of files (platform independent tcl) ###
 #############################################################

# original file containing all required sasfit filenames WITHOUT binaries build by cmake
set(SASFIT_BIN_LIST_FILE ${CURRENT_DIR}/cmake/create_bin_package.list)
set(SASFIT_SRC_LIST_FILE ${CURRENT_DIR}/cmake/create_source_package.list)
set(SASFIT_PCKG_SCRIPT ${CURRENT_DIR}/cmake/create_package.cmake)
set(SASFIT_STARPACK_SCRIPT ${CURRENT_DIR}/cmake/create_starpack.cmake)

# copy the binary package file list to a temporary list file 
# (binaries built will be added later)
set(BIN_FILE_LIST "")
file(STRINGS ${SASFIT_BIN_LIST_FILE} BIN_FILE_LIST)
file(STRINGS ${SASFIT_SRC_LIST_FILE} SASFIT_SRC_FILE_LIST)


# add runtime files required on Windows with Visual Studio only 
# TODO ::: (ATTENTION!, depends on Visual Studio Version, here 2005) ::: TODO
if(MSVC80)
	message(STATUS "\n   Assuming 'msvcr80.dll' and 'Microsoft.VC80.CRT.manifest' in\n   '${SASFIT_ROOT_DIR}'\n")
	sasfit_file_list(${BIN_FILE_LIST}
		msvcr80.dll
		Microsoft.VC80.CRT.manifest
	)
else(MSVC80)
	sasfit_file_list(${BIN_FILE_LIST})
endif(MSVC80)

if(CMAKE_HOST_APPLE)
	add_definitions(-DMACOSX)
	add_definitions(-DNO_LONGLONG_AND_NO_LONGINT)
	set(CMAKE_OSX_ARCHITECTURES ${CMAKE_SYSTEM_PROCESSOR})
	if(CMAKE_SYSTEM_PROCESSOR MATCHES "i[0-9]86")
		# assuming os is recent enough to understand x64
		# GSL build environment does not support multiple "-arch" 
		set(CMAKE_OSX_ARCHITECTURES x86_64)
	endif(CMAKE_SYSTEM_PROCESSOR MATCHES "i[0-9]86")
	message(STATUS "Bulding on MacOS for: ${CMAKE_OSX_ARCHITECTURES}")
endif(CMAKE_HOST_APPLE)


 #####################
### OPTIONS for f2c ###
 #####################

# platform specific properties
IF(WIN32) # for Windows

	ADD_DEFINITIONS(
		-DUSE_CLOCK	 # see etime_.c
		-DMSDOS
		-DNO_ONEXIT
		-DNO_FPINIT
		-DCONFLICTING_ISATTY
		-DNO_FUNCTION_MACRO
	)

	IF(MSVC80)
		# suppress warnings about usage of old non-limited string functions
		# has to be fixed later
		add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
		add_definitions(-DINTEGER_STAR_8) # see f2c/f2c.h
	ELSE(MSVC80) # NOT MSVC80
#			add_definitions(-DNO_LONGLONG_AND_NO_LONGINT)
	ENDIF(MSVC80)

ENDIF(WIN32) # for Windows

# for Unix/Linux
IF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
#	ADD_DEFINITIONS(-DINTEGER_STAR_8 -fopenmp) # see f2c/f2c.h
	ADD_DEFINITIONS(-DINTEGER_STAR_8 ) # see f2c/f2c.h
ENDIF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)


 ###############################
### OPTIONS for sasfit_common ###
 ###############################

add_definitions(
	-DSASFIT_LIB_SUFFIX="${CMAKE_SHARED_MODULE_SUFFIX}"
	-DSASFIT_LIB_PREFIX="${CMAKE_SHARED_MODULE_PREFIX}"
)


 ########################
### library definition ###
 ########################

# GSL
find_package(GSL QUIET REQUIRED)
message("gsl found; '${GSL_FOUND}'")
if(NOT GSL_FOUND)
        message("subdir gsl")
        add_subdirectory(gsl)
        find_package(GSL REQUIRED)
endif()
message(STATUS "GSL INCLUDES:  ${GSL_INCLUDE_DIRS}")
message(STATUS "GSL LIBRARIES: ${GSL_LIBRARIES}")

# TCL
find_package(TCL QUIET REQUIRED)
if(NOT TCL_FOUND)
        message("subdir tcl")
        add_subdirectory(tcl)
        find_package(TCL REQUIRED)
endif()
message(STATUS "TCL INCLUDES:  ${TCL_INCLUDE_DIRS}")
message(STATUS "TCL LIBRARIES: ${TCL_LIBRARIES}")
message(STATUS "TCL SH: ${TCL_SH}")

# FFTW
find_package(FFTW QUIET REQUIRED)
if(NOT FFTW_FOUND)
        message("subdir fftw")
        add_subdirectory(fftw)
        find_package(FFTW REQUIRED)
endif()
message(STATUS "FFTW INCLUDES:  ${FFTW_INCLUDE_DIRS}")
message(STATUS "FFTW LIBRARIES: ${FFTW_LIBRARIES}")

# sundials
find_package(sundials QUIET REQUIRED)
if(NOT sundials_FOUND)
        message("subdir sundials")
        add_subdirectory(sundials)
        find_package(sundials REQUIRED)
endif()
message(STATUS "sundials INCLUDES:  ${sundials_INCLUDE_DIRS}")
message(STATUS "sundials LIBRARIES: ${sundials_LIBRARIES}")

add_definitions(
	-DUSE_TCL_STUBS
	-DUSE_TK_STUBS
)

# openMP
#find_package(OpenMP QUIET)

#if(OPENMP_FOUND)
#        message(STATUS "Enabling OpenMP support")
#        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#        message(STATUS "OpenMP_C_FLAGS: '${OpenMP_C_FLAGS}'")
#        message(STATUS "OpenMP_CXX_FLAGS: '${OpenMP_CXX_FLAGS}'")
#        message(STATUS "OpenMP_EXE_LINKER_FLAGS: '${OpenMP_EXE_LINKER_FLAGS}'")
#        set(OpenMP_LIB)
#        if(WIN32)
#            # gather required libraries
#            execute_process(COMMAND ${CMAKE_C_COMPILER} -print-file-name=libgomp.a
#                            OUTPUT_VARIABLE OpenMP_LIB)
#            string(STRIP "${OpenMP_LIB}" OpenMP_LIB)
#            get_filename_component(OpenMP_LIB "${OpenMP_LIB}" REALPATH)
#            find_library(pthread_LIB pthread)
#            list(APPEND OpenMP_LIB "${pthread_LIB}")
#        endif()
#        message(STATUS "OpenMP LIBRARIES: '${OpenMP_LIB}'")
#else()
#        message(STATUS "Disabling OpenMP support")
#endif()

# get a list of all source files and prepare include dirs
set(SRC_FILES "")
set(SF_INC_DIRS "")
foreach(SF_MODULE f2c sasfit_common sasfit_sd sasfit_sq sasfit_peaks sasfit_ff sasfit_core sasfit_old sasfit_oz sasfit_frida)
	if(IS_DIRECTORY ${CURRENT_DIR}/${SF_MODULE})
		foreach(SRC_FILE ${SOURCE_${SF_MODULE}})
			list(APPEND SRC_FILES "${SF_MODULE}/${SRC_FILE}")
		endforeach(SRC_FILE)
		if(${SF_MODULE} MATCHES "f2c")
			list(APPEND SF_INC_DIRS "${SF_MODULE}")
		else(${SF_MODULE} MATCHES "f2c")
			list(APPEND SF_INC_DIRS "${SF_MODULE}/include")
		endif(${SF_MODULE} MATCHES "f2c")
	endif(IS_DIRECTORY ${CURRENT_DIR}/${SF_MODULE})
endforeach(SF_MODULE)

include_directories(
	${SF_INC_DIRS}
	${TCL_INCLUDE_DIRS}
	${GSL_INCLUDE_DIRS}
	${FFTW_INCLUDE_DIRS}
	${sundials_INCLUDE_DIRS}
)

add_library(sasfit SHARED
	${SRC_FILES}
)

target_link_libraries(sasfit
        ${TCL_LIBRARIES}
        ${GSL_STATIC_LIBRARIES}
        ${FFTW_LIBRARIES}
        ${sundials_LIBRARIES}
#        ${OpenMP_LIB}
)

# configure build flags, assumes gcc-style flags on all platforms
#sasfit_build_props_unix(sasfit "-save-temps")
set(COMPILE_FLAGS -std=gnu99)
if(UNIX)
	set(COMPILE_FLAGS "${COMPILE_FLAGS} -Wall")
endif(UNIX)
message("DEBUG: '${DEBUG}' OPTIM: '${OPTIM}'")
if(DEBUG)
	# debug
	set(COMPILE_FLAGS "${COMPILE_FLAGS} -g")
elseif(OPTIM)
	# non-debug optimized
	set(COMPILE_FLAGS "${COMPILE_FLAGS} -march=native -O2 -pipe -fomit-frame-pointer -fforce-addr")
else(DEBUG)
	# general purpose binary
	set(COMPILE_FLAGS "${COMPILE_FLAGS} -O2 -pipe -fomit-frame-pointer -fforce-addr")
endif(DEBUG)

set_target_properties(sasfit PROPERTIES
	COMPILE_FLAGS "${COMPILE_FLAGS}"
	MACOSX_RPATH FALSE # cmake 3 compatibility
)

sasfit_copy_shared_libs(sasfit "sasfit.vfs/lib/Sasfit")

if(DEFINED WITH_STATIC)

	set(SRC_FILES)
	foreach(SF_MODULE f2c sasfit_common)
		if(IS_DIRECTORY ${CURRENT_DIR}/${SF_MODULE})
			foreach(SRC_FILE ${SOURCE_${SF_MODULE}})
				list(APPEND SRC_FILES "${SF_MODULE}/${SRC_FILE}")
			endforeach(SRC_FILE)
		endif(IS_DIRECTORY ${CURRENT_DIR}/${SF_MODULE})
	endforeach(SF_MODULE)

	add_library(sasfit_common_stat STATIC
		${SRC_FILES}
	)

	if(UNIX)
		if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
			# required for static builds on 64bit
			set(COMPILE_FLAGS "${COMPILE_FLAGS} -fPIC")
		endif(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
	endif(UNIX)
	set_target_properties(sasfit_common_stat PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS}")
	set_target_properties(sasfit_common_stat PROPERTIES COMPILE_DEFINITIONS STATIC_ENABLED)

endif(DEFINED WITH_STATIC)

 #############
### Plugins ###
 #############

# build all the plugins
add_subdirectory(plugins)

message(STATUS "The binary package will include the following plugins: ")
foreach(PLUGIN_NAME ${SASFIT_PLUGINS_INCLUDED})
	message(STATUS "* ${PLUGIN_NAME}")
endforeach(PLUGIN_NAME)

 ####################################
### create a sasfit binary package ###
 ####################################

if(CREATE_BINARY)

	# selects libstdc++5 version of saskit
	set(LIBSTDC_VER "")
	if(DEFINED USE_STDC5)
		set(LIBSTDC_VER "_stdc++5")
	endif(DEFINED USE_STDC5)

	# selects debug version if desired
	set(DEBUG_SUFFIX "")
	if(DEBUG)
		message(STATUS "Using a starpack with debug symbols.")
		set(DEBUG_SUFFIX "_debug")
	endif(DEBUG)
#	set(SASFIT_VERSION "${SASFIT_VERSION}${DEBUG_SUFFIX}")

	set(SASFIT_PCKG_DIR_POSTFIX ${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR})
	if(CMAKE_HOST_APPLE)
		set(SASFIT_PCKG_DIR_POSTFIX ${CMAKE_SYSTEM_NAME}_${CMAKE_OSX_ARCHITECTURES})
	endif(CMAKE_HOST_APPLE)
        string(TOLOWER "${SASFIT_PCKG_DIR_POSTFIX}" SASFIT_PCKG_DIR_POSTFIX)
	set(SASFIT_PCKG_DIR_POSTFIX ${SASFIT_PCKG_DIR_POSTFIX}${LIBSTDC_VER}${DEBUG_SUFFIX})
	set(SASFIT_EXEC_FILENAME sasfit${CMAKE_EXECUTABLE_SUFFIX})

	sasfit_file_list(${SASFIT_BIN_FILE_LIST} ${SASFIT_EXEC_FILENAME})

	# set the saskit filename, chose the debug version if required
	set(SASKIT_FILENAME "saskit_${SASFIT_PCKG_DIR_POSTFIX}${CMAKE_EXECUTABLE_SUFFIX}")
    build_saskit("${SASFIT_ROOT_DIR}" "${SASKIT_FILENAME}")
    message(STATUS "Using saskit file: '${SASKIT_FILENAME}'")
    get_saskit_dependencies("${SASFIT_ROOT_DIR}" "${SASKIT_FILENAME}")
	# write the list of filenames to a temporary file
    # (workaround for shell command length restrictions)
	set(SASFIT_LIST_FILE ${SASFIT_BIN_LIST_FILE}.tmp)
	file(WRITE ${SASFIT_LIST_FILE} "${SASFIT_BIN_FILE_LIST}")

	add_custom_target(sasfit_binary ALL
		${CMAKE_COMMAND} -D SASFIT_ROOT_DIR:PATH=${SASFIT_ROOT_DIR} 
				-D SASKIT_FILENAME:STRING=${SASKIT_FILENAME}
				-D SASFIT_EXEC_FILENAME:STRING=${SASFIT_EXEC_FILENAME}
				-D SASFIT_DEBUG:BOOL=${DEBUG}
				-P ${SASFIT_STARPACK_SCRIPT}
		DEPENDS sasfit ${SASFIT_PLUGINS_INCLUDED}
        COMMENT "Creating sasfit binary ..."
		VERBATIM
	)
	add_custom_target(sasfit_binary_package ALL
		${CMAKE_COMMAND} -D SASFIT_ROOT_DIR:PATH=${SASFIT_ROOT_DIR} 
				-D SASFIT_PCKG_DIR_POSTFIX:STRING=${SASFIT_PCKG_DIR_POSTFIX}
				-D SASFIT_LIST_FILE:FILEPATH=${SASFIT_LIST_FILE}
				-D CM_GEN:STRING=${CMAKE_GENERATOR}
				-D SASFIT_VERSION:STRING=${SASFIT_VERSION}
				-P ${SASFIT_PCKG_SCRIPT}
		DEPENDS sasfit_binary
        COMMENT "Creating sasfit package ..."
		VERBATIM
	)

endif(CREATE_BINARY)

if(CREATE_SOURCE)

	 ####################################
	### create a sasfit source package ###
	 ####################################

	# write the list of filenames to a temporary file
    # (workaround for shell command length restrictions)
	set(SASFIT_LIST_FILE ${SASFIT_SRC_LIST_FILE}.tmp)
	file(WRITE ${SASFIT_LIST_FILE} "${SASFIT_SRC_FILE_LIST}")

	# copies all files required for a valid source distribution into a separate directory
	add_custom_target(sasfit_source ALL
		${CMAKE_COMMAND} -D SASFIT_ROOT_DIR:PATH=${SASFIT_ROOT_DIR} 
				-D SASFIT_PCKG_DIR_POSTFIX:STRING=source
				-D SASFIT_LIST_FILE:FILEPATH=${SASFIT_LIST_FILE}
				-D CM_GEN:STRING=${CMAKE_GENERATOR}
				-D SASFIT_VERSION:STRING=${SASFIT_VERSION}
				-P ${SASFIT_PCKG_SCRIPT}
        COMMENT "Copying sasfit source files to a separate directory ..."
		VERBATIM
	)
endif(CREATE_SOURCE)

# vim: set ts=4 sw=4 sts=4 tw=0:
