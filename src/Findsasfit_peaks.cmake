# src/Findsasfit_peaks.cmake
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

# Find the sasfit_peaks includes and library
#
# sasfit_peaks_INCLUDE_DIRS - where to find the touch lib header files
# sasfit_peaks_LIBRARIES    - List of fully qualified libraries to link 
#                              against.
# sasfit_peaks_FOUND        - Do not attempt to use if "no" or undefined.
  
GET_FILENAME_COMPONENT(CURRENT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)

FIND_PATH(sasfit_peaks_INCLUDE_DIR sasfit_peaks.h
    ${CURRENT_DIR}/sasfit_peaks/include/
)
  
FIND_LIBRARY(sasfit_peaks_LIBRARY sasfit_peaks
    ${CURRENT_DIR}/sasfit_peaks/lib/
    ${CURRENT_DIR}/sasfit_peaks/lib/debug/
    ${CURRENT_DIR}/sasfit_peaks/lib/release/
)

IF(sasfit_peaks_INCLUDE_DIR)
    SET(sasfit_peaks_FOUND "YES")
    SET(sasfit_peaks_INCLUDE_DIRS ${sasfit_peaks_INCLUDE_DIR})
  IF(sasfit_peaks_LIBRARY)
    SET(sasfit_peaks_LIBRARIES ${sasfit_peaks_LIBRARY})
  ELSE(sasfit_peaks_LIBRARY)
    MESSAGE(STATUS "sasfit_peaks library not found!")
  ENDIF(sasfit_peaks_LIBRARY)
ELSE(sasfit_peaks_INCLUDE_DIR)
  MESSAGE(STATUS "sasfit_peaks include dir not found!")
ENDIF(sasfit_peaks_INCLUDE_DIR)

MESSAGE(STATUS "found sasfit_peaks inc: ${sasfit_peaks_INCLUDE_DIR}")
MESSAGE(STATUS "found sasfit_peaks lib: ${sasfit_peaks_LIBRARY}")

IF(NOT sasfit_peaks_FOUND)
    MESSAGE(SEND_ERROR "The sasfit_peaks library was not found. For adjustments use ${CMAKE_CURRENT_LIST_FILE}.")
ENDIF(NOT sasfit_peaks_FOUND)
