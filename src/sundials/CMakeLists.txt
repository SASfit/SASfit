# src/sundials/CMakeLists.txt
#
# Author(s) of this file:
#   Ingo Bressler (dev@ingobressler.net)

# extracts and builds the included sundials package

# cmake .. -DEXAMPLES_ENABLE=OFF -DMPI_ENABLE=OFF ; make sundials_kinsol_share

cmake_minimum_required(VERSION 3.5)

get_filename_component(CURRENT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)

# see if the source was already extracted
#build_from_source_by_cmake(${CURRENT_DIR} ${CONFIGURE_OPTIONS})
#OPTION(EXAMPLES_ENABLE OFF) # tests require python, not needed here
#OPTION(MPI_ENABLE OFF)      # mpi framework not needed
#add_subdirectory(sundials-2.6.2)
# required for static builds on 64bit Linux
if(UNIX)
    if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
        # required for static builds on 64bit
        set(COMPILE_FLAGS "${COMPILE_FLAGS} -fPIC")
    endif()
endif()

get_filename_component(CURRENT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)

set(CONFIG_OPTIONS
    -DEXAMPLES_ENABLE=OFF
    -DMPI_ENABLE=OFF
    -DBUILD_SHARED_LIBS=OFF
    -DBUILD_ARKODE=OFF
    -DBUILD_CVODE=OFF
    -DBUILD_CVODES=OFF
    -DBUILD_IDA=OFF
    -DBUILD_IDAS=OFF
    -DCMAKE_C_FLAGS=${COMPILE_FLAGS}
)
build_from_source(${CURRENT_DIR} "${CONFIG_OPTIONS}")

# vim: set ts=4 sts=4 sw=4 tw=0:
