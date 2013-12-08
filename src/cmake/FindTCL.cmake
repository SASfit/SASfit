# cmake/FindTCL.cmake
#
# Author(s) of this file:
#   Ingo Breßler (dev@ingobressler.net)

# Once run this will define: 
# 
# TCL_FOUND       = system has found the lib
# TCL_LIBRARIES   = full path to the libraries
# TCL_INCLUDE_DIRS     = where to find headers 

message("find TCL")
get_package_dir(${SASFIT_ROOT_DIR}/src/tcl)

# file include dir of custom build library
FIND_PATH(TCL_INCLUDE_DIRS
    NAMES tcl.h
    PATHS ${SOURCE_DIR}
    PATH_SUFFIXES include
    NO_DEFAULT_PATH
    DOC "TCL include dir"
)

FIND_FILE(TCL_STATIC_LIB
    NAMES libtcl8.4.a libtcl84s.a
    PATHS ${SOURCE_DIR}
    PATH_SUFFIXES lib
    NO_DEFAULT_PATH
    DOC "TCL static library"
)

FIND_FILE(TCL_STATIC_STUBS_LIB
    NAMES libtclstub8.4.a libtclstub84s.a
    PATHS ${SOURCE_DIR}
    PATH_SUFFIXES lib
    NO_DEFAULT_PATH
    DOC "TCL static stubs library"
)

if(TCL_LIBRARY OR TCL_STATIC_LIB OR TCL_STATIC_STUBS_LIB)
    SET(TCL_LIBRARIES
        ${TCL_LIBRARY}
        ${TCL_STATIC_LIB}
        ${TCL_STATIC_STUBS_LIB}
    )
endif()

set(TCL_FOUND FALSE)
if(IS_DIRECTORY "${TCL_INCLUDE_DIRS}" AND TCL_LIBRARIES)
    set(TCL_FOUND TRUE)
endif()

# vim: set ts=4 sw=4 sts=4 tw=0:
