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
# This module expects to live in <sasfit-dir>/src/cmake

cmake_minimum_required(VERSION 2.6.2)
cmake_policy(SET CMP0011 NEW)
cmake_policy(SET CMP0012 NEW)

include(GetPrerequisites)

set(CMAKE_LEGACY_CYGWIN_WIN32 1)
# fix processor architecture on Windows with MSYS2/MinGW64
if(MINGW)
    if(NOT WIN32)
        set(WIN32 1)
    endif()
    if(NOT CMAKE_SYSTEM_PROCESSOR OR CMAKE_SYSTEM_PROCESSOR MATCHES "unknown")
        set(CMAKE_SYSTEM_PROCESSOR $ENV{MSYSTEM_CARCH})
        if(NOT CMAKE_SYSTEM_PROCESSOR)
            set(CMAKE_SYSTEM_PROCESSOR $ENV{MSYS2_ARCH})
        endif()
        set(CMAKE_HOST_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR})
    endif()
endif()
if(0) # some debug info, in case ...
	message("WIN32, MINGW: ${WIN32}, ${MINGW}")
	message("CMAKE_GENERATOR: '${CMAKE_GENERATOR}'")
	message("CMAKE_SYSTEM_NAME: '${CMAKE_SYSTEM_NAME}' '${CMAKE_SYSTEM}'")
	message("CMAKE_HOST_APPLE:   ${CMAKE_HOST_APPLE}")
	message("CMAKE_SYSTEM_PROCESSOR: '${CMAKE_SYSTEM_PROCESSOR}'")
    message("CMAKE_HOST_SYSTEM_PROCESSOR: '${CMAKE_HOST_SYSTEM_PROCESSOR}'")
	message("CMAKE_EXECUTABLE_SUFFIX: '${CMAKE_EXECUTABLE_SUFFIX}'")
	message("CMAKE_INSTALL_PREFIX: '${CMAKE_INSTALL_PREFIX}'")
    message("CMAKE_SHARED_LIBRARY_SUFFIX: '${CMAKE_SHARED_LIBRARY_SUFFIX}'")
	message("CMAKE_STATIC_LIBRARY_SUFFIX: '${CMAKE_STATIC_LIBRARY_SUFFIX}'")
	message("CMAKE_SHARED_MODULE_SUFFIX: '${CMAKE_SHARED_MODULE_SUFFIX}'")
	message("CMAKE_EXTRA_LINK_EXTENSIONS: '${CMAKE_EXTRA_LINK_EXTENSIONS}'")
	message("CMAKE_EXTRA_SHARED_LIBRARY_SUFFIXES: '${CMAKE_EXTRA_SHARED_LIBRARY_SUFFIXES}'")
endif()

# check for target architecture 64bit?
set(SYSTEM_IS_64 FALSE)
if(CMAKE_SIZEOF_VOID_P)
    if(${CMAKE_SIZEOF_VOID_P} EQUAL 8) # check for 64bit platform
        set(SYSTEM_IS_64 TRUE)
    endif()
endif()

set(PLATFORM "${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}")
string(TOLOWER ${PLATFORM} PLATFORM)

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

# retrieves the path to the already extracted source package
# sets result variables in parent scope:
# SOURCE_DIR: absolute path to source package directory
# BASE_NAME:  name of the current dir,
#             usually the base name of the package to build
function(get_package_dir CURRENT_DIR)
    if(NOT EXISTS ${CURRENT_DIR} OR NOT DEFINED PLATFORM)
        return()
    endif()
    get_filename_component(CURRENT_DIR "${CURRENT_DIR}" REALPATH)
    message(STATUS "get_package_dir: '${CURRENT_DIR}'")
    get_filename_component(BASE_NAME ${CURRENT_DIR} NAME)
    set(BASE_NAME ${BASE_NAME} PARENT_SCOPE)
    message(STATUS "package name: '${BASE_NAME}'")
    unset(SOURCE_DIR)
    find_configure(${CURRENT_DIR}/${PLATFORM})
    #message("SOURCE_DIR: '${SOURCE_DIR}' '${SUFFIX_DIR}' '${PLATFORM}'")
    message(STATUS "found source dir: '${SOURCE_DIR}'")
    message(STATUS "found config file: '${CONFIG_FILE}'")
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
    set(subdirCandidates .)
    if(WIN32)
        list(APPEND subdirCandidates win)
    elseif(UNIX)
        list(APPEND subdirCandidates unix)
    endif()
    set(configfileCandidates configure CMakeLists.txt)
    foreach(configfileCandidate ${configfileCandidates})
        foreach(subdirCandidate ${subdirCandidates})
            # try sth for the TCL case
            file(GLOB CONFIG_FILE "${PCKG_PATH}/${subdirCandidate}/${configfileCandidate}")
            if(CONFIG_FILE)
                get_filename_component(CONFIG_PATH ${CONFIG_FILE} PATH ABSOLUTE)
                if(EXISTS "${CONFIG_PATH}")
                    set(SUFFIX_DIR ${subdirCandidate} PARENT_SCOPE)
                    get_filename_component(SOURCE_DIR "${CONFIG_PATH}" PATH ABSOLUTE)
                    set(SOURCE_DIR "${SOURCE_DIR}" PARENT_SCOPE)
                    get_filename_component(CONFIG_FILE "${CONFIG_FILE}" NAME)
                    set(CONFIG_FILE "${CONFIG_FILE}" PARENT_SCOPE)
                    return() # found it
                endif()
            endif()
        endforeach()
    endforeach()
endfunction()

# looks for existing source dirs and extracts the package
function(build_from_source CURRENT_DIR CONFIG_OPTIONS)
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
    file(RENAME "${SOURCE_DIR}" "${TARGET}")

    # look again for the source directory, should be ready now
    get_package_dir(${CURRENT_DIR})
    # get configure options from var args of this function
    set(CONFIG_OPTIONS)
    foreach(ARGIDX RANGE 1 ${ARGC})
        list(APPEND CONFIG_OPTIONS ${ARGV${ARGIDX}})
    endforeach()

    set(WORK_DIR ${SOURCE_DIR}/${SUFFIX_DIR})
    get_filename_component(WORK_DIR "${WORK_DIR}" REALPATH)

    message(STATUS "Searching for patches in: '${CURRENT_DIR}'")
    # applying any patches lying around in ${CURRENT_DIR}
    execute_process(COMMAND sh -c "for fn in $(ls -1 \"${CURRENT_DIR}\"/*.patch); do
                                     echo \"Applying '$fn':\";
                                     patch -p1 < \"$fn\";
                                   done"
                    WORKING_DIRECTORY ${WORK_DIR})

    # configure and build by appropriate script
    if(${CONFIG_FILE} STREQUAL "configure")
        run_configure(${CURRENT_DIR} "${CONFIG_OPTIONS}")
    elseif(${CONFIG_FILE} STREQUAL "CMakeLists.txt")
        run_cmake(${CURRENT_DIR} "${CONFIG_OPTIONS}")
    endif()
endfunction()

function(run_cmake CURRENT_DIR CONFIG_OPTIONS)
    # local build directory
    set(WORK_DIR ${WORK_DIR}/build)
    file(MAKE_DIRECTORY "${WORK_DIR}")
    # run configure script
    message(STATUS "Running ${BASE_NAME} cmake with options: '${CONFIG_OPTIONS}'")

    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" ${CONFIG_OPTIONS} ..
                    WORKING_DIRECTORY ${WORK_DIR})

    # run make, i.e. build the library and install it in this local path
    message(STATUS "Building ${BASE_NAME} ...")
    execute_process(COMMAND make all
                    WORKING_DIRECTORY ${WORK_DIR})
endfunction()

function(run_configure CURRENT_DIR CONFIG_OPTIONS)
    # set general package install location
    list(APPEND CONFIG_OPTIONS --prefix=${SOURCE_DIR})
    # run configure script
    message(STATUS "Running ${BASE_NAME} configure with options: '${CONFIG_OPTIONS}'")
    execute_process(COMMAND sh configure ${CONFIG_OPTIONS}
                    WORKING_DIRECTORY ${WORK_DIR})

    # run make, i.e. build the library and install it in this local path
    message(STATUS "Building ${BASE_NAME} ...")
    execute_process(COMMAND make all
                    WORKING_DIRECTORY ${WORK_DIR})
    execute_process(COMMAND make install
                    WORKING_DIRECTORY ${WORK_DIR})
endfunction()

function(build_saskit SASFIT_ROOT_DIR SASKIT_FILENAME)
    set(SASKIT_PATH ${SASFIT_ROOT_DIR}/saskit)
    set(SASKIT_FILE ${SASKIT_PATH}/${SASKIT_FILENAME})
    if(EXISTS "${SASKIT_FILE}")
        message(STATUS "saskit exists already: '${SASKIT_FILE}'.")
        return() # nothing to do if saskit file exists already
    endif()
    message(STATUS "Building saskit '${SASKIT_FILE}'.")
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

function(get_prerequisites2 target prerequisites_var exclude_system recurse exepath dirs)
  set(verbose 1)
  set(eol_char "E")
  if(ARGC GREATER 6)
    set(rpaths "${ARGV6}")
  else()
    set(rpaths "")
  endif()

  if(NOT IS_ABSOLUTE "${target}")
    message("warning: target '${target}' is not absolute...")
  endif()

  if(NOT EXISTS "${target}")
    message("warning: target '${target}' does not exist...")
    return()
  endif()

  set(gp_cmd_paths ${gp_cmd_paths}
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\14.0;InstallDir]/../../VC/bin"
    "$ENV{VS140COMNTOOLS}/../../VC/bin"
    "C:/Program Files (x86)/Microsoft Visual Studio 14.0/VC/bin"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\12.0;InstallDir]/../../VC/bin"
    "$ENV{VS120COMNTOOLS}/../../VC/bin"
    "C:/Program Files (x86)/Microsoft Visual Studio 12.0/VC/bin"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\11.0;InstallDir]/../../VC/bin"
    "$ENV{VS110COMNTOOLS}/../../VC/bin"
    "C:/Program Files (x86)/Microsoft Visual Studio 11.0/VC/bin"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\10.0;InstallDir]/../../VC/bin"
    "$ENV{VS100COMNTOOLS}/../../VC/bin"
    "C:/Program Files (x86)/Microsoft Visual Studio 10.0/VC/bin"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\9.0;InstallDir]/../../VC/bin"
    "$ENV{VS90COMNTOOLS}/../../VC/bin"
    "C:/Program Files/Microsoft Visual Studio 9.0/VC/bin"
    "C:/Program Files (x86)/Microsoft Visual Studio 9.0/VC/bin"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\8.0;InstallDir]/../../VC/bin"
    "$ENV{VS80COMNTOOLS}/../../VC/bin"
    "C:/Program Files/Microsoft Visual Studio 8/VC/BIN"
    "C:/Program Files (x86)/Microsoft Visual Studio 8/VC/BIN"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\7.1;InstallDir]/../../VC7/bin"
    "$ENV{VS71COMNTOOLS}/../../VC7/bin"
    "C:/Program Files/Microsoft Visual Studio .NET 2003/VC7/BIN"
    "C:/Program Files (x86)/Microsoft Visual Studio .NET 2003/VC7/BIN"
    )

  # <setup-gp_tool-vars>
  #
  # Try to choose the right tool by default. Caller can set gp_tool prior to
  # calling this function to force using a different tool.
  #
  if(NOT gp_tool)
    set(gp_tool "ldd")

    if(APPLE)
      set(gp_tool "otool")
    endif()

    if(WIN32 AND NOT UNIX) # This is how to check for cygwin, har!
      find_program(gp_dumpbin "dumpbin" PATHS ${gp_cmd_paths})
      if(gp_dumpbin)
        set(gp_tool "dumpbin")
      else() # Try harder. Maybe we're on MinGW
        set(gp_tool "objdump")
      endif()
    endif()
  endif()

  find_program(gp_cmd ${gp_tool} PATHS ${gp_cmd_paths})

  if(NOT gp_cmd)
    message(STATUS "warning: could not find '${gp_tool}' - cannot analyze prerequisites...")
    return()
  endif()

  set(gp_cmd_maybe_filter)      # optional command to pre-filter gp_tool results

  if(gp_tool STREQUAL "ldd")
    set(gp_cmd_args "")
    set(gp_regex "^[\t ]*[^\t ]+ => ([^\t\(]+) .*${eol_char}$")
    set(gp_regex_error "not found${eol_char}$")
    set(gp_regex_fallback "^[\t ]*([^\t ]+) => ([^\t ]+).*${eol_char}$")
    set(gp_regex_cmp_count 1)
  elseif(gp_tool STREQUAL "otool")
    set(gp_cmd_args "-L")
    set(gp_regex "^\t([^\t]+) \\(compatibility version ([0-9]+.[0-9]+.[0-9]+), current version ([0-9]+.[0-9]+.[0-9]+)\\)${eol_char}$")
    set(gp_regex_error "")
    set(gp_regex_fallback "")
    set(gp_regex_cmp_count 3)
  elseif(gp_tool STREQUAL "dumpbin")
    set(gp_cmd_args "/dependents")
    set(gp_regex "^    ([^ ].*[Dd][Ll][Ll])${eol_char}$")
    set(gp_regex_error "")
    set(gp_regex_fallback "")
    set(gp_regex_cmp_count 1)
  elseif(gp_tool STREQUAL "objdump")
    set(gp_cmd_args "-p")
    set(gp_regex "^\t*DLL Name: (.*\\.[Dd][Ll][Ll])${eol_char}$")
    set(gp_regex_error "")
    set(gp_regex_fallback "")
    set(gp_regex_cmp_count 1)
    # objdump generates copious output so we create a grep filter to pre-filter results
    if(WIN32)
      find_program(gp_grep_cmd findstr)
    else()
      find_program(gp_grep_cmd grep)
    endif()
    if(gp_grep_cmd)
      set(gp_cmd_maybe_filter COMMAND ${gp_grep_cmd} "-a" "^[[:blank:]]*DLL Name: ")
    endif()
  else()
    message(STATUS "warning: gp_tool='${gp_tool}' is an unknown tool...")
    message(STATUS "CMake function get_prerequisites needs more code to handle '${gp_tool}'")
    message(STATUS "Valid gp_tool values are dumpbin, ldd, objdump and otool.")
    return()
  endif()


  if(gp_tool STREQUAL "dumpbin")
    # When running dumpbin, it also needs the "Common7/IDE" directory in the
    # PATH. It will already be in the PATH if being run from a Visual Studio
    # command prompt. Add it to the PATH here in case we are running from a
    # different command prompt.
    #
    get_filename_component(gp_cmd_dir "${gp_cmd}" PATH)
    get_filename_component(gp_cmd_dlls_dir "${gp_cmd_dir}/../../Common7/IDE" ABSOLUTE)
    # Use cmake paths as a user may have a PATH element ending with a backslash.
    # This will escape the list delimiter and create havoc!
    if(EXISTS "${gp_cmd_dlls_dir}")
      # only add to the path if it is not already in the path
      set(gp_found_cmd_dlls_dir 0)
      file(TO_CMAKE_PATH "$ENV{PATH}" env_path)
      foreach(gp_env_path_element ${env_path})
        if(gp_env_path_element STREQUAL gp_cmd_dlls_dir)
          set(gp_found_cmd_dlls_dir 1)
        endif()
      endforeach()

      if(NOT gp_found_cmd_dlls_dir)
        file(TO_NATIVE_PATH "${gp_cmd_dlls_dir}" gp_cmd_dlls_dir)
        set(ENV{PATH} "$ENV{PATH};${gp_cmd_dlls_dir}")
      endif()
    endif()
  endif()
  #
  # </setup-gp_tool-vars>

  if(gp_tool STREQUAL "ldd")
    set(old_ld_env "$ENV{LD_LIBRARY_PATH}")
    set(new_ld_env "${exepath}")
    foreach(dir ${dirs})
      string(APPEND new_ld_env ":${dir}")
    endforeach()
    set(ENV{LD_LIBRARY_PATH} "${new_ld_env}:$ENV{LD_LIBRARY_PATH}")
  endif()


  # Track new prerequisites at each new level of recursion. Start with an
  # empty list at each level:
  #
  set(unseen_prereqs)

  # Run gp_cmd on the target:
  #
  execute_process(
    COMMAND ${gp_cmd} ${gp_cmd_args} ${target}
    ${gp_cmd_maybe_filter}
    RESULT_VARIABLE gp_rv
    OUTPUT_VARIABLE gp_cmd_ov
    ERROR_VARIABLE gp_ev
    )

  if(gp_tool STREQUAL "dumpbin")
    # Exclude delay load dependencies under windows (they are listed in dumpbin output after the message below)
    string(FIND "${gp_cmd_ov}" "Image has the following delay load dependencies" gp_delayload_pos)
    if (${gp_delayload_pos} GREATER -1)
      string(SUBSTRING "${gp_cmd_ov}" 0 ${gp_delayload_pos} gp_cmd_ov_no_delayload_deps)
      string(SUBSTRING "${gp_cmd_ov}" ${gp_delayload_pos} -1 gp_cmd_ov_delayload_deps)
      if (verbose)
        message(STATUS "GetPrequisites(${target}) : ignoring the following delay load dependencies :\n ${gp_cmd_ov_delayload_deps}")
      endif()
      set(gp_cmd_ov ${gp_cmd_ov_no_delayload_deps})
    endif()
  endif()

  if(NOT gp_rv STREQUAL "0")
    if(gp_tool STREQUAL "dumpbin")
      # dumpbin error messages seem to go to stdout
      message(FATAL_ERROR "${gp_cmd} failed: ${gp_rv}\n${gp_ev}\n${gp_cmd_ov}")
    else()
      message(FATAL_ERROR "${gp_cmd} failed: ${gp_rv}\n${gp_ev}")
    endif()
  endif()

  if(gp_tool STREQUAL "ldd")
    set(ENV{LD_LIBRARY_PATH} "${old_ld_env}")
  endif()

  if(verbose)
    message(STATUS "<RawOutput cmd='${gp_cmd} ${gp_cmd_args} ${target}'>")
    message(STATUS "gp_cmd_ov='${gp_cmd_ov}'")
    message(STATUS "</RawOutput>")
  endif()

  get_filename_component(target_dir "${target}" PATH)

  # Convert to a list of lines:
  #
  string(REPLACE ";" "\\;" candidates "${gp_cmd_ov}")
  string(REPLACE "\n" "${eol_char};" candidates "${candidates}")

  # check for install id and remove it from list, since otool -L can include a
  # reference to itself
  set(gp_install_id)
  if(gp_tool STREQUAL "otool")
    execute_process(
      COMMAND otool -D ${target}
      RESULT_VARIABLE otool_rv
      OUTPUT_VARIABLE gp_install_id_ov
      ERROR_VARIABLE otool_ev
      )
    if(NOT otool_rv STREQUAL "0")
      message(FATAL_ERROR "otool -D failed: ${otool_rv}\n${otool_ev}")
    endif()
    # second line is install name
    string(REGEX REPLACE ".*:\n" "" gp_install_id "${gp_install_id_ov}")
    if(gp_install_id)
      # trim
      string(REGEX MATCH "[^\n ].*[^\n ]" gp_install_id "${gp_install_id}")
      #message("INSTALL ID is \"${gp_install_id}\"")
    endif()
  endif()

  # Analyze each line for file names that match the regular expression:
  #
  foreach(candidate ${candidates})
  if("${candidate}" MATCHES "${gp_regex}")

    # Extract information from each candidate:
    if(gp_regex_error AND "${candidate}" MATCHES "${gp_regex_error}")
      string(REGEX REPLACE "${gp_regex_fallback}" "\\1" raw_item "${candidate}")
    else()
      string(REGEX REPLACE "${gp_regex}" "\\1" raw_item "${candidate}")
    endif()

    if(gp_regex_cmp_count GREATER 1)
      string(REGEX REPLACE "${gp_regex}" "\\2" raw_compat_version "${candidate}")
      string(REGEX REPLACE "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$" "\\1" compat_major_version "${raw_compat_version}")
      string(REGEX REPLACE "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$" "\\2" compat_minor_version "${raw_compat_version}")
      string(REGEX REPLACE "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$" "\\3" compat_patch_version "${raw_compat_version}")
    endif()

    if(gp_regex_cmp_count GREATER 2)
      string(REGEX REPLACE "${gp_regex}" "\\3" raw_current_version "${candidate}")
      string(REGEX REPLACE "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$" "\\1" current_major_version "${raw_current_version}")
      string(REGEX REPLACE "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$" "\\2" current_minor_version "${raw_current_version}")
      string(REGEX REPLACE "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$" "\\3" current_patch_version "${raw_current_version}")
    endif()

    # Use the raw_item as the list entries returned by this function. Use the
    # gp_resolve_item function to resolve it to an actual full path file if
    # necessary.
    #
    set(item "${raw_item}")

    # Add each item unless it is excluded:
    #
    set(add_item 1)

    if(item STREQUAL gp_install_id)
      set(add_item 0)
    endif()

    if(add_item AND ${exclude_system})
      set(type "")
      gp_resolved_file_type("${target}" "${item}" "${exepath}" "${dirs}" type "${rpaths}")

      if(type STREQUAL "system")
        set(add_item 0)
      endif()
    endif()

    if(add_item)
      list(LENGTH ${prerequisites_var} list_length_before_append)
      gp_append_unique(${prerequisites_var} "${item}")
      list(LENGTH ${prerequisites_var} list_length_after_append)

      if(${recurse})
        # If item was really added, this is the first time we have seen it.
        # Add it to unseen_prereqs so that we can recursively add *its*
        # prerequisites...
        #
        # But first: resolve its name to an absolute full path name such
        # that the analysis tools can simply accept it as input.
        #
        if(NOT list_length_before_append EQUAL list_length_after_append)
          gp_resolve_item("${target}" "${item}" "${exepath}" "${dirs}" resolved_item "${rpaths}")
          if(EXISTS "${resolved_item}")
            # Recurse only if we could resolve the item.
            # Otherwise the prerequisites_var list will be cleared
            set(unseen_prereqs ${unseen_prereqs} "${resolved_item}")
          endif()
        endif()
      endif()
    endif()
  else()
    if(verbose)
      message(STATUS "ignoring non-matching line: '${candidate}'")
    endif()
  endif()
  endforeach()

  list(LENGTH ${prerequisites_var} prerequisites_var_length)
  if(prerequisites_var_length GREATER 0)
    list(SORT ${prerequisites_var})
  endif()
  if(${recurse})
    set(more_inputs ${unseen_prereqs})
    foreach(input ${more_inputs})
      get_prerequisites("${input}" ${prerequisites_var} ${exclude_system} ${recurse} "${exepath}" "${dirs}" "${rpaths}")
    endforeach()
  endif()

  set(${prerequisites_var} ${${prerequisites_var}} PARENT_SCOPE)
endfunction()

function(get_saskit_dependencies SASFIT_ROOT_DIR SASKIT_FILENAME)
    set(SASKIT_FILE ${SASFIT_ROOT_DIR}/saskit/${SASKIT_FILENAME})
    if(NOT EXISTS "${SASKIT_FILE}")
        message(STATUS "saskit '${SASKIT_FILE}' does not exist, not checking dependencies.")
        return()
    endif()
    # init with linux settings
    set(EXCLUDE_SYSTEM FALSE)
    set(RECURSIVE FALSE)
    if(WIN32)
        set(EXCLUDE_SYSTEM TRUE)
    elseif(UNIX AND APPLE) # osx
        # get libx11 only and its dependencies
        set(EXCLUDE_SYSTEM TRUE)
        set(RECURSIVE TRUE)
    endif()
    message(STATUS "Searching dependencies of '${SASKIT_FILE}'")
    if(TRUE) # for debugging
        message("EXCLUDE_SYSTEM: ${EXCLUDE_SYSTEM}")
        message("RECURSIVE:      ${RECURSIVE}")
    endif()
    message("debugging get_prerequisites:")
    execute_process(COMMAND ls -la "/c/msys64/")
    message("pre get_prerequisites")
    get_prerequisites2(${SASKIT_FILE} PREREQ ${EXCLUDE_SYSTEM} ${RECURSIVE} "" "")
#    message("PREREQ: ${PREREQ}") # for debugging
    if(UNIX AND NOT APPLE) # linux
        # avoid modifying LD_LIBRARY_PATH,
        # using dyn.lib.loader in sasfit.sh instead
        list(APPEND PREREQ
            "/lib/*-linux-gnu/ld-[0-9].[0-9][0-9].so"
#            "/lib/*-linux-gnu/libpthread.so.[0-9]"
            "/usr/lib/*-linux-gnu/libXrender.so.[0-9]"
            "/usr/lib/*-linux-gnu/libXfixes.so.[0-9]"
            )
    elseif(WIN32)
        find_library(pthread_LIB winpthread-1)
#        message("found pthread'? '${pthread_LIB}'")
        list(APPEND PREREQ "${pthread_LIB}")
    endif()
    message(STATUS "dependent libs:")
    foreach(FN ${PREREQ})
        unset(ABSFN)
        if(NOT EXISTS "${FN}")
            find_library(ABSFN "${FN}")
#            message("found lib '${FN}'? '${ABSFN}'")
            if(NOT ABSFN)
                message("not found")
                file(GLOB ABSFN "${FN}")
			elseif(ABSFN MATCHES ".*Windows.System32.*")
#                message("Provided by System32 folder!")
                unset(ABSFN CACHE)
                continue()
            endif()
        else()
            set(ABSFN "${FN}")
        endif()
        if(EXISTS "${ABSFN}")
            list(APPEND SASFIT_BIN_FILE_LIST "${ABSFN}")
            message(STATUS "    ${ABSFN}")
        else()
            message(WARNING "Dependency not found: '${FN}'")
        endif()
        unset(ABSFN CACHE)
    endforeach()
#    message("SASFIT_BIN_FILE_LIST: ${SASFIT_BIN_FILE_LIST}") # for debugging
    set(SASFIT_BIN_FILE_LIST ${SASFIT_BIN_FILE_LIST} PARENT_SCOPE)
endfunction()

# vim: set ts=4 sw=4 sts=4 tw=0:
