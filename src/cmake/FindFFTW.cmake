# src/cmake/FindFFTW.cmake
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
#   Felix Woelk 07/2004
#   Jan Woetzel (www.mip.informatik.uni-kiel.de)
#   Ingo Bressler (dev@ingobressler.net) 2008-2013
#
# Try to find FFTW library
#
# Once run this will define: 
# 
# FFTW_FOUND       = system has FFTW lib
#
# FFTW_LIBRARIES   = full path to the libraries
# 
# FFTW_INCLUDE_DIRS     = where to find headers 
#
# FFTW_LINK_DIRECTORIES = link directories, useful for rpath on Unix
# FFTW_EXE_LINKER_FLAGS = rpath on Unix

get_package_dir(FFTW ${SASFIT_ROOT_DIR}/src/fftw)

# file include dir of custom build library
FIND_PATH(FFTW_INCLUDE_DIRS
        NAMES fftw3.h
        PATHS ${SOURCE_DIR}
        PATH_SUFFIXES include
        NO_DEFAULT_PATH
        DOC "FFTW header include dir"
)

# get library dirs
FIND_FILE(FFTW_STATIC_LIB
        NAMES libfftw3.a
        PATHS ${SOURCE_DIR}
        PATH_SUFFIXES lib
        NO_DEFAULT_PATH
        DOC "FFTW static library"
)

if(FFTW_STATIC_LIB)
        set(FFTW_LIBRARIES ${FFTW_STATIC_LIB})
endif()

set(FFTW_FOUND FALSE)
if(IS_DIRECTORY ${FFTW_INCLUDE_DIRS} AND FFTW_LIBRARIES)
    set(FFTW_FOUND TRUE)
endif()

