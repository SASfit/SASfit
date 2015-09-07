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

message("find sundials")
get_package_dir(${SASFIT_ROOT_DIR}/src/sundials)

# file include dir of custom build library
find_path(sundials_INCLUDE_DIRS
        NAMES kinsol.h
        PATHS ${SOURCE_DIR}/include
        PATH_SUFFIXES kinsol
        NO_DEFAULT_PATH
        DOC "sundials header include dir"
)
if(IS_DIRECTORY "${sundials_INCLUDE_DIRS}")
        get_filename_component(sundials_INCLUDE_DIRS
                "${sundials_INCLUDE_DIRS}" PATH ABSOLUTE)
endif()

file(GLOB sundials_STATIC_LIBS ${SOURCE_DIR}/build/src/*/*.a)

if(sundials_STATIC_LIBS)
        set(sundials_LIBRARIES ${sundials_STATIC_LIBS})
endif()

set(sundials_FOUND FALSE)
if(IS_DIRECTORY ${sundials_INCLUDE_DIRS} AND sundials_LIBRARIES)
    set(sundials_FOUND TRUE)
endif()

