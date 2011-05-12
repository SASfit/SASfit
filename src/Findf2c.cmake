# Find the f2c includes and library
#
# f2c_INCLUDE_DIRS - where to find the touch lib header files
# f2c_LIBRARIES    - List of fully qualified libraries to link 
#                              against.
# f2c_FOUND        - Do not attempt to use if "no" or undefined.

# Author(s) of this file:
#   Ingo Bressler (ingo@cs.tu-berlin.de) 12/2008
  
GET_FILENAME_COMPONENT(CURRENT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)

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

