# src/cmake/FindGSL.cmake
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
#   Ingo Bressler (ingo.bressler@bam.de) 2008-2011
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

SET(FFTW_POSSIBLE_ROOT_DIRS
    c:/mingw/msys/1.0/local/
    ${FFTW_ROOT_DIR}
    $ENV{FFTW_ROOT_DIR}
    ${FFTW_DIR}
    ${FFTW_HOME}    
    $ENV{FFTW_DIR}
    $ENV{FFTW_HOME}
    $ENV{FFTW_PATH}
    $ENV{EXTERN_LIBS_DIR}/fftw
    $ENV{EXTRA}
)
SET(WIN32_PROGRAMFILES "$ENV{PROGRAMFILES}")
IF(WIN32_PROGRAMFILES)
LIST(APPEND FFTW_POSSIBLE_ROOT_DIRS "${WIN32_PROGRAMFILES}\\FFTW\\")
ENDIF(WIN32_PROGRAMFILES)


set(SEARCH_OPTIONS "")
if(DEFINED FFTW_PATH)
	if(EXISTS ${FFTW_PATH} AND IS_DIRECTORY ${FFTW_PATH})
		set(FFTW_POSSIBLE_ROOT_DIRS ${FFTW_PATH})
		set(SEARCH_OPTIONS NO_DEFAULT_PATH)
	endif(EXISTS ${FFTW_PATH} AND IS_DIRECTORY ${FFTW_PATH})
endif(DEFINED FFTW_PATH)
message(STATUS "SEARCH_OPTIONS= ${SEARCH_OPTIONS}")
MESSAGE("DBG possible root dirs: '${FFTW_POSSIBLE_ROOT_DIRS}'")

FIND_PATH(FFTW_INCLUDE_DIR
	NAMES fftw3.h
	PATHS ${FFTW_POSSIBLE_ROOT_DIRS}
	PATH_SUFFIXES include
	${SEARCH_OPTIONS}
	DOC "FFTW header include dir"
)
MESSAGE(STATUS "FFTW_INCLUDE_DIR: ${FFTW_INCLUDE_DIR}")

FIND_LIBRARY(FFTW_FFTW_LIBRARY
	NAMES  fftw3 libfftw3
	PATHS  ${FFTW_POSSIBLE_ROOT_DIRS}
	PATH_SUFFIXES lib .libs bin
	${SEARCH_OPTIONS}
	NO_DEFAULT_PATH
	DOC "FFTW library dir"
)
FIND_FILE(FFTW_FFTW_LIBRARY
	NAMES fftw3 libfftw3
	PATHS  ${FFTW_POSSIBLE_ROOT_DIRS}
	PATH_SUFFIXES lib .libs bin
	${SEARCH_OPTIONS}
	NO_DEFAULT_PATH
	DOC "FFTW library dir"
)

FIND_FILE(FFTW_FFTW_STATIC_LIBRARY
	NAMES fftw3.a libfftw3.a 
	PATHS  ${FFTW_POSSIBLE_ROOT_DIRS}
	PATH_SUFFIXES lib .libs
	${SEARCH_OPTIONS}
	NO_DEFAULT_PATH
	DOC "FFTW static library dir"
)

IF(NOT FFTW_FFTW_LIBRARY)
	SET(FFTW_FFTW_LIBRARY ${FFTW_FFTW_STATIC_LIBRARY})
ENDIF(NOT FFTW_FFTW_LIBRARY)

SET(FFTW_LIBRARIES
  ${FFTW_FFTW_LIBRARY}
)
SET(FFTW_STATIC_LIBRARIES 
  ${FFTW_FFTW_STATIC_LIBRARY}
)
SET(FFTW_INCLUDE_DIRS ${FFTW_INCLUDE_DIR})

MESSAGE(STATUS "FFTW_INCLUDE_DIRS=    ${FFTW_INCLUDE_DIRS}")
MESSAGE(STATUS "FFTW_STATIC_LIBRARIES=${FFTW_STATIC_LIBRARIES}")
MESSAGE(STATUS "FFTW_LIBRARIES=       ${FFTW_LIBRARIES}")

IF(FFTW_LIBRARIES)
  IF(FFTW_INCLUDE_DIRS)

    SET(FFTW_FOUND 1)
    
  ENDIF(FFTW_INCLUDE_DIRS)
ENDIF(FFTW_LIBRARIES)


# ==========================================
IF(NOT FFTW_FOUND)
  # make FIND_PACKAGE friendly
  IF(NOT FFTW_FIND_QUIETLY)
    IF(FFTW_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "FFTW required, please specify it's location.")
    ELSE(FFTW_FIND_REQUIRED)
      MESSAGE(STATUS       "ERROR: FFTW was not found.")
    ENDIF(FFTW_FIND_REQUIRED)
  ENDIF(NOT FFTW_FIND_QUIETLY)
ENDIF(NOT FFTW_FOUND)
