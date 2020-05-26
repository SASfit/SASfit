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
# A CMake module to provide utility functions and macros used elsewhere.
#
# This module expects to live in <sasfit-dir>/src/cmake

cmake_minimum_required(VERSION 3.0)

# replaced by file(GET_RUNTIME_DEPENDENCIES)
include(GetPrerequisites)

# check for target architecture 64bit?
set(SYSTEM_IS_64 FALSE)
if(CMAKE_SIZEOF_VOID_P)
    if(${CMAKE_SIZEOF_VOID_P} EQUAL 8) # check for 64bit platform
        set(SYSTEM_IS_64 TRUE)
    endif()
endif()

# set system platform name to a user friendly text
string(TOLOWER "${CMAKE_SYSTEM_NAME}" PLATFORM)
if(WIN32)
    set(PLATFORM "Windows")
endif()
if(CMAKE_HOST_APPLE)
    set(MACOS TRUE)
    set(PLATFORM "macos")
endif()
if(SYSTEM_IS_64)
    set(PLATFORM "${PLATFORM}64")
endif()
string(TOLOWER "${PLATFORM}" PLATFORM)

set(VERSION_TIMESTAMP_FORMAT "%y%m%d%H%M%S")
set(APPVEYOR_API_URL "https://ci.appveyor.com/api/projects/SASfit/sasfit")

# defining some colors
string(ASCII 27 ESC)
set(colend  "${ESC}[m")
set(white   "${ESC}[37m")
set(bold    "${ESC}[1m")
set(green   "${ESC}[32m")

# determine sasfit-root directory
if(NOT DEFINED SASFIT_ROOT_DIR)
	GET_FILENAME_COMPONENT(SASFIT_ROOT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
	GET_FILENAME_COMPONENT(SASFIT_ROOT_DIR "${SASFIT_ROOT_DIR}/../.." ABSOLUTE)
endif()
set(LIBRARY_OUTPUT_PATH ${SASFIT_ROOT_DIR}/lib)

macro(dbg_cmake_vars)
    # some debug info, in case ...
    message(STATUS "ENV{MSYSTEM}: '$ENV{MSYSTEM}'")
    cmake_print_variables(WIN32 MINGW MSYS UNIX MACOS)
    cmake_print_variables(CMAKE_HOST_WIN32)
    cmake_print_variables(CMAKE_SYSTEM_NAME)
    cmake_print_variables(CMAKE_SYSTEM)
    cmake_print_variables(PLATFORM)
    cmake_print_variables(SYSTEM_IS_64)
    cmake_print_variables(CMAKE_GENERATOR)
    cmake_print_variables(CMAKE_HOST_APPLE)
    cmake_print_variables(CMAKE_SYSTEM_PROCESSOR)
    cmake_print_variables(CMAKE_HOST_SYSTEM_PROCESSOR)
    cmake_print_variables(CMAKE_EXECUTABLE_SUFFIX)
    cmake_print_variables(CMAKE_INSTALL_PREFIX)
    cmake_print_variables(CMAKE_SHARED_LIBRARY_SUFFIX)
    cmake_print_variables(CMAKE_STATIC_LIBRARY_SUFFIX)
    cmake_print_variables(CMAKE_SHARED_MODULE_SUFFIX)
    cmake_print_variables(CMAKE_EXTRA_LINK_EXTENSIONS)
    cmake_print_variables(CMAKE_EXTRA_SHARED_LIBRARY_SUFFIXES)
    cmake_print_variables(CMAKE_C_COMPILER_LAUNCHER)
    message("ENV:")
    execute_process(COMMAND "env" COMMAND "sort")
endmacro()

# Prints out all optional arguments conveniently formatted one per line
function(list_paths channel description)
    set(indent "    ")
    message(${channel} "${description}")
    foreach(entry ${ARGN})
        message(${channel} "${indent}${entry}")
    endforeach()
endfunction()

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
#	message("replace_str_in_file '${FILENAME}' '${PATTERN_STR}' '${REPLACE_STR}'")
	file(READ ${FILENAME} FILE_BODY)
	string(REGEX REPLACE "${PATTERN_STR}" "${REPLACE_STR}"
		FILE_BODY_NEW "${FILE_BODY}")
	file(WRITE "${FILENAME}" "${FILE_BODY_NEW}")
endmacro(replace_str_in_file)

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
    message("")
    message(STATUS "${bold}${green}Configuring Plugin '${PRJ_NAME}':${colend}")

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
	set_property(TARGET ${PRJ_NAME} PROPERTY COMPILE_DEFINITIONS MAKE_SASFIT_PLUGIN;SASFIT_PLUGIN_NAME=${PRJ_NAME})
	set(COMPILE_FLAGS)
	if(UNIX)
		set(COMPILE_FLAGS "-Wall")
		if(DEFINED SASFIT_DEBUG)
			set(COMPILE_FLAGS "${COMPILE_FLAGS} -g") # -O0 -save-temps
		endif(DEFINED SASFIT_DEBUG)
	endif(UNIX)
	set_target_properties(${PRJ_NAME} PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS}")

	# set library search paths for internal shared libraries
	# build with the whole sasfit package controlled by a toplevel CMakeLists
	sasfit_copy_plugins(${PRJ_NAME} "${SRC_DIR}/include" "${PRJ_NAME}.h")

	if(DEFINED WITH_STATIC)
		sasfit_cmake_plugin_static()
	endif(DEFINED WITH_STATIC)

endmacro(sasfit_cmake_plugin)

macro(get_git_info)
    find_package(Git)
    if(NOT GIT_FOUND)
        return()
    endif()
    # get the abbreviated commit hash
    execute_process(COMMAND git show -s --format=%h
        WORKING_DIRECTORY ${SASFIT_ROOT_DIR}
        OUTPUT_VARIABLE GIT_COMMIT
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    # get the commit timestamp incl. seconds
    execute_process(COMMAND git show -s --format=%ad
                                        --date=format:${VERSION_TIMESTAMP_FORMAT}
        WORKING_DIRECTORY ${SASFIT_ROOT_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_DATETIME
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    # get the tag if there is one
    execute_process(COMMAND git tag --contains
        WORKING_DIRECTORY ${SASFIT_ROOT_DIR}
        OUTPUT_VARIABLE GIT_TAG
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(GIT_TAG_MSG)
    if(GIT_TAG)
        set(GIT_TAG_MSG " (tag: ${GIT_TAG})")
    endif()
    message(STATUS "This source tree is on GIT commit ${GIT_COMMIT}${GIT_TAG_MSG}"
                   " with timestamp ${GIT_COMMIT_DATETIME}.")
endmacro()

# splits the provided string *str* at the given separator *sep*
# and returns the second (latter) part of the two
function(string_split_2nd str sep)
    string(FIND ${${str}} ${sep} istart)        # get position of separator
    #cmake_print_variables(istart)
    math(EXPR istart "${istart}+1")          # stripping colon and quote at the front
    string(SUBSTRING ${${str}} ${istart} -1 ${str})
    set(${str} ${${str}} PARENT_SCOPE)
endfunction()

function(appveyor_get_latest_version)
    find_program(curl_path name curl HINTS ENV PATH)
    if(NOT curl_path)
        message(FATAL_ERROR "Could not find the curl program! "
                            "Can't configure the version, giving up!")
    endif()
    # get info on the last 5 builds
    execute_process(COMMAND ${curl_path} -s ${APPVEYOR_API_URL}/history?recordsNumber=5
                    OUTPUT_VARIABLE appveyor_last_build_json)
    #cmake_print_variables(appveyor_last_build_json)
    # find the version text in JSON of the last build on appveyor
    string(REGEX MATCHALL "\"version\":\"[^\"]+\"" match_version
                          ${appveyor_last_build_json})
    #cmake_print_variables(match_version)
    string(REGEX MATCHALL "\"buildNumber\":\"?[0-9]+\"?" match_buildNumber
                          ${appveyor_last_build_json})
    #cmake_print_variables(match_buildNumber)
    # find a version unequal build number
    # (both are equal at the beginning, version is changed by CMake later)
    foreach(ver bnum IN ZIP_LISTS match_version match_buildNumber)
        #cmake_print_variables(ver bnum)
        string_split_2nd(ver :)
        string_split_2nd(bnum :)
        string(REPLACE "\"" "" ver ${ver})
        string(REPLACE "\"" "" bnum ${bnum})
        #cmake_print_variables(ver bnum)
        if(NOT ${ver} STREQUAL ${bnum})
            set(last_version ${ver})
            break()
        endif()
    endforeach()
    #cmake_print_variables(last_version)
    set(appveyor_latest_version ${last_version} PARENT_SCOPE)
endfunction()

function(appveyor_reset_build_number)
    # check for authentication token
    if(NOT DEFINED ENV{APPVEYOR_TOKEN})
        message(FATAL_ERROR "Appveyor auth token not provided, can't reset build number!")
    endif()
    execute_process(COMMAND ${curl_path} -s -X PUT
                        -H "Authorization: Bearer $ENV{APPVEYOR_TOKEN}"
                        -H Content-Type:application/json
                        -H Accept:application/json
                        -d "{ \"nextBuildNumber\": 1 }"
                        ${APPVEYOR_API_URL}/settings/build-number
    )
endfunction()

# set up a sortable version number based on the current GIT commit timestamp
# (the commit hash does not sort multiple packages chronologically in file system)
macro(sasfit_update_version)
    get_git_info()
    if(NOT GIT_FOUND) # no GIT, use current timestamp as version
        string(TIMESTAMP SASFIT_VERSION "${VERSION_TIMESTAMP_FORMAT}")
    else()
        #cmake_print_variables(GIT_FOUND GIT_COMMIT GIT_TAG GIT_COMMIT_DATETIME)
        # default version is timestamp based
        set(SASFIT_VERSION "${GIT_COMMIT_DATETIME}")
        if(GIT_TAG) # use the tag name if there is one
            set(SASFIT_VERSION ${GIT_TAG})
        endif()
        if($ENV{APPVEYOR}) # running on appveyor CI
            appveyor_get_latest_version()
            #cmake_print_variables(appveyor_latest_version)
            # strip build number from latest appveyor version
            string(REGEX MATCH "^[^b]+" appveyor_latest_version ${appveyor_latest_version})
            #cmake_print_variables(appveyor_latest_version)
            # check if the new version number is different
#            if(NOT ${SASFIT_VERSION} STREQUAL ${appveyor_latest_version})
#                # is different, reset appveyor build number
#                message(STATUS "Version changed, resetting Appveyor build number.")
#                appveyor_reset_build_number()
#            endif()
            # append build number for unique build version in appveyor
            set(appveyor_build_version "${SASFIT_VERSION}b$ENV{APPVEYOR_BUILD_NUMBER}")
            execute_process(COMMAND appveyor UpdateBuild -Version "${appveyor_build_version}")
            message(STATUS "Appveyor build version was set to '${appveyor_build_version}'.")
            execute_process(COMMAND appveyor SetVariable
                                        -Name SASFIT_VERSION -Value "${SASFIT_VERSION}")
        endif()
    endif()
    message(STATUS "SASfit version was set to '${SASFIT_VERSION}'.")

    # let the tcl code know about the svn revision number
    file(WRITE ${SASFIT_ROOT_DIR}/sasfit.vfs/lib/app-sasfit/tcl/sasfit_svn_rev.tcl
        "set sasfit(svn_rev) ${SASFIT_VERSION}"
    )
    # let the documentation know about the svn revision number
    replace_str_in_file(${SASFIT_ROOT_DIR}/src/Doxyfile 
        "PROJECT_NUMBER         = ([^\n]*)"
        "PROJECT_NUMBER         = ${SASFIT_VERSION}"
    )
    # NOTE: (TODO)
    # above code changes have to be reverted elsewhere after build
    # to prevent repeatedly commits of those files
endmacro()

# retrieves the path to the already extracted source package
# sets result variables in parent scope:
# SOURCE_DIR: absolute path to source package directory
function(get_package_dir PCKG_NAME CURRENT_DIR)
    if(NOT EXISTS ${CURRENT_DIR} OR NOT DEFINED PLATFORM)
        return()
    endif()
    message("")
    message(STATUS "${bold}${white}Looking for ${PCKG_NAME} in '${CURRENT_DIR}'${colend}")
    get_filename_component(CURRENT_DIR "${CURRENT_DIR}" REALPATH)
    unset(SOURCE_DIR)
    set_package_source_dir(PCKG_SRC_DIR ${CURRENT_DIR})
    find_configure(${PCKG_SRC_DIR})
    #message("SOURCE_DIR: '${SOURCE_DIR}' '${SUFFIX_DIR}' '${PLATFORM}'")
    if(SOURCE_DIR)
        message(STATUS "Found source dir:  '${SOURCE_DIR}'")
    else()
        message(STATUS "Package source dir not found.")
    endif()
    if(CONFIG_FILE)
        message(STATUS "Found config file: '${CONFIG_FILE}'")
    endif()
    set(SOURCE_DIR ${SOURCE_DIR} PARENT_SCOPE)
endfunction()

function(find_configure PCKG_PATH)
    message(STATUS "Searching configure in '${PCKG_PATH}'")
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

# sets the given variable name to the expected source directory of the package
function(set_package_source_dir varname workdir)
    set(${varname} ${workdir}/${PLATFORM} PARENT_SCOPE)
    # hostname is different for each worker for macos on appveyor
    if(NOT $ENV{APPVEYOR}) # running on appveyor CI
        cmake_host_system_information(RESULT CMAKE_HOSTNAME QUERY HOSTNAME)
        set(${varname} ${${varname}}_${CMAKE_HOSTNAME} PARENT_SCOPE)
    endif()
endfunction()

# looks for existing source dirs and extracts the package
function(build_from_source CURRENT_DIR CONFIG_OPTIONS)
    if(NOT EXISTS ${CURRENT_DIR} OR NOT DEFINED PLATFORM)
        return()
    endif()
    get_filename_component(BASE_NAME ${CURRENT_DIR} NAME)
    string(TOUPPER ${BASE_NAME} PCKG_NAME)
    message(STATUS "${PCKG_NAME} not found, building from source:")
    
    # look for existing source dir
    get_package_dir(${PCKG_NAME} ${CURRENT_DIR})

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
        message(STATUS "${PCKG_NAME} source dir not found! Giving up.")
        return()
    endif()

    set_package_source_dir(PCKG_SRC_DIR ${CURRENT_DIR})
    get_filename_component(from ${SOURCE_DIR} NAME)
    get_filename_component(to ${PCKG_SRC_DIR} NAME)
    message(STATUS "Renaming extracted package '${from}' -> '${to}'")
    if(EXISTS ${PCKG_SRC_DIR})
        file(REMOVE_RECURSE ${PCKG_SRC_DIR})
    endif()
    file(RENAME "${SOURCE_DIR}" "${PCKG_SRC_DIR}")

    # look again for the source directory, should be ready now
    get_package_dir(${PCKG_NAME} ${CURRENT_DIR})
    # get configure options from var args of this function
    set(CONFIG_OPTIONS)
    foreach(ARGIDX RANGE 1 ${ARGC})
        list(APPEND CONFIG_OPTIONS ${ARGV${ARGIDX}})
    endforeach()

    message(STATUS "Applying patches to ${PCKG_NAME} found in: '${CURRENT_DIR}'")
    # applying any patches lying around in ${CURRENT_DIR}
    execute_process(COMMAND sh -c
        "for fn in $(find '${CURRENT_DIR}' -maxdepth 1 -type f -name '*.patch'); do
            pwd;
            echo \"Applying '$fn':\";
            patch -p1 < \"$fn\";
        done"
        WORKING_DIRECTORY ${SOURCE_DIR})

    # set working directory for next build steps configure or cmake
    set(WORK_DIR ${SOURCE_DIR}/${SUFFIX_DIR})
    get_filename_component(WORK_DIR "${WORK_DIR}" REALPATH)

    # configure and build by appropriate script
    if(${CONFIG_FILE} STREQUAL "configure")
        run_configure(${CURRENT_DIR} "${CONFIG_OPTIONS}")
    elseif(${CONFIG_FILE} STREQUAL "CMakeLists.txt")
        run_cmake(${CURRENT_DIR} "${CONFIG_OPTIONS}")
    endif()
endfunction()

macro(get_make_job_count varname)
    # using externally defined number of cores
    set(${varname} "" PARENT_SCOPE)
    if($ENV{NUM_LOGICAL_CORES} AND $ENV{NUM_LOGICAL_CORES} GREATER 1)
        set(${varname} "-j$ENV{NUM_LOGICAL_CORES}" PARENT_SCOPE)
    endif()
endmacro()

function(run_cmake CURRENT_DIR CONFIG_OPTIONS)
    # local build directory
    set(WORK_DIR ${WORK_DIR}/build)
    file(MAKE_DIRECTORY "${WORK_DIR}")
    # run configure script
    message(STATUS "Running ${PCKG_NAME} cmake with options:\n"
                   "   '${CONFIG_OPTIONS}'")

    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" ${CONFIG_OPTIONS} ..
                    WORKING_DIRECTORY ${WORK_DIR})

    # run make, i.e. build the library and install it in this local path
    message(STATUS "Building ${PCKG_NAME} ...")
    get_make_job_count(jobcount)
    execute_process(COMMAND make ${jobcount} all
                    WORKING_DIRECTORY ${WORK_DIR})
endfunction()

function(run_configure CURRENT_DIR CONFIG_OPTIONS)
    # set general package install location
    list(APPEND CONFIG_OPTIONS --prefix=${SOURCE_DIR})
    # run configure script
    message(STATUS "Running ${PCKG_NAME} configure in '${WORK_DIR}' with options:\n"
                   "   '${CONFIG_OPTIONS}'")
    execute_process(COMMAND sh configure ${CONFIG_OPTIONS}
                    WORKING_DIRECTORY ${WORK_DIR})

    # run make, i.e. build the library and install it in this local path
    message(STATUS "Building ${PCKG_NAME} ...")
    get_make_job_count(jobcount)
    execute_process(COMMAND make ${jobcount} all
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

# modified get_prerequisites() from CMake module GetPrerequisites (v3.13.2)
# changes: use objdump on MSYS/MinGW platform, avoid dumpbin from MS VC
# relevant on Appveyor platform where Visual Studio tools are installed by default
function(get_prerequisites2 target prerequisites_var exclude_system recurse exepath dirs)
  set(verbose 0)
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
      if(NOT MINGW AND gp_dumpbin)
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

  if(verbose)
    message(STATUS "<RawOutput cmd='${gp_cmd} ${gp_cmd_args} ${target}'>")
    message(STATUS "gp_cmd_ov='${gp_cmd_ov}'")
    message(STATUS "</RawOutput>")
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

# Gets the path to a binary library and returns a list of libraries it depends on from non-standard locations.
# Checks dependencies of found files recursively.
function(get_dependent_libs out_list filename)
    if(NOT EXISTS "${filename}")
        message(STATUS "Library '${filename}' does not exist, not checking dependencies.")
        return()
    endif()
    message(STATUS "Searching dependencies of '${filename}':")
    if(MACOS)
        execute_process(COMMAND otool -L "${filename}"
                        COMMAND awk -F "(" "/^[[:space:]]/ { print $1 }"
                        COMMAND grep -v "^[[:space:]]\\+/System/Library"
                        COMMAND grep -v "^[[:space:]]\\+/usr/lib"
                        OUTPUT_VARIABLE dep_stdout
                        OUTPUT_STRIP_TRAILING_WHITESPACE)
    else() # win/mingw or linux
        # new CMake command file(GET_RUNTIME_DEPENDENCIES) does not identify libwinpthread* as dependency
        execute_process(COMMAND ldd "${filename}"
                        COMMAND awk "!/[\\/][wW][iI][nN][dD][oO][wW][sS][\\/]|[\\?]+/ {print $3}"
                        OUTPUT_VARIABLE dep_stdout
                        OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(WIN32)
            # find cygpath tool
            find_program(cygpath name cygpath HINTS ENV PATH)
            if(NOT EXISTS ${cygpath})
                message(WARNING "Cygpath tool could not be found!")
            else()
                message(STATUS "Using cygpath: '${cygpath}'.")
            endif()
        endif()
    endif()
    cmake_print_variables(dep_stdout)
#    if(UNIX AND NOT APPLE) # linux
        # avoid modifying LD_LIBRARY_PATH,
        # using dyn.lib.loader in sasfit.sh instead
#        list(APPEND PREREQ
#            "/lib/*-linux-gnu/ld-[0-9].[0-9][0-9].so"
#            "/lib/*-linux-gnu/libpthread.so.[0-9]"
#            "/usr/lib/*-linux-gnu/libXrender.so.[0-9]"
#            "/usr/lib/*-linux-gnu/libXfixes.so.[0-9]"
#            )
#    endif()

    separate_arguments(libs UNIX_COMMAND ${dep_stdout})
    foreach(fn ${libs})
        if(${fn} STREQUAL ${filename})
            continue() # skip same file investigating right here
        endif()
        message(STATUS "  Resolving ${fn}:")
        set(absfn "${fn}")
        if(cygpath)
            execute_process(COMMAND ${cygpath} -m "${fn}"
                            OUTPUT_VARIABLE absfn
                            OUTPUT_STRIP_TRAILING_WHITESPACE)
            message(STATUS "    (Located at '${absfn}'.)")
        endif()
        if(EXISTS "${absfn}")
            list(FIND ${out_list} "${absfn}" found_idx)
            if(found_idx LESS 0) # not found in list
                message(STATUS "    Exists, adding to binary files list.")
                list(APPEND ${out_list} "${absfn}")
                message(STATUS "    Checking dependencies of just added library.")
                get_dependent_libs(${out_list} ${absfn})
            else()
                message(STATUS "    Already in list, skipped.")
            endif()
        else()
            message(WARNING "Dependency not found: '${fn}'!")
        endif()
        unset(absfn CACHE)
    endforeach()
    set(${out_list} ${${out_list}} PARENT_SCOPE)
endfunction()

function(get_saskit_dependencies SASFIT_ROOT_DIR SASKIT_FILENAME)
    set(SASKIT_FILE ${SASFIT_ROOT_DIR}/saskit/${SASKIT_FILENAME})
    if(NOT EXISTS "${SASKIT_FILE}")
        message(STATUS "saskit '${SASKIT_FILE}' does not exist, not checking dependencies.")
        return()
    endif()
    get_dependent_libs(SASFIT_BIN_FILE_LIST ${SASKIT_FILE})

    list_paths(STATUS "Files included in binary package:" ${SASFIT_BIN_FILE_LIST})
    set(SASFIT_BIN_FILE_LIST ${SASFIT_BIN_FILE_LIST} PARENT_SCOPE)
endfunction()

# vim: set ts=4 sw=4 sts=4 tw=0 et:
