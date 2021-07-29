# src/cmake/create_package.cmake
#
# Copyright (c) 2008-2011, Paul Scherrer Institute (PSI)
#
# This file is part of SASfit.
#
# SASfit is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# SASfit is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with SASfit.  If not, see <http://www.gnu.org/licenses/>.
#
# Author(s) of this file:
#   Ingo Bressler (ingo.bressler@bam.de)
#
# This cmake script will copy all required files in a separate directory
# where it can be easily zipped or whatever
# following variables have to be set (e.g. via 'cmake -D')
# SASFIT_ROOT_DIR  - the root directory of sasfit where 
#                    the subdirectories 'src' and 'bin' live in
# SASFIT_PCKG_DIR  - the package directory, the root of the new sasfit-tree,
#                    'bin' and 'src' will be copied here to
# SASFIT_LIST_FILE - full filename (incl. path) to the file which contains
#                  - the list of files to copy

cmake_minimum_required(VERSION 3.0)

set(CMAKE_MODULE_PATH ${SASFIT_ROOT_DIR}/src/cmake)
include(SasfitCmakeUtils)
#message("package dir: ${SASFIT_PCKG_DIR}")
#message("root dir: ${SASFIT_ROOT_DIR}")

if(NOT DEFINED SASFIT_VERSION)
    message(STATUS "SASfit Version is not set !")
endif(NOT DEFINED SASFIT_VERSION)

# build the name of the package path (for distribution)
set(PCKG_DIR_NAME sasfit_${SASFIT_VERSION}_${SASFIT_PCKG_DIR_POSTFIX})
set(SASFIT_PCKG_DIR ${SASFIT_ROOT_DIR}/../${PCKG_DIR_NAME})
get_filename_component(SASFIT_PCKG_DIR "${SASFIT_PCKG_DIR}" REALPATH) 

# remove previously created package directory (if the same as present) and create a new one
if(EXISTS ${SASFIT_PCKG_DIR})
    file(REMOVE_RECURSE ${SASFIT_PCKG_DIR})
endif(EXISTS ${SASFIT_PCKG_DIR})

file(MAKE_DIRECTORY ${SASFIT_PCKG_DIR})
message(STATUS "Created directory '${SASFIT_PCKG_DIR}'.")

# get the list of filenames to copy
file(READ ${SASFIT_LIST_FILE} SASFIT_FILE_LIST)

# copy each file from the list to its destination
foreach(REL_FILENAME ${SASFIT_FILE_LIST})
#    message("'${REL_FILENAME}'")
    set(SRC_FILENAME "${REL_FILENAME}")
    if(NOT EXISTS "${SRC_FILENAME}")
        set(SRC_FILENAME ${SASFIT_ROOT_DIR}/${REL_FILENAME})
    else() # exists, is absolute path
        get_filename_component(REL_FILENAME "${SRC_FILENAME}" NAME)
        # check for system lib, place in sub dir 'lib'
        if("${SRC_FILENAME}" MATCHES "/lib/")
            set(REL_FILENAME "lib/${REL_FILENAME}")
        endif()
    endif()
    if(EXISTS ${SRC_FILENAME})
        get_filename_component(subdir "${REL_FILENAME}" DIRECTORY)
        message("copying '${SRC_FILENAME}' -> '${SASFIT_PCKG_DIR}/${subdir}'")
        file(COPY ${SRC_FILENAME} DESTINATION ${SASFIT_PCKG_DIR}/${subdir} FOLLOW_SYMLINK_CHAIN)
        if(IS_MACOS AND REL_FILENAME STREQUAL "sasfit.sh")
			# SASfit startup script needs to end with .command on macOS
			file(RENAME "${SASFIT_PCKG_DIR}/${REL_FILENAME}" "${SASFIT_PCKG_DIR}/sasfit.command")
        endif() # sasfit.sh
    else()
        message("Could not copy file: ${SRC_FILENAME}")
    endif()
endforeach()

# build zip/tar archive
set(CPACK_GENERATOR TBZ2)
if(WIN32)
    set(CPACK_GENERATOR ZIP)
endif()
set(CPackConfigPattern "\\\"[^\\\"]+\\\"")
execute_process(COMMAND ${CMAKE_COMMAND} -D FILENAME=${CMAKE_MODULE_PATH}/CPackConfig.cmake
    -D "PATTERN=CPACK_PACKAGE_FILE_NAME ${CPackConfigPattern}"
    -D "REPLACEMENT=CPACK_PACKAGE_FILE_NAME \"${PCKG_DIR_NAME}\""
    -P ${CMAKE_MODULE_PATH}/replace_text_in_file.cmake)
execute_process(COMMAND ${CMAKE_COMMAND} -D FILENAME=${CMAKE_MODULE_PATH}/CPackConfig.cmake
    -D "PATTERN=CPACK_CMAKE_GENERATOR ${CPackConfigPattern}"
    -D "REPLACEMENT=CPACK_CMAKE_GENERATOR \"${CM_GEN}\""
    -P ${CMAKE_MODULE_PATH}/replace_text_in_file.cmake)
execute_process(COMMAND ${CMAKE_COMMAND} -D FILENAME=${CMAKE_MODULE_PATH}/CPackConfig.cmake
    -D "PATTERN=CPACK_INSTALLED_DIRECTORIES ${CPackConfigPattern}"
    -D "REPLACEMENT=CPACK_INSTALLED_DIRECTORIES \"${SASFIT_PCKG_DIR};.\""
    -P ${CMAKE_MODULE_PATH}/replace_text_in_file.cmake)
execute_process(COMMAND ${CMAKE_COMMAND} -D FILENAME=${CMAKE_MODULE_PATH}/CPackConfig.cmake
    -D "PATTERN=CPACK_GENERATOR ${CPackConfigPattern}"
    -D "REPLACEMENT=CPACK_GENERATOR \"${CPACK_GENERATOR}\""
    -P ${CMAKE_MODULE_PATH}/replace_text_in_file.cmake)

execute_process(COMMAND cpack
        --config "cmake/CPackConfig.cmake"
    WORKING_DIRECTORY ${SASFIT_ROOT_DIR}/src
    TIMEOUT 180.0
    RESULT_VARIABLE CPACK_RES
    OUTPUT_VARIABLE CPACK_OUT
    ERROR_VARIABLE CPACK_OUT
)
# some debug output
#message("CPACK_RES: '${CPACK_RES}'")
#message("CPACK_OUT: '${CPACK_OUT}'")
#message("CPACK_ERR: '${CPACK_ERR}'")

# Convert output to a CMake list (each list element is one line)
STRING(REGEX REPLACE ";" "\\\\;" CPACK_OUT "${CPACK_OUT}")
STRING(REGEX REPLACE "\n" ";" CPACK_OUT "${CPACK_OUT}")
foreach(line ${CPACK_OUT})
    message(STATUS ${line})
endforeach()

if(FALSE) # output contents of CPackConfig for debugging
    message(STATUS "Contents of CPackConfig:")
    execute_process(COMMAND cat "cmake/CPackConfig.cmake"
        WORKING_DIRECTORY ${SASFIT_ROOT_DIR}/src)
endif()

# restore CPackConfig if GIT is available
find_package(Git)
if(Git_FOUND)
    execute_process(COMMAND ${CMAKE_COMMAND} -E echo_append "Restoring CPackConfig: ")
    execute_process(COMMAND git checkout "cmake/CPackConfig.cmake"
                    WORKING_DIRECTORY ${SASFIT_ROOT_DIR}/src)
endif()

# check work dir of cpack process for generated package file
file(GLOB SASFIT_PACKAGE_FILE LIST_DIRECTORIES false
    "${SASFIT_ROOT_DIR}/src/${PCKG_DIR_NAME}*")
appveyor_set_var(SASFIT_PACKAGE_FILE "${SASFIT_PACKAGE_FILE}")

# vim: set ts=4 sw=4 sts=4 tw=0:
