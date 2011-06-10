# src/Findsasfit.cmake
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

# Find the sasfit includes and library
#
# sasfit_INCLUDE_DIRS - where to find the touch lib header files
# sasfit_LIBRARIES    - List of fully qualified libraries to link 
#                              against.
# sasfit_FOUND        - Do not attempt to use if "no" or undefined.
  
GET_FILENAME_COMPONENT(CURRENT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
GET_FILENAME_COMPONENT(CURRENT_DIR "${CURRENT_DIR}/.." ABSOLUTE)

SET(sasfit_INCLUDE_DIR
    ${CURRENT_DIR}/sasfit_common/include/
    ${CURRENT_DIR}/sasfit_sd/include/
    ${CURRENT_DIR}/sasfit_sq/include/
    ${CURRENT_DIR}/sasfit_ff/include/
    ${CURRENT_DIR}/sasfit_peaks/include/
    ${CURRENT_DIR}/sasfit_core/include/
)
  
FIND_LIBRARY(sasfit_LIBRARY sasfit
    ${CURRENT_DIR}/lib/
    ${CURRENT_DIR}/lib/debug/
    ${CURRENT_DIR}/lib/release/
)

IF(sasfit_INCLUDE_DIR)
    SET(sasfit_FOUND "YES")
    SET(sasfit_INCLUDE_DIRS ${sasfit_INCLUDE_DIR})
  IF(sasfit_LIBRARY)
    SET(sasfit_LIBRARIES ${sasfit_LIBRARY})
  ELSE(sasfit_LIBRARY)
    MESSAGE(STATUS "sasfit library not found!")
  ENDIF(sasfit_LIBRARY)
ELSE(sasfit_INCLUDE_DIR)
  MESSAGE(STATUS "sasfit include dir not found!")
ENDIF(sasfit_INCLUDE_DIR)

MESSAGE(STATUS "found sasfit inc: ${sasfit_INCLUDE_DIRS}")

IF(NOT sasfit_FOUND)
    MESSAGE(SEND_ERROR "The sasfit library was not found. For adjustments use ${CMAKE_CURRENT_LIST_FILE}.")
ENDIF(NOT sasfit_FOUND)

