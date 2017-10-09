# src/cmake/create_starpack.cmake
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
# This cmake script will create a starpack (standalone executable)
# based on the virtual filesystem in <root>/sasfit.vfs
# using the precompiled saskit binaries in <root>/saskit
#
# The top-level CMakeLists.txt in <root>/src/ will finally create a package
# directory structure for distribution
#
# SASFIT_ROOT_DIR  - the root directory of sasfit where 
#                    the subdirectories 'src' and 'bin' live in
# SASFIT_PCKG_DIR  - the package directory, the root of the new sasfit-tree,
#                    'bin' and 'src' will be copied here to
# SASFIT_LIST_FILE - full filename (incl. path) to the file which contains
#                  - the list of files to copy

cmake_minimum_required(VERSION 2.6.2)

set(SASKIT_PATH ${SASFIT_ROOT_DIR}/saskit)
set(SASKIT_FILE ${SASKIT_PATH}/${SASKIT_FILENAME})
set(SASFIT_EXEC ${SASFIT_ROOT_DIR}/${SASFIT_EXEC_FILENAME})

message(STATUS "Creating Starpack ...")

if(NOT EXISTS "${SASKIT_FILE}")
        return()
endif()
message(STATUS "Using ${SASKIT_FILE}")
if(NOT WIN32)
        execute_process(COMMAND chmod u+x,g+x,o+x ${SASKIT_FILE})
endif(NOT WIN32)
file(REMOVE ${SASFIT_EXEC})
configure_file(${SASKIT_FILE} ${SASKIT_FILE}_rt COPYONLY)
execute_process(COMMAND ${SASKIT_FILE} "${SASKIT_PATH}/sdx.kit" wrap ${SASFIT_EXEC_FILENAME} -runtime ${SASKIT_FILE}_rt
                WORKING_DIRECTORY ${SASFIT_ROOT_DIR}
                TIMEOUT 80.0
                RESULT_VARIABLE SASKIT_RES
                OUTPUT_VARIABLE SASKIT_OUT
                ERROR_VARIABLE SASKIT_OUT
)
if(SASKIT_RES EQUAL 0 OR SASKIT_RES STREQUAL "Process terminated due to timeout")
        message(STATUS "Successfully created executable '${SASFIT_EXEC}'")
else(SASKIT_RES EQUAL 0 OR SASKIT_RES STREQUAL "Process terminated due to timeout")
        message(STATUS "An error occured while creating executable '${SASFIT_EXEC}'!")
        message("return value: '${SASKIT_RES}'")
        message("output: '${SASKIT_OUT}'")
endif()
file(REMOVE ${SASKIT_FILE}_rt)

# vim: set ts=4 sw=4 sts=4 tw=0:
