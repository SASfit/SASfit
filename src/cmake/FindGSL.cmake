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
# Try to find gnu scientific library GSL
# See 
# http://www.gnu.org/software/gsl/  and 
# http://gnuwin32.sourceforge.net/packages/gsl.htm
#
# Once run this will define: 
# 
# GSL_FOUND       = system has GSL lib
#
# GSL_LIBRARIES   = full path to the libraries
#    on Unix/Linux with additional linker flags from "gsl-config --libs"
# 
# CMAKE_GSL_CXX_FLAGS  = Unix compiler flags for GSL, essentially "`gsl-config --cxxflags`"
#
# GSL_INCLUDE_DIRS     = where to find headers 
#
# GSL_LINK_DIRECTORIES = link directories, useful for rpath on Unix
# GSL_EXE_LINKER_FLAGS = rpath on Unix

# JW tested with gsl-1.8, Windows XP, MSVS 7.1, MSVS 8.0
SET(GSL_POSSIBLE_ROOT_DIRS
#    ${GSL_ROOT_DIR}
#    $ENV{GSL_ROOT_DIR}
#    ${GSL_DIR}
#    ${GSL_HOME}    
#    $ENV{GSL_DIR}
#    $ENV{GSL_HOME}
#    $ENV{EXTERN_LIBS_DIR}/gsl
#    $ENV{EXTRA}
)
SET(WIN32_PROGRAMFILES "$ENV{PROGRAMFILES}")
IF(WIN32_PROGRAMFILES)
LIST(APPEND GSL_POSSIBLE_ROOT_DIRS "${WIN32_PROGRAMFILES}\\GnuWin32\\")
ENDIF(WIN32_PROGRAMFILES)

# search gsl-config program
SET(GSL_CONFIG_PREFER_PATH "" CACHE STRING "preferred path to GSL (gsl-config)")
if(NOT WIN32)
	STRING(REPLACE ":" ";" GSL_CONFIG_PREFER_PATH "$ENV{PATH}")
else(NOT WIN32)
	SET(GSL_CONFIG_PREFER_PATH "$ENV{PATH}")
endif(NOT WIN32)
LIST(APPEND GSL_CONFIG_PREFER_PATH
#      "$ENV{GSL_DIR}/bin"
#      "$ENV{GSL_DIR}"
#      "$ENV{GSL_HOME}/bin" 
#      "$ENV{GSL_HOME}" 
)
#  message("DBG gsl prefer path: ${GSL_CONFIG_PREFER_PATH} ${LEN}")
unset(GSL_CONFIG)
unset(GSL_CONFIG CACHE)
FIND_PROGRAM(GSL_CONFIG gsl-config
	${GSL_CONFIG_PREFER_PATH}
)
#  MESSAGE("DBG GSL_CONFIG ${GSL_CONFIG}")

set(ARGS "")
list(APPEND ARGS "--prefix")

# add directory candidate to possible root dirs
IF(GSL_CONFIG)
	if(WIN32)
		list(INSERT ARGS 0 ${GSL_CONFIG})
		set(INTERP bash.exe)
		FIND_PROGRAM(GSL_CONFIG_CMD ${INTERP} ENV PATH)
		#string(LENGTH ${INTERP} LEN)
		#string(REPLACE ${INTERP} "" ${GSL_CONFIG})
	else(WIN32)
		set(GSL_CONFIG_CMD ${GSL_CONFIG})
	endif(WIN32)
#    MESSAGE("DBG GSL using gsl-config ${GSL_CONFIG_CMD} '${ARGS}'")
	# set CXXFLAGS to be fed into CXX_FLAGS by the user:
	execute_process(COMMAND ${GSL_CONFIG_CMD} ${ARGS}
		OUTPUT_VARIABLE GSL_PREFIX
	)
#    MESSAGE("DBG GSL_PREFIX: '${GSL_PREFIX}'")
	string(STRIP ${GSL_PREFIX} GSL_PREFIX)
	if(WIN32)
		string(REPLACE "/usr" "c:/msys" GSL_PREFIX "${GSL_PREFIX}")
	endif(WIN32)
#    MESSAGE("DBG GSL_PREFIX: '${GSL_PREFIX}'")
	IF(EXISTS ${GSL_PREFIX})
		#LIST(APPEND GSL_POSSIBLE_ROOT_DIRS ${GSL_PREFIX})
		SET(GSL_POSSIBLE_ROOT_DIRS ${GSL_PREFIX})
	ELSE(EXISTS ${GSL_PREFIX})
		get_filename_component(TEMP_PATH ${GSL_CONFIG} PATH)
		get_filename_component(TEMP_PATH ${TEMP_PATH} PATH)
		IF(EXISTS ${TEMP_PATH})
			SET(GSL_POSSIBLE_ROOT_DIRS ${TEMP_PATH}/)
		ENDIF(EXISTS ${TEMP_PATH})
	ENDIF(EXISTS ${GSL_PREFIX})
ENDIF(GSL_CONFIG)

set(SEARCH_OPTIONS "")
if(DEFINED GSL_PATH)
	if(EXISTS ${GSL_PATH} AND IS_DIRECTORY ${GSL_PATH})
		set(GSL_POSSIBLE_ROOT_DIRS ${GSL_PATH})
		set(SEARCH_OPTIONS NO_DEFAULT_PATH)
	endif(EXISTS ${GSL_PATH} AND IS_DIRECTORY ${GSL_PATH})
endif(DEFINED GSL_PATH)
message(STATUS "SEARCH_OPTIONS= ${SEARCH_OPTIONS}")
#MESSAGE("DBG possible root dirs: '${GSL_POSSIBLE_ROOT_DIRS}'")

FIND_PATH(GSL_INCLUDE_DIR
	NAMES gsl/gsl_cdf.h gsl/gsl_randist.h
	PATHS ${GSL_POSSIBLE_ROOT_DIRS}
	PATH_SUFFIXES include
	${SEARCH_OPTIONS}
	DOC "GSL header include dir"
)
#MESSAGE(STATUS "GSL_INCLUDE_DIR: ${GSL_INCLUDE_DIR}")

FIND_LIBRARY(GSL_GSL_LIBRARY
	NAMES gsl libgsl
	PATHS  ${GSL_POSSIBLE_ROOT_DIRS}
	PATH_SUFFIXES lib .libs
	${SEARCH_OPTIONS}
	DOC "GSL library dir"
)

FIND_FILE(GSL_GSL_STATIC_LIBRARY
	NAMES gsl.a libgsl.a
	PATHS  ${GSL_POSSIBLE_ROOT_DIRS}
	PATH_SUFFIXES lib .libs
	${SEARCH_OPTIONS}
	DOC "GSL static library dir"
)

IF(NOT GSL_GSL_LIBRARY)
	SET(GSL_GSL_LIBRARY ${GSL_GSL_STATIC_LIBRARY})
ENDIF(NOT GSL_GSL_LIBRARY)

FIND_LIBRARY(GSL_GSLCBLAS_LIBRARY
	NAMES gslcblas libgslcblas
	PATHS  ${GSL_POSSIBLE_ROOT_DIRS}
	PATH_SUFFIXES lib cblas/.libs
	${SEARCH_OPTIONS}
	DOC "GSL cblas library dir"
)

FIND_FILE(GSL_GSLCBLAS_STATIC_LIBRARY
	NAMES gslcblas.a libgslcblas.a
	PATHS  ${GSL_POSSIBLE_ROOT_DIRS}
	PATH_SUFFIXES lib cblas/.libs
	${SEARCH_OPTIONS}
	DOC "GSL cblas static library dir"
)
	
IF(NOT GSL_GSLCBLAS_LIBRARY)
	SET(GSL_GSLCBLAS_LIBRARY ${GSL_GSLCBLAS_STATIC_LIBRARY})
ENDIF(NOT GSL_GSLCBLAS_LIBRARY)

SET(GSL_LIBRARIES
  ${GSL_GSL_LIBRARY}
  ${GSL_GSLCBLAS_LIBRARY}
)
SET(GSL_STATIC_LIBRARIES 
  ${GSL_GSL_STATIC_LIBRARY}
  ${GSL_GSLCBLAS_STATIC_LIBRARY}
)
SET(GSL_INCLUDE_DIRS ${GSL_INCLUDE_DIR})

MESSAGE(STATUS "GSL_INCLUDE_DIRS=    ${GSL_INCLUDE_DIRS}")
MESSAGE(STATUS "GSL_STATIC_LIBRARIES=${GSL_STATIC_LIBRARIES}")
MESSAGE(STATUS "GSL_LIBRARIES=       ${GSL_LIBRARIES}")

IF(GSL_LIBRARIES)
  IF(GSL_INCLUDE_DIRS OR GSL_CXX_FLAGS)

    SET(GSL_FOUND 1)
    
  ENDIF(GSL_INCLUDE_DIRS OR GSL_CXX_FLAGS)
ENDIF(GSL_LIBRARIES)


# ==========================================
IF(NOT GSL_FOUND)
  # make FIND_PACKAGE friendly
  IF(NOT GSL_FIND_QUIETLY)
    IF(GSL_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "GSL required, please specify it's location. On Windows you may want to install GSL for Windows (http://sourceforge.net/projects/mingw-cross/files/%5BLIB%5D%20GSL/).")
    ELSE(GSL_FIND_REQUIRED)
      MESSAGE(STATUS       "ERROR: GSL was not found.")
    ENDIF(GSL_FIND_REQUIRED)
  ENDIF(NOT GSL_FIND_QUIETLY)
ENDIF(NOT GSL_FOUND)

# vim: set ts=2 sw=2 tw=0:
