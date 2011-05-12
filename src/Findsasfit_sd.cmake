# src/Findsasfit_sd.cmake
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

# Find the sasfit_sd includes and library
#
# sasfit_sd_INCLUDE_DIRS - where to find the touch lib header files
# sasfit_sd_LIBRARIES    - List of fully qualified libraries to link 
#                              against.
# sasfit_sd_FOUND        - Do not attempt to use if "no" or undefined.
  
GET_FILENAME_COMPONENT(CURRENT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)

FIND_PATH(sasfit_sd_INCLUDE_DIR sasfit_sd.h
    ${CURRENT_DIR}/sasfit_sd/include/
)
  
FIND_LIBRARY(sasfit_sd_LIBRARY sasfit_sd
    ${CURRENT_DIR}/sasfit_sd/lib/
    ${CURRENT_DIR}/sasfit_sd/lib/debug/
    ${CURRENT_DIR}/sasfit_sd/lib/release/
)

IF(sasfit_sd_INCLUDE_DIR)
    SET(sasfit_sd_FOUND "YES")
    SET(sasfit_sd_INCLUDE_DIRS ${sasfit_sd_INCLUDE_DIR})
  IF(sasfit_sd_LIBRARY)
    SET(sasfit_sd_LIBRARIES ${sasfit_sd_LIBRARY})
  ELSE(sasfit_sd_LIBRARY)
    MESSAGE(STATUS "sasfit_sd library not found!")
  ENDIF(sasfit_sd_LIBRARY)
ELSE(sasfit_sd_INCLUDE_DIR)
  MESSAGE(STATUS "sasfit_sd include dir not found!")
ENDIF(sasfit_sd_INCLUDE_DIR)

MESSAGE(STATUS "found sasfit_sd inc: ${sasfit_sd_INCLUDE_DIR}")

IF(NOT sasfit_sd_FOUND)
    MESSAGE(SEND_ERROR "The sasfit_sd library was not found. For adjustments use ${CMAKE_CURRENT_LIST_FILE}.")
ENDIF(NOT sasfit_sd_FOUND)

