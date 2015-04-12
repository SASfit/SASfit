# src/cmake/SasfitCmakeUtils.cmake
#
# Copyright (c) 2008-2011, Paul Scherrer Institute (PSI)
#
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the <organization> nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Author(s) of this file:
#   Ingo Bressler (ingo.bressler@bam.de)
#
#############################################################################
# A Cmake module to provide utility functions often used 
# in sasfit cmake scripts
#
# This module expects to live in <sasfit-dir>/src

cmake_minimum_required(VERSION 2.6.2)
if(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} VERSION_GREATER 2.6.2)
	cmake_policy(SET CMP0011 OLD)
endif(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} VERSION_GREATER 2.6.2)
cmake_policy(SET CMP0012 NEW)

# adds given files to binary distribution file list
# needs to be a function, because we operate on a cmake cache variable
function(sasfit_file_list)
	set(SASFIT_BIN_FILE_LIST ${ARGN}
		CACHE STRING "files part of a sasfit binary package" FORCE
	)
endfunction(sasfit_file_list)

# ensure that the dir doesn't exist before creating it
macro(sasfit_assert_dir)
	if(${ARGC} GREATER 0)
		if(NOT EXISTS ${ARGV0})
			file(MAKE_DIRECTORY ${ARGV0})
		endif(NOT EXISTS ${ARGV0})
	endif(${ARGC} GREATER 0)
endmacro(sasfit_assert_dir)

# copy shared libs to a target dir (where sasfit tcl routines will find them)
macro(sasfit_copy_shared_libs SHARED_TARGET REL_TARGET_DIR)
	sasfit_copy_libs(${REL_TARGET_DIR} "${CMAKE_SHARED_LIBRARY_PREFIX}" ${SHARED_TARGET} "${CMAKE_SHARED_LIBRARY_SUFFIX}")
endmacro(sasfit_copy_shared_libs)

macro(sasfit_copy_static_libs STATIC_TARGET REL_TARGET_DIR)
	sasfit_copy_libs(${REL_TARGET_DIR} "${CMAKE_STATIC_LIBRARY_PREFIX}" ${STATIC_TARGET} "${CMAKE_STATIC_LIBRARY_SUFFIX}")
endmacro(sasfit_copy_static_libs)

# copy plugin (shared lib) to a target dir (where sasfit tcl routines will find them)
macro(sasfit_copy_plugins SHARED_TARGET SUPP_FILE_PATH SUPP_FILE)
	sasfit_copy_libs(plugins "${CMAKE_SHARED_MODULE_PREFIX}" ${SHARED_TARGET} "${CMAKE_SHARED_MODULE_SUFFIX}" ${SUPP_FILE_PATH} ${SUPP_FILE})
endmacro(sasfit_copy_plugins)

macro(replace_str_in_file FILENAME PATTERN_STR REPLACE_STR)
	# let the documentation know about the svn revision number
	file(READ ${FILENAME} FILE_BODY)
	string(REGEX REPLACE "${PATTERN_STR}" "${REPLACE_STR}"
		FILE_BODY_NEW ${FILE_BODY})
	file(WRITE ${FILENAME} ${FILE_BODY_NEW})
endmacro(replace_str_in_file)

macro(get_rev_from_docs FILENAME)
	file(READ ${FILENAME} FILE_BODY)
	string(LENGTH ${FILE_BODY} LEN)
	set(PATTERN "PROJECT_NUMBER         = [a-z]?[0-9]+\\.?[0-9]*\\.?([a-z]|[0-9])*\\.?([a-z]|[0-9])*")
	string(REGEX MATCH "${PATTERN}" LINE "${FILE_BODY}")
	string(REGEX MATCH "[0-9]+\\.?[0-9]*\\.?([a-z]|[0-9])*\\.?([a-z]|[0-9])*" REV_NR "${LINE}")
endmacro(get_rev_from_docs)

# determine sasfit-root directory
if(NOT DEFINED SASFIT_ROOT_DIR)
	GET_FILENAME_COMPONENT(SASFIT_ROOT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
	GET_FILENAME_COMPONENT(SASFIT_ROOT_DIR "${SASFIT_ROOT_DIR}/.." ABSOLUTE)
endif(NOT DEFINED SASFIT_ROOT_DIR)

# copy shared libs to a target dir (where sasfit tcl routines will find them)
# SHARED_TARGET: name of the cmake target whose libraries should be copied
# REL_TARGET_DIR: path of target directory, relative to sasfit-root dir
# ARG2: absolute path to supplementary file to copy [optional]
# ARG3: filename of supplementary file to copy [optional]
macro(sasfit_copy_libs REL_TARGET_DIR PREFIX SHARED_TARGET SUFFIX)
	set(TARGET_DIR ${SASFIT_ROOT_DIR}/${REL_TARGET_DIR})
	sasfit_assert_dir(${TARGET_DIR})
	set(SHARED_LIB_NAME ${PREFIX}${SHARED_TARGET}${SUFFIX})
	set(SHARED_LIB_FULL_NAME ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${SHARED_LIB_NAME})

	# add additional copy command if a 2nd file was provided
	set(SUPP_CMD "")
	if(${ARGC} GREATER 5)
		set(SUPP_CMD COMMAND ${CMAKE_COMMAND} ARGS -E copy "${ARGV4}/${ARGV5}" "${TARGET_DIR}")
		sasfit_file_list(${SASFIT_BIN_FILE_LIST}
			${REL_TARGET_DIR}/${ARGV5}
		)
	endif(${ARGC} GREATER 5)
	set(CMD COMMAND ${CMAKE_COMMAND} ARGS -E copy "${SHARED_LIB_FULL_NAME}" "${TARGET_DIR}/${SHARED_TARGET}${SUFFIX}")
	add_custom_command(TARGET ${SHARED_TARGET} POST_BUILD
			${CMD}
			${SUPP_CMD}
			VERBATIM
	)

	if(NOT ${SHARED_TARGET} STREQUAL "sasfit")
		# add this binary to the list of files to copy for a working package
		sasfit_file_list(${SASFIT_BIN_FILE_LIST}
			${REL_TARGET_DIR}/${SHARED_TARGET}${SUFFIX}
		)
	endif(NOT ${SHARED_TARGET} STREQUAL "sasfit")
endmacro(sasfit_copy_libs)

macro(sasfit_cmake_plugin_static)

	set(PRJ_NAME_STAT ${PRJ_NAME}_stat)
	find_package(sasfit REQUIRED)
	include_directories(
		${sasfit_INCLUDE_DIRS}
		${SASFIT_ROOT_DIR}/plugins
	)

	add_library(${PRJ_NAME_STAT} STATIC ${PRJ_SOURCE})

	set(COMPILE_FLAGS)
	if(UNIX)
		set(COMPILE_FLAGS "-Wall")
		if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
			# required for static builds on 64bit
			set(COMPILE_FLAGS "${COMPILE_FLAGS} -fPIC")
		endif(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
		if(DEFINED SASFIT_DEBUG)
			set(COMPILE_FLAGS "${COMPILE_FLAGS} -g")
		endif(DEFINED SASFIT_DEBUG)
	endif(UNIX)
	set_target_properties(${PRJ_NAME_STAT} PROPERTIES COMPILE_DEFINITIONS STATIC_ENABLED)
	set_target_properties(${PRJ_NAME_STAT} PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS}")

	# set library search paths for internal shared libraries
	# build with the whole sasfit package controlled by a toplevel CMakeLists
#	sasfit_copy_plugins(${PRJ_NAME} "${SRC_DIR}/include" "${PRJ_NAME}.h")

endmacro(sasfit_cmake_plugin_static)

macro(sasfit_cmake_plugin)

	include(SasfitSourceFiles)

	set(PRJ_SOURCE ${SOURCE_${PRJ_NAME}})
	set(LIBRARY_OUTPUT_PATH ${SRC_DIR}/lib)
    if(NOT EXISTS ${LIBRARY_OUTPUT_PATH})
        # if the output dir does not exist the linker throws errors difficult to
        # interpret: "ar" reporting on missing lib which it's supposed to build
        file(MAKE_DIRECTORY "${LIBRARY_OUTPUT_PATH}")
    endif()

	add_library(${PRJ_NAME} MODULE ${PRJ_SOURCE})

	find_package(sasfit_common REQUIRED)
	find_package(f2c REQUIRED)
	include_directories(
		${sasfit_common_INCLUDE_DIRS}
		${f2c_INCLUDE_DIRS}
	)
	foreach(LIB_EXT ${LIBS_EXT})
		find_package(${LIB_EXT} REQUIRED)
		if(${LIB_EXT}_INCLUDE_DIR)
			include_directories(${${LIB_EXT}_INCLUDE_DIR})
		endif(${LIB_EXT}_INCLUDE_DIR)
		if(${LIB_EXT}_INCLUDE_DIRS)
			include_directories(${${LIB_EXT}_INCLUDE_DIRS})
		endif(${LIB_EXT}_INCLUDE_DIRS)
		# add library to link list
		if(${LIB_EXT}_STATIC_LIBRARIES)
			target_link_libraries(${PRJ_NAME} ${${LIB_EXT}_STATIC_LIBRARIES})
		else(${LIB_EXT}_STATIC_LIBRARIES)
			target_link_libraries(${PRJ_NAME} ${${LIB_EXT}_LIBRARIES})
		endif(${LIB_EXT}_STATIC_LIBRARIES)
	endforeach(LIB_EXT)

	# set some compiler switches
	set_target_properties(${PRJ_NAME} PROPERTIES COMPILE_DEFINITIONS MAKE_SASFIT_PLUGIN)
	set(COMPILE_FLAGS)
	if(UNIX)
		set(COMPILE_FLAGS "-Wall")
		if(DEFINED SASFIT_DEBUG)
			set(COMPILE_FLAGS "${COMPILE_FLAGS} -g")
		endif(DEFINED SASFIT_DEBUG)
	endif(UNIX)
	set_target_properties(${PRJ_NAME} PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS}")
#	sasfit_build_props_unix(${PRJ_NAME} -save-temps)

	# set library search paths for internal shared libraries
	# build with the whole sasfit package controlled by a toplevel CMakeLists
	sasfit_copy_plugins(${PRJ_NAME} "${SRC_DIR}/include" "${PRJ_NAME}.h")

	if(DEFINED WITH_STATIC)
		sasfit_cmake_plugin_static()
	endif(DEFINED WITH_STATIC)

endmacro(sasfit_cmake_plugin)


macro(sasfit_update_version)

	set(DETERMINED_FROM_DOCS FALSE)
	if(NOT DEFINED SASFIT_VERSION)
                set(SASFIT_VERSION "custom")
		find_package(Mercurial)
                if(MERCURIAL_FOUND AND IS_DIRECTORY "${SASFIT_ROOT_DIR}/.hg")
			mercurial_hg_info(${SASFIT_ROOT_DIR} sasfit)
			set(SASFIT_VERSION 
				"${sasfit_HG_DATE}-${sasfit_HG_BRANCH}-${sasfit_HG_CHANGESET}")
			message(STATUS "Current source version is '${SASFIT_VERSION}'")
                endif(MERCURIAL_FOUND AND IS_DIRECTORY "${SASFIT_ROOT_DIR}/.hg")

# old SVN
#		# try to get the revision number of the working copy (current dir)
#		# for a correct rev number it is required to update again after commit
#		set(SASFIT_SVN_DIRS
#			${SASFIT_ROOT_DIR}/.svn
#			${SASFIT_ROOT_DIR}/src/.svn
#		)
#		set(REV_NR 0)
#		foreach(DIRNAME ${SASFIT_SVN_DIRS})
#			if(EXISTS ${DIRNAME}/entries)
#				file(STRINGS ${DIRNAME}/entries SASFIT_SVN LIMIT_COUNT 4)
#				foreach(str ${SASFIT_SVN}) ## how to chose the last string of a string-'array' ?
#					if(${str} GREATER ${REV_NR})
#						set(REV_NR ${str}) # set to highest revision if available
#					endif(${str} GREATER ${REV_NR})
#				endforeach(str)
#			endif(EXISTS ${DIRNAME}/entries)
#		endforeach(DIRNAME)
#		if(${REV_NR} EQUAL 0)
#			# get the version number from the documentation if
#			# everything else fails
#			get_rev_from_docs(${SASFIT_ROOT_DIR}/src/Doxyfile)
#			set(DETERMINED_FROM_DOCS TRUE)
#		endif(${REV_NR} EQUAL 0)
#		set(SASFIT_VERSION "r${REV_NR}")
	endif(NOT DEFINED SASFIT_VERSION)

	# let the tcl code know about the svn revision number
	file(WRITE ${SASFIT_ROOT_DIR}/sasfit.vfs/lib/app-sasfit/tcl/sasfit_svn_rev.tcl
		"set sasfit(svn_rev) ${SASFIT_VERSION}"
	)
	# let the documentation know about the svn revision number
	replace_str_in_file(${SASFIT_ROOT_DIR}/src/Doxyfile 
		"PROJECT_NUMBER         = ([^\n]*)"
		"PROJECT_NUMBER         = ${SASFIT_VERSION}"
	)

	# set sasfit revision number
	if(DEFINED REV_NR)
		if(${DETERMINED_FROM_DOCS})
			set(SASFIT_VERSION "devel_custom")
		else(${DETERMINED_FROM_DOCS})
			set(SASFIT_VERSION "devel_r${REV_NR}")
		endif(${DETERMINED_FROM_DOCS})
	endif(DEFINED REV_NR)

endmacro(sasfit_update_version)

# check for target architecture
set(SYSTEM_IS_64 FALSE)
if(CMAKE_SIZEOF_VOID_P)
    if(${CMAKE_SIZEOF_VOID_P} EQUAL 8) # check for 64bit platform
        set(SYSTEM_IS_64 TRUE)
    endif()
endif()

set(PLATFORM "${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}")
string(TOLOWER ${PLATFORM} PLATFORM)

# retrieves the path to the already extracted source package
# sets result variables in parent scope:
# SOURCE_DIR: absolute path to source package directory
# BASE_NAME:  name of the current dir,
#             usually the base name of the package to build
function(get_package_dir CURRENT_DIR)
    if(NOT EXISTS ${CURRENT_DIR} OR NOT DEFINED PLATFORM)
        return()
    endif()
    message(STATUS "get_package_dir: '${CURRENT_DIR}'")
    get_filename_component(BASE_NAME ${CURRENT_DIR} NAME)
    set(BASE_NAME ${BASE_NAME} PARENT_SCOPE)
    message(STATUS "package name: '${BASE_NAME}'")
    unset(SOURCE_DIR)
    find_configure(${CURRENT_DIR}/${PLATFORM})
    #message("SOURCE_DIR: '${SOURCE_DIR}' '${SUFFIX_DIR}' '${PLATFORM}'")
    message(STATUS "found source dir: '${SOURCE_DIR}'")
    set(SOURCE_DIR ${SOURCE_DIR} PARENT_SCOPE)
endfunction()

function(find_configure PCKG_PATH)
    message(STATUS "find_configure '${PCKG_PATH}'")
    # determine directory for PCKG_PATH containing *
    file(GLOB GLOB_RESULT "${PCKG_PATH}")
    foreach(RESULT ${GLOB_RESULT})
        if(EXISTS "${RESULT}" AND IS_DIRECTORY "${RESULT}")
            set(PCKG_PATH ${RESULT})
            break()
        endif()
    endforeach()
    # unset suffix result
    unset(SUFFIX_DIR)
    set(SUFFIX_DIR PARENT_SCOPE)
    # list of sub directories containing the configure script eventually
    set(candidates .)
    if(WIN32)
        list(APPEND candidates win)
    elseif(UNIX)
        list(APPEND candidates unix)
    endif()
    foreach(candidate ${candidates})
        # try sth for the TCL case
        #message("GLOB: '${PCKG_PATH}/${candidate}/configure'")
        file(GLOB CONFIG_FILE "${PCKG_PATH}/${candidate}/configure")
        #message("CONFIG_FILE1 '${CONFIG_FILE}'")
        if(CONFIG_FILE)
            get_filename_component(CONFIG_FILE ${CONFIG_FILE} PATH ABSOLUTE)
            #message("CONFIG_FILE2 '${CONFIG_FILE}'")
            if(EXISTS "${CONFIG_FILE}")
                set(SUFFIX_DIR ${candidate} PARENT_SCOPE)
                get_filename_component(SOURCE_DIR ${CONFIG_FILE} PATH ABSOLUTE)
                set(SOURCE_DIR ${SOURCE_DIR} PARENT_SCOPE)
                break()
            endif()
        endif()
    endforeach()
endfunction()

# looks for existing source dirs and extracts the package
function(build_from_source CURRENT_DIR CONFIGURE_OPTIONS)
    if(NOT EXISTS ${CURRENT_DIR} OR NOT DEFINED PLATFORM)
        return()
    endif()
    
    # look for existing source dir
    get_package_dir(${CURRENT_DIR})

    # remove any existing source tree first
    if(IS_DIRECTORY ${SOURCE_DIR})
        message(STATUS "Removing existing source tree first: '${SOURCE_DIR}'")
        file(REMOVE_RECURSE ${SOURCE_DIR})
    endif()

    # extract source tree, assuming *.tar.gz archive file
    file(GLOB PCKG_FILE ${CURRENT_DIR}/${BASE_NAME}*.tar.gz)
    if(NOT EXISTS ${PCKG_FILE})
        return() # break up if no package was found, nothing to do
    endif()
    get_filename_component(PCKG_FILE ${PCKG_FILE} NAME)
    message(STATUS "Extracting package: '${PCKG_FILE}'")

    # actual tar command for extraction
    execute_process(COMMAND tar zxvf ${PCKG_FILE}
                    WORKING_DIRECTORY ${CURRENT_DIR})
    # rename source dir to platform name
    find_configure(${CURRENT_DIR}/${BASE_NAME}*)
    if(NOT EXISTS "${SOURCE_DIR}")
        message(STATUS "${BASE_NAME} source dir not found! Giving up.")
        return()
    endif()
    #message("SOURCE_DIR: '${SOURCE_DIR}' '${SUFFIX_DIR}' '${PLATFORM}'")
    set(TARGET ${CURRENT_DIR}/${PLATFORM})
    message(STATUS "renaming '${SOURCE_DIR} ${TARGET}'")
    if(EXISTS ${TARGET})
        file(REMOVE_RECURSE ${TARGET})
    endif()
    file(RENAME ${SOURCE_DIR} ${TARGET})

    # look again for the source directory, should be ready now
    get_package_dir(${CURRENT_DIR})
    # set general package install location
    set(CONFIGURE_OPTIONS
        --prefix=${SOURCE_DIR}
    )

    # get configure options from var args of this function
    foreach(ARGIDX RANGE 1 ${ARGC})
        list(APPEND CONFIGURE_OPTIONS ${ARGV${ARGIDX}})
    endforeach()

    set(WORK_DIR ${SOURCE_DIR}/${SUFFIX_DIR})
    # run configure script
    message(STATUS "Running ${BASE_NAME} configure with options: '${CONFIGURE_OPTIONS}'")
    execute_process(COMMAND sh configure ${CONFIGURE_OPTIONS}
                    WORKING_DIRECTORY ${WORK_DIR})

    # run make, i.e. build the library and install it in this local path
    message(STATUS "Building ${BASE_NAME} ...")
    execute_process(COMMAND make all
                    WORKING_DIRECTORY ${WORK_DIR})
    execute_process(COMMAND make install
                    WORKING_DIRECTORY ${WORK_DIR})

endfunction()

function(build_saskit SASFIT_ROOT_DIR SASKIT_FILENAME)
    message(STATUS "build_saskit '${SASFIT_ROOT_DIR}' '${SASKIT_FILENAME}'")
    set(SASKIT_PATH ${SASFIT_ROOT_DIR}/saskit)
    set(SASKIT_FILE ${SASKIT_PATH}/${SASKIT_FILENAME})
    if(WIN32)
        # some required dependencies on windows platforms
        foreach(LIB_NAME gcc_s_dw2-1 stdc++-6)
            unset(DEP_LIB CACHE)
            find_library(DEP_LIB "${LIB_NAME}")
            list(APPEND SASFIT_BIN_FILE_LIST "${DEP_LIB}")
        endforeach()
        set(SASFIT_BIN_FILE_LIST ${SASFIT_BIN_FILE_LIST} PARENT_SCOPE)
    endif()
    if(EXISTS "${SASKIT_FILE}")
        return() # nothing to do if saskit file exists already
    endif()
    message(STATUS "Saskit file '${SASKIT_FILE}' does not exist. "
                   "Attempting to build it.")
    if(EXISTS "${TCL_SH}")
        set(ENV{TCLSH} "${TCL_SH}")
    endif()
    execute_process(COMMAND sh gen/common.sh
                    WORKING_DIRECTORY ${SASKIT_PATH})
    site_name(HOSTNAME)
    string(TOLOWER "${HOSTNAME}" HOSTNAME)
    string(REGEX MATCH "[0-9a-zA-Z-_]+" HOSTNAME "${HOSTNAME}")
    set(GLOB_PATTERN "${SASKIT_PATH}/bin/${HOSTNAME}*/dqkit*")
    file(GLOB DQKIT_FILE ${GLOB_PATTERN})
    message(STATUS "dqfile: '${DQKIT_FILE}'")
    if(EXISTS "${DQKIT_FILE}")
        configure_file("${DQKIT_FILE}" "${SASKIT_FILE}" COPYONLY)
    else()
        message(STATUS "dq file built previously not found!")
        message(STATUS "glob pattern: '${GLOB_PATTERN}'")
    endif()
endfunction()

# vim: set ts=4 sw=4 sts=4 tw=0:
