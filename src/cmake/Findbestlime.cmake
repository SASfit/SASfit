# src/cmake/Findbestlime.cmake
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
# Try to find the bestlime library
#
# Once run this will define: 
# 
# bestlime_FOUND       = system has bestlime lib
#
# bestlime_LIBRARIES   = full path to the libraries
# 
# bestlime_INCLUDE_DIRS     = where to find headers 
#
# bestlime_LINK_DIRECTORIES = link directories, useful for rpath on Unix
# bestlime_EXE_LINKER_FLAGS = rpath on Unix

set(bestlime_FOUND FALSE) # init result variable
get_package_dir(BESTLIME ${SASFIT_ROOT_DIR}/src/bestlime)

if(NOT SOURCE_DIR) # nothing to do
    return()
endif()
set(bestlime_INCLUDE_DIRS ${SOURCE_DIR}/include)

# search build directory
file(GLOB bestlime_BUILD_DIRS ${SOURCE_DIR}/*/CMakeCache.txt)
list(LENGTH bestlime_BUILD_DIRS NUM_DIRS)
if(NUM_DIRS GREATER 0)
        list(GET bestlime_BUILD_DIRS 0 bestlime_BUILD_DIR)
        get_filename_component(bestlime_BUILD_DIR
                               "${bestlime_BUILD_DIR}" DIRECTORY)
        list(APPEND bestlime_INCLUDE_DIRS ${bestlime_BUILD_DIR}/include)
endif()

# BestLime's own CMakeLists.txt (src/bestlime/windows64/CMakeLists.txt) sets
# CMAKE_ARCHIVE_OUTPUT_DIRECTORY to "${PROJECT_BINARY_DIR}/lib", so the built
# static/import library (e.g. libbestlime.a or, for a MinGW shared build,
# the libbestlime.dll.a import library) lands directly in <build>/lib/, not
# two levels below <build>/src/ as the previous glob pattern assumed (that
# old pattern never matched anything, on any platform).
file(GLOB bestlime_STATIC_LIBS ${bestlime_BUILD_DIR}/lib/*.a)

if(NOT bestlime_STATIC_LIBS)
    # fall back to a recursive search in case the output directory is
    # ever reorganized again, so this doesn't silently break a 2nd time
    file(GLOB_RECURSE bestlime_STATIC_LIBS ${bestlime_BUILD_DIR}/*.a)
endif()

if(bestlime_STATIC_LIBS)
        set(bestlime_LIBRARIES ${bestlime_STATIC_LIBS})
endif()

list(LENGTH bestlime_INCLUDE_DIRS NUM_INC_DIRS)
if(NUM_INC_DIRS EQUAL 2 AND bestlime_LIBRARIES)
        set(bestlime_FOUND TRUE)
endif()

