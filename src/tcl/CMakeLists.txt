# src/tcl/CMakeLists.txt
#
# Author(s) of this file:
#   Ingo Bressler (dev@ingobressler.net)

# extracts and builds the included TCL package

cmake_minimum_required(VERSION 3.0)

# breaks Tcl build (esp. configure) with gcc-14 otherwise
set(ENV{CFLAGS} -Wno-error=implicit-int)
# disable some warnings for apples clang compiler
set(ENV{CFLAGS} -Wno-deprecated-non-prototype -Wno-implicit-function-declaration)

# set up options for configure script
set(CONFIGURE_OPTIONS
    --enable-static
    --disable-shared
)
if(SYSTEM_IS_64)
    list(APPEND CONFIGURE_OPTIONS --enable-64bit)
    # required for static builds on 64bit Linux
    if(UNIX AND NOT WIN32)
        list(APPEND CONFIGURE_OPTIONS --with-pic)
    endif()
endif()

get_filename_component(CURRENT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)

# see if the source was already extracted
build_from_source(${CURRENT_DIR} ${CONFIGURE_OPTIONS})

# vim: set ts=4 sts=4 sw=4 tw=0:
