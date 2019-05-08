# src/cmake/Findsasfit_ff.cmake
#
# Copyright (c) 2008-2013, Paul Scherrer Institute (PSI)
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
#   Joachim Kohlbrecherr (joachim.kohlbrecher@psi.ch)

# Find the sasfit_oz includes and library
#
# sasfit_oz_INCLUDE_DIRS - where to find the touch lib header files
# sasfit_oz_LIBRARIES    - List of fully qualified libraries to link 
#                              against.
# sasfit_oz_FOUND        - Do not attempt to use if "no" or undefined.
  
GET_FILENAME_COMPONENT(CURRENT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
GET_FILENAME_COMPONENT(CURRENT_DIR "${CURRENT_DIR}/.." ABSOLUTE)

FIND_PATH(sasfit_oz_INCLUDE_DIR sasfit_oz.h
    ${CURRENT_DIR}/sasfit_oz/include/
)
  
FIND_LIBRARY(sasfit_oz_LIBRARY sasfit_oz
    ${CURRENT_DIR}/sasfit_oz/lib/
    ${CURRENT_DIR}/sasfit_oz/lib/debug/
    ${CURRENT_DIR}/sasfit_oz/lib/release/
)

IF(sasfit_ff_INCLUDE_DIR)
    SET(sasfit_oz_FOUND "YES")
    SET(sasfit_oz_INCLUDE_DIRS ${sasfit_oz_INCLUDE_DIR})
  IF(sasfit_oz_LIBRARY)
    SET(sasfit_oz_LIBRARIES ${sasfit_oz_LIBRARY})
  ELSE(sasfit_oz_LIBRARY)
    MESSAGE(STATUS "sasfit_oz library not found!")
  ENDIF(sasfit_oz_LIBRARY)
ELSE(sasfit_oz_INCLUDE_DIR)
  MESSAGE(STATUS "sasfit_oz include dir not found!")
ENDIF(sasfit_oz_INCLUDE_DIR)

MESSAGE(STATUS "found sasfit_oz inc: ${sasfit_oz_INCLUDE_DIR}")
MESSAGE(STATUS "found sasfit_oz lib: ${sasfit_oz_LIBRARY}")

IF(NOT sasfit_oz_FOUND)
    MESSAGE(SEND_ERROR "The sasfit_oz library was not found. For adjustments use ${CMAKE_CURRENT_LIST_FILE}.")
ENDIF(NOT sasfit_oz_FOUND)

