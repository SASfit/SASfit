# src/Findsasfit_ff.cmake
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

# Find the sasfit_ff includes and library
#
# sasfit_ff_INCLUDE_DIRS - where to find the touch lib header files
# sasfit_ff_LIBRARIES    - List of fully qualified libraries to link 
#                              against.
# sasfit_ff_FOUND        - Do not attempt to use if "no" or undefined.
  
GET_FILENAME_COMPONENT(CURRENT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
GET_FILENAME_COMPONENT(CURRENT_DIR "${CURRENT_DIR}/.." ABSOLUTE)

FIND_PATH(sasfit_ff_INCLUDE_DIR sasfit_ff.h
    ${CURRENT_DIR}/sasfit_ff/include/
)
  
FIND_LIBRARY(sasfit_ff_LIBRARY sasfit_ff
    ${CURRENT_DIR}/sasfit_ff/lib/
    ${CURRENT_DIR}/sasfit_ff/lib/debug/
    ${CURRENT_DIR}/sasfit_ff/lib/release/
)

IF(sasfit_ff_INCLUDE_DIR)
    SET(sasfit_ff_FOUND "YES")
    SET(sasfit_ff_INCLUDE_DIRS ${sasfit_ff_INCLUDE_DIR})
  IF(sasfit_ff_LIBRARY)
    SET(sasfit_ff_LIBRARIES ${sasfit_ff_LIBRARY})
  ELSE(sasfit_ff_LIBRARY)
    MESSAGE(STATUS "sasfit_ff library not found!")
  ENDIF(sasfit_ff_LIBRARY)
ELSE(sasfit_ff_INCLUDE_DIR)
  MESSAGE(STATUS "sasfit_ff include dir not found!")
ENDIF(sasfit_ff_INCLUDE_DIR)

MESSAGE(STATUS "found sasfit_ff inc: ${sasfit_ff_INCLUDE_DIR}")
MESSAGE(STATUS "found sasfit_ff lib: ${sasfit_ff_LIBRARY}")

IF(NOT sasfit_ff_FOUND)
    MESSAGE(SEND_ERROR "The sasfit_ff library was not found. For adjustments use ${CMAKE_CURRENT_LIST_FILE}.")
ENDIF(NOT sasfit_ff_FOUND)

