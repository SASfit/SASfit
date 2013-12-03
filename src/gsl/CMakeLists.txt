# src/gsl/CMakeLists.txt
#
# Author(s) of this file:
#   Ingo Bressler (ingo.bressler@bam.de)

# extracts and builds the included GSL package

cmake_minimum_required(VERSION 2.6.2)

# set up options for configure script
set(CONFIGURE_OPTIONS
    --enable-static
    --disable-shared
)

# required for static builds on 64bit Linux
if(UNIX AND SYSTEM_IS_64)
    list(APPEND CONFIGURE_OPTIONS --with-pic)
endif()

get_filename_component(CURRENT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)

# see if the source was already extracted
build_from_source(${CURRENT_DIR} ${CONFIGURE_OPTIONS})

# vim: set ts=4 sts=4 sw=4 tw=0:
