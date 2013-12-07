# cmake/FindGSL.cmake
#
# Copyright (c) 2013 Ingo Breßler
#
# All rights reserved.
# 
# Author(s) of this file:
#   Ingo Breßler (dev@ingobressler.net)

# Find a local build of the GNU scientific library GSL
# See also:
# http://www.gnu.org/software/gsl/
#
# Once run this will define: 
# 
# GSL_FOUND       = system has GSL lib
#
# GSL_LIBRARIES   = full path to the libraries
#    on Unix/Linux with additional linker flags from "gsl-config --libs"
# 
# GSL_INCLUDE_DIRS     = where to find headers 

get_package_dir(${SASFIT_ROOT_DIR}/src/gsl)

# file include dir of custom build library
FIND_PATH(GSL_INCLUDE_DIRS
    NAMES gsl/gsl_math.h
    PATHS ${SOURCE_DIR}
    PATH_SUFFIXES include
    NO_DEFAULT_PATH
    DOC "GSL header include dir"
)

# get library dirs
FIND_LIBRARY(GSL_LIBRARY
	NAMES gsl libgsl
    PATHS ${SOURCE_DIR}
	PATH_SUFFIXES lib
    NO_DEFAULT_PATH
	DOC "GSL library dir"
)

FIND_FILE(GSL_STATIC_LIBRARY
	NAMES gsl.a libgsl.a
    PATHS ${SOURCE_DIR}
	PATH_SUFFIXES lib
    NO_DEFAULT_PATH
	DOC "GSL static library dir"
)

IF(NOT GSL_LIBRARY)
	SET(GSL_LIBRARY ${GSL_STATIC_LIBRARY})
ENDIF()

FIND_LIBRARY(GSLCBLAS_LIBRARY
	NAMES gslcblas libgslcblas
    PATHS ${SOURCE_DIR}
	PATH_SUFFIXES lib
    NO_DEFAULT_PATH
	DOC "GSL cblas library dir"
)

FIND_FILE(GSLCBLAS_STATIC_LIBRARY
	NAMES gslcblas.a libgslcblas.a
    PATHS ${SOURCE_DIR}
	PATH_SUFFIXES lib
    NO_DEFAULT_PATH
	DOC "GSL cblas static library dir"
)
	
IF(NOT GSLCBLAS_LIBRARY)
	SET(GSLCBLAS_LIBRARY ${GSLCBLAS_STATIC_LIBRARY})
ENDIF()

SET(GSL_LIBRARIES
  ${GSL_LIBRARY}
  ${GSLCBLAS_LIBRARY}
)
SET(GSL_STATIC_LIBRARIES 
  ${GSL_STATIC_LIBRARY}
  ${GSLCBLAS_STATIC_LIBRARY}
)

set(GSL_FOUND FALSE)
if(IS_DIRECTORY ${GSL_INCLUDE_DIRS} AND GSL_LIBRARIES)
    set(GSL_FOUND TRUE)
endif()
message("GSL_FOUND: '${GSL_FOUND}'")

# vim: set ts=4 sw=4 sts=4 tw=0:
