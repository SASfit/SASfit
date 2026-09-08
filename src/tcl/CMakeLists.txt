# src/tcl/CMakeLists.txt
#
# Author(s) of this file:
#   Ingo Bressler (dev@ingobressler.net)

# extracts and builds the included TCL package

# cmake_minimum_required(VERSION 3.0)

# set up options for configure script
# CFLAGS=-std=gnu17: GCC 15 switched its default C dialect to gnu23, under
# which an empty parameter list "()" in a function declaration now means
# "takes zero arguments" instead of the classic K&R "unspecified
# arguments" meaning that C89-era code like this Tcl 8.4.20 source (and
# autoconf's own configure-time compile checks) relies on throughout, e.g.
# tclUnixPort.h's "extern double strtod();" conflicting with the real
# prototype once the -Dstrtod=fixstrtod substitution is applied. This
# joins the existing implicit-int.patch/fix-incompat-func-ptr.patch as
# another instance of a newer GCC default breaking this old source; -std=
# reverts the dialect for the whole build (configure, and the make it
# invokes) rather than patching the many individual K&R declarations.
set(CONFIGURE_OPTIONS
    CFLAGS=-std=gnu17
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
