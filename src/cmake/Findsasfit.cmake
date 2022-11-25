# src/cmake/Findsasfit.cmake
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

