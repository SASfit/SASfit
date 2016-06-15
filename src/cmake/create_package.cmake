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

cmake_minimum_required(VERSION 2.6.2)

include(${SASFIT_ROOT_DIR}/src/cmake/SasfitCmakeUtils.cmake)
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
message("=> ${SASFIT_PCKG_DIR}")

# get the list of filenames to copy
file(READ ${SASFIT_LIST_FILE} SASFIT_FILE_LIST)

# copy each file from the list to its destination
foreach(REL_FILENAME ${SASFIT_FILE_LIST})
#	message("'${REL_FILENAME}'")
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
#	message("processing file: '${SRC_FILENAME}' '${SASFIT_PCKG_DIR}/${FILENAME}'")
	if(EXISTS ${SRC_FILENAME})
                configure_file(${SRC_FILENAME} ${SASFIT_PCKG_DIR}/${REL_FILENAME} COPYONLY)
	else()
		message("Could not copy file: ${SRC_FILENAME}")
	endif()
endforeach()

# build zip/tar archive
set(CPackConfigPattern "\\\\\"[^\"]+\\\\\"")
replace_str_in_file(${SASFIT_ROOT_DIR}/src/cmake/CPackConfig.cmake
        "CPACK_PACKAGE_FILE_NAME ${CPackConfigPattern}"
	"CPACK_PACKAGE_FILE_NAME \"${PCKG_DIR_NAME}\""
)
replace_str_in_file(${SASFIT_ROOT_DIR}/src/cmake/CPackConfig.cmake
        "CPACK_CMAKE_GENERATOR ${CPackConfigPattern}"
	"CPACK_CMAKE_GENERATOR \"${CM_GEN}\""
)
replace_str_in_file(${SASFIT_ROOT_DIR}/src/cmake/CPackConfig.cmake
        "CPACK_INSTALLED_DIRECTORIES ${CPackConfigPattern}"
	"CPACK_INSTALLED_DIRECTORIES \"${SASFIT_PCKG_DIR}\\\\\;.\""
)
execute_process(COMMAND cpack --config "${SASFIT_ROOT_DIR}/src/cmake/CPackConfig.cmake"
		WORKING_DIRECTORY ${SASFIT_ROOT_DIR}/src
		TIMEOUT 180.0
		RESULT_VARIABLE CPACK_RES
		OUTPUT_VARIABLE CPACK_OUT
		ERROR_VARIABLE CPACK_OUT
)
# restore default values in CPackConfig.cmake; prevents changes from being tracked by git
replace_str_in_file(${SASFIT_ROOT_DIR}/src/cmake/CPackConfig.cmake
        "CPACK_PACKAGE_FILE_NAME ${CPackConfigPattern}"
	"CPACK_PACKAGE_FILE_NAME \"\""
)
replace_str_in_file(${SASFIT_ROOT_DIR}/src/cmake/CPackConfig.cmake
        "CPACK_CMAKE_GENERATOR ${CPackConfigPattern}"
	"CPACK_CMAKE_GENERATOR \"\""
)
replace_str_in_file(${SASFIT_ROOT_DIR}/src/cmake/CPackConfig.cmake
        "CPACK_INSTALLED_DIRECTORIES ${CPackConfigPattern}"
	"CPACK_INSTALLED_DIRECTORIES \"\""
)
message("${CPACK_RES}${CPACK_OUT}")

