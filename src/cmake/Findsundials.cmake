# src/cmake/Findsundials.cmake
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
#   Ingo Bressler (dev@ingobressler.net) 2008-2013
#
# Try to find the sundials library
#
# Once run this will define: 
# 
# sundials_FOUND       = system has sundials lib
#
# sundials_LIBRARIES   = full path to the libraries
# 
# sundials_INCLUDE_DIRS     = where to find headers 
#
# sundials_LINK_DIRECTORIES = link directories, useful for rpath on Unix
# sundials_EXE_LINKER_FLAGS = rpath on Unix

set(sundials_FOUND FALSE) # init result variable
get_package_dir(SUNDIALS ${SASFIT_ROOT_DIR}/src/sundials)

message(STATUS "[${CMAKE_CURRENT_LIST_FILE}:${CMAKE_CURRENT_LIST_LINE}] SOURCE_DIR: '${SOURCE_DIR}'")
if(NOT SOURCE_DIR)
    message(STATUS "done here")
    return()
endif()
set(sundials_INCLUDE_DIRS ${SOURCE_DIR}/include)

# search build directory
file(GLOB sundials_BUILD_DIRS ${SOURCE_DIR}/*/CMakeCache.txt)
list(LENGTH sundials_BUILD_DIRS NUM_DIRS)
if(NUM_DIRS GREATER 0)
        list(GET sundials_BUILD_DIRS 0 sundials_BUILD_DIR)
        get_filename_component(sundials_BUILD_DIR
                               "${sundials_BUILD_DIR}" DIRECTORY)
#        message("sundials_BUILD_DIR: '${sundials_BUILD_DIR}'")
        list(APPEND sundials_INCLUDE_DIRS ${sundials_BUILD_DIR}/include)
endif()
message(STATUS "DBG08")

file(GLOB sundials_STATIC_LIBS ${sundials_BUILD_DIR}/src/*/*.a)
message(STATUS "DBG09")

if(sundials_STATIC_LIBS)
        set(sundials_LIBRARIES ${sundials_STATIC_LIBS})
endif()

message(STATUS "DBG10")
list(LENGTH sundials_INCLUDE_DIRS NUM_INC_DIRS)
if(NUM_INC_DIRS EQUAL 2 AND sundials_LIBRARIES)
        set(sundials_FOUND TRUE)
endif()

