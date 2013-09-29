cmake_minimum_required(VERSION 2.6.2)

get_filename_component(CURRENT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)

set(CONFIG_FILE)
set(SOURCE_DIR)
macro(get_source_dir)
    file(GLOB CONFIG_FILE ${CURRENT_DIR}/*/configure)
    if(CONFIG_FILE)
        get_filename_component(CONFIG_FILE ${CONFIG_FILE} ABSOLUTE)
        get_filename_component(SOURCE_DIR ${CONFIG_FILE} PATH ABSOLUTE)
    endif()
endmacro()

# see if the source was already extracted
get_source_dir()

# remove any existing source tree first
if(IS_DIRECTORY ${SOURCE_DIR})
    file(REMOVE_RECURSE ${SOURCE_DIR})
endif()
# extract source tree
file(GLOB PCKG_FILE ${CURRENT_DIR}/*fftw*.tar.gz)
get_filename_component(PCKG_FILE ${PCKG_FILE} NAME)
message(STATUS "Extracting FFTW package: '${PCKG_FILE}'")

execute_process(COMMAND tar xzvf ${PCKG_FILE}
                WORKING_DIRECTORY ${CURRENT_DIR})

get_source_dir(${CURRENT_DIR})
message(STATUS "Running FFTW configure script: '${CONFIG_FILE}'")

# set up options for configure script
set(CONFIG_ARGS
    --prefix ${SOURCE_DIR}
    --enable-static
    --disable-shared
    --enable-threads
    --enable-sse2
)
if(WIN32)
    list(APPEND CONFIG_ARGS
        --with-our-malloc16
        --with-windows-f77-mangling
        --with-combined-threads
    #    --enable-portable-binary
    )
endif()

message(STATUS "with options: '${CONFIG_ARGS}'")
execute_process(COMMAND sh configure ${CONFIG_ARGS}
                WORKING_DIRECTORY ${SOURCE_DIR})

message(STATUS "Building FFTW ...")
execute_process(COMMAND make install
                WORKING_DIRECTORY ${SOURCE_DIR})

# vim: set ts=4 sts=4 sw=4 tw=0:

