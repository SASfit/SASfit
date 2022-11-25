# src/cmake/Findf2c.cmake
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
# Find the f2c includes and library
#
# f2c_INCLUDE_DIRS - where to find the touch lib header files
# f2c_LIBRARIES    - List of fully qualified libraries to link 
#                              against.
# f2c_FOUND        - Do not attempt to use if "no" or undefined.

GET_FILENAME_COMPONENT(CURRENT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
GET_FILENAME_COMPONENT(CURRENT_DIR "${CURRENT_DIR}/.." ABSOLUTE)

FIND_PATH(f2c_INCLUDE_DIR f2c.h
    ${CURRENT_DIR}/f2c/
    NO_DEFAULT_PATH
)
  
FIND_LIBRARY(f2c_LIBRARY f2c
    ${CURRENT_DIR}/f2c/lib/
    ${CURRENT_DIR}/f2c/lib/debug/
    ${CURRENT_DIR}/f2c/lib/release/
)

SET(f2c_INCLUDE_DIRS ${f2c_INCLUDE_DIR})
SET(f2c_LIBRARIES ${f2c_LIBRARY})

IF(f2c_INCLUDE_DIR)
  SET(f2c_FOUND "YES")
  IF(NOT f2c_LIBRARY)
    MESSAGE(STATUS "f2c library not found!")
  ENDIF(NOT f2c_LIBRARY)
ELSE(f2c_INCLUDE_DIR)
  MESSAGE(STATUS "f2c include dir not found!")
ENDIF(f2c_INCLUDE_DIR)

MESSAGE(STATUS "found f2c inc: ${f2c_INCLUDE_DIRS}")

IF(NOT f2c_FOUND)
    MESSAGE(SEND_ERROR "The f2c library was not found. For adjustments use ${CMAKE_CURRENT_LIST_FILE}.")
ENDIF(NOT f2c_FOUND)

