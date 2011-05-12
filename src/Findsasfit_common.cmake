# src/Findsasfit_common.cmake
#
# Copyright (c) 2008-2009, Paul Scherrer Institute (PSI)
#
# This file is part of SASfit.
#
# SASfit is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# SASfit is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with SASfit.  If not, see <http://www.gnu.org/licenses/>.

# Author(s) of this file:
#   Ingo Bressler (ingo@cs.tu-berlin.de)

# Find the sasfit_common includes and library
#
# sasfit_common_INCLUDE_DIRS - where to find the touch lib header files
# sasfit_common_LIBRARIES    - List of fully qualified libraries to link 
#                              against.
# sasfit_common_FOUND        - Do not attempt to use if "no" or undefined.
  
GET_FILENAME_COMPONENT(CURRENT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)

FIND_PATH(sasfit_common_INCLUDE_DIR sasfit_common.h
    ${CURRENT_DIR}/sasfit_common/include/
)
  
FIND_LIBRARY(sasfit_common_LIBRARY sasfit_common
    ${CURRENT_DIR}/sasfit_common/lib/
    ${CURRENT_DIR}/sasfit_common/lib/debug/
    ${CURRENT_DIR}/sasfit_common/lib/release/
)

IF(sasfit_common_INCLUDE_DIR)
    SET(sasfit_common_FOUND "YES")
    SET(sasfit_common_INCLUDE_DIRS ${sasfit_common_INCLUDE_DIR})
  IF(sasfit_common_LIBRARY)
    SET(sasfit_common_LIBRARIES ${sasfit_common_LIBRARY})
  ELSE(sasfit_common_LIBRARY)
    MESSAGE(STATUS "sasfit_common library not found!")
  ENDIF(sasfit_common_LIBRARY)
ELSE(sasfit_common_INCLUDE_DIR)
  MESSAGE(STATUS "sasfit_common include dir not found!")
ENDIF(sasfit_common_INCLUDE_DIR)

MESSAGE(STATUS "found sasfit_common inc: ${sasfit_common_INCLUDE_DIRS}")

IF(NOT sasfit_common_FOUND)
    MESSAGE(SEND_ERROR "The sasfit_common library was not found. For adjustments use ${CMAKE_CURRENT_LIST_FILE}.")
ENDIF(NOT sasfit_common_FOUND)

