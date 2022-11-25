# src/fftw/CMakeLists.txt
#
# Author(s) of this file:
#   Ingo Bressler (dev@ingobressler.net)

# extracts and builds the included FFTW package

cmake_minimum_required(VERSION 3.0)

# set up options for configure script
set(CONFIGURE_OPTIONS
    --with-our-malloc
    --enable-static
    --disable-shared
    --disable-threads
    --enable-sse2
)
if(WIN32)
    list(APPEND CONFIGURE_OPTIONS
        --with-our-malloc16
        --with-windows-f77-mangling
    #    --with-combined-threads
    #    --enable-portable-binary
    )
endif()

# required for static builds on 64bit Linux
if(UNIX AND SYSTEM_IS_64)
    list(APPEND CONFIGURE_OPTIONS --with-pic)
endif()

get_filename_component(CURRENT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)

# see if the source was already extracted
build_from_source(${CURRENT_DIR} ${CONFIGURE_OPTIONS})

# vim: set ts=4 sts=4 sw=4 tw=0:
