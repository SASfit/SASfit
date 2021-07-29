cmake_minimum_required(VERSION 3.18)

# load some basic settings
get_filename_component(CURRENT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
set(CMAKE_MODULE_PATH ${CURRENT_DIR}/cmake)
include(CMakePrintHelpers)

# get our current location and set the distribution directory name
project(sasfit C)

# load custom modules for some automation
include(SasfitCmakeUtils)
include(SasfitSourceFiles)
dbg_cmake_vars()

# the version number is derived from GIT, use 'git tag' for a specific one
sasfit_update_version()

# decides which packages to build afterwards
if(NOT DEFINED CREATE_BINARY)
    set(CREATE_BINARY TRUE)
endif(NOT DEFINED CREATE_BINARY)

 #############################################################
### A target for source code documentation using doxygen    ###
 #############################################################

option(DOCS_ONLY "Build documentation only, do not build the program" OFF)

add_custom_command(OUTPUT Doxyfile
    COMMAND ${CMAKE_COMMAND} -E copy Doxyfile.template.txt Doxyfile
    COMMAND ${CMAKE_COMMAND} -D FILENAME=Doxyfile
        -D "PATTERN=\\(PROJECT_NUMBER[ \\\\t]*= ?\\)[ \\\\t]*"
        -D "REPLACEMENT=\\\\1${SASFIT_VERSION}"
        -P cmake/replace_text_in_file.cmake
    WORKING_DIRECTORY ${CURRENT_DIR}
    BYPRODUCTS Doxyfile
    DEPENDS Doxyfile.template.txt)

add_custom_target(docs
    COMMAND doxygen DEPENDS Doxyfile
    WORKING_DIRECTORY ${CURRENT_DIR}
    SOURCES Doxyfile BYPRODUCTS ${CURRENT_DIR}/../doc/html
    COMMENT "Requires *doxygen* to be installed and optionally *graphviz*.")

if(DOCS_ONLY)
    return() # skip further targets (which build trigger building stuff ...)
endif()

 #############################################################
### determine platform specific properties                  ###
### get the static list of files (platform independent tcl) ###
 #############################################################

# original file containing all required sasfit filenames WITHOUT binaries build by cmake
set(SASFIT_BIN_LIST_FILE ${CMAKE_MODULE_PATH}/create_bin_package.list)
set(SASFIT_PCKG_SCRIPT ${CMAKE_MODULE_PATH}/create_package.cmake)
set(SASFIT_STARPACK_SCRIPT ${CMAKE_MODULE_PATH}/create_starpack.cmake)

# copy the binary package file list to a temporary list file 
# (binaries built will be added later)
file(STRINGS ${SASFIT_BIN_LIST_FILE} BIN_FILE_LIST)
sasfit_add2pckg(${BIN_FILE_LIST})

# add runtime files required on Windows with Visual Studio only 
# TODO ::: (ATTENTION!, depends on Visual Studio Version, here 2005) ::: TODO
if(MSVC80)
    message(STATUS "\n   Assuming 'msvcr80.dll' and 'Microsoft.VC80.CRT.manifest' in\n   '${SASFIT_ROOT_DIR}'\n")
    sasfit_add2pckg(
        msvcr80.dll
        Microsoft.VC80.CRT.manifest
    )
endif()

if(CMAKE_HOST_APPLE)
    add_definitions(-DMACOSX)
    add_definitions(-DNO_LONGLONG_AND_NO_LONGINT)
    set(CMAKE_OSX_ARCHITECTURES ${CMAKE_SYSTEM_PROCESSOR})
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "i[0-9]86")
        # assuming os is recent enough to understand x64
        # GSL build environment does not support multiple "-arch" 
        set(CMAKE_OSX_ARCHITECTURES x86_64)
    endif(CMAKE_SYSTEM_PROCESSOR MATCHES "i[0-9]86")
    message(STATUS "Bulding on MacOS for: ${CMAKE_OSX_ARCHITECTURES}")
endif(CMAKE_HOST_APPLE)


 #####################
### OPTIONS for f2c ###
 #####################

# platform specific properties
IF(WIN32) # for Windows

    ADD_DEFINITIONS(
        -DUSE_CLOCK     # see etime_.c
        -DMSDOS
        -DNO_ONEXIT
        -DNO_FPINIT
        -DCONFLICTING_ISATTY
        -DNO_FUNCTION_MACRO
    )

    IF(MSVC80)
        # suppress warnings about usage of old non-limited string functions
        # has to be fixed later
        add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
        add_definitions(-DINTEGER_STAR_8) # see f2c/f2c.h
    ELSE(MSVC80) # NOT MSVC80
#            add_definitions(-DNO_LONGLONG_AND_NO_LONGINT)
    ENDIF(MSVC80)

ENDIF(WIN32) # for Windows

# for Unix/Linux
IF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
#    ADD_DEFINITIONS(-DINTEGER_STAR_8 -fopenmp) # see f2c/f2c.h
    ADD_DEFINITIONS(-DINTEGER_STAR_8 ) # see f2c/f2c.h
    add_definitions(-UNO_LONGLONG_AND_NO_LONGINT)
ENDIF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)


 ###############################
### OPTIONS for sasfit_common ###
 ###############################

add_definitions(
    -DSASFIT_LIB_SUFFIX="${CMAKE_SHARED_MODULE_SUFFIX}"
    -DSASFIT_LIB_PREFIX="${CMAKE_SHARED_MODULE_PREFIX}"
)


 ########################
### library definition ###
 ########################

# GSL
find_package(GSL QUIET REQUIRED)
if(NOT GSL_FOUND)
    add_subdirectory(gsl)
    find_package(GSL REQUIRED)
endif()
message(STATUS "GSL INCLUDES:  ${GSL_INCLUDE_DIRS}")
message(STATUS "GSL LIBRARIES: ${GSL_LIBRARIES}")

# TCL
find_package(TCL QUIET REQUIRED)
if(NOT TCL_FOUND)
        add_subdirectory(tcl)
        find_package(TCL REQUIRED)
endif()
message(STATUS "TCL INCLUDES:  ${TCL_INCLUDE_DIRS}")
message(STATUS "TCL LIBRARIES: ${TCL_LIBRARIES}")
message(STATUS "TCL SH: ${TCL_SH}")

# FFTW
find_package(FFTW QUIET REQUIRED)
if(NOT FFTW_FOUND)
        add_subdirectory(fftw)
        find_package(FFTW REQUIRED)
endif()
message(STATUS "FFTW INCLUDES:  ${FFTW_INCLUDE_DIRS}")
message(STATUS "FFTW LIBRARIES: ${FFTW_LIBRARIES}")

# sundials
find_package(sundials QUIET REQUIRED)
if(NOT sundials_FOUND)
        add_subdirectory(sundials)
        find_package(sundials REQUIRED)
endif()
message(STATUS "sundials INCLUDES:  ${sundials_INCLUDE_DIRS}")
message(STATUS "sundials LIBRARIES: ${sundials_LIBRARIES}")

add_definitions(
    -DUSE_TCL_STUBS
    -DUSE_TK_STUBS
)

# openMP
#find_package(OpenMP QUIET)

#if(OPENMP_FOUND)
#        message(STATUS "Enabling OpenMP support")
#        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#        message(STATUS "OpenMP_C_FLAGS: '${OpenMP_C_FLAGS}'")
#        message(STATUS "OpenMP_CXX_FLAGS: '${OpenMP_CXX_FLAGS}'")
#        message(STATUS "OpenMP_EXE_LINKER_FLAGS: '${OpenMP_EXE_LINKER_FLAGS}'")
#        set(OpenMP_LIB)
#        if(WIN32)
#            # gather required libraries
#            execute_process(COMMAND ${CMAKE_C_COMPILER} -print-file-name=libgomp.a
#                            OUTPUT_VARIABLE OpenMP_LIB)
#            string(STRIP "${OpenMP_LIB}" OpenMP_LIB)
#            get_filename_component(OpenMP_LIB "${OpenMP_LIB}" REALPATH)
#            find_library(pthread_LIB pthread)
#            list(APPEND OpenMP_LIB "${pthread_LIB}")
#        endif()
#        message(STATUS "OpenMP LIBRARIES: '${OpenMP_LIB}'")
#else()
#        message(STATUS "Disabling OpenMP support")
#endif()

# get a list of all source files and prepare include dirs
set(SRC_FILES "")
set(SF_INC_DIRS "")
foreach(SF_MODULE f2c sasfit_common sasfit_sd sasfit_sq sasfit_ff sasfit_core sasfit_old sasfit_oz sasfit_fixed_point_acc sasfit_frida)
    if(IS_DIRECTORY ${CURRENT_DIR}/${SF_MODULE})
        foreach(SRC_FILE ${SOURCE_${SF_MODULE}})
            list(APPEND SRC_FILES "${SF_MODULE}/${SRC_FILE}")
        endforeach(SRC_FILE)
        if(${SF_MODULE} MATCHES "f2c")
            list(APPEND SF_INC_DIRS "${SF_MODULE}")
        else(${SF_MODULE} MATCHES "f2c")
            list(APPEND SF_INC_DIRS "${SF_MODULE}/include")
        endif(${SF_MODULE} MATCHES "f2c")
    endif(IS_DIRECTORY ${CURRENT_DIR}/${SF_MODULE})
endforeach(SF_MODULE)

include_directories(
    ${SF_INC_DIRS}
    ${TCL_INCLUDE_DIRS}
    ${GSL_INCLUDE_DIRS}
    ${FFTW_INCLUDE_DIRS}
    ${sundials_INCLUDE_DIRS}
)

add_library(sasfit SHARED
    ${SRC_FILES}
)

target_link_libraries(sasfit
        ${TCL_LIBRARIES}
        ${GSL_STATIC_LIBRARIES}
        ${FFTW_LIBRARIES}
        ${sundials_LIBRARIES}
#        ${OpenMP_LIB}
)

# configure build flags, assumes gcc-style flags on all platforms
#sasfit_build_props_unix(sasfit "-save-temps")
set(COMPILE_FLAGS -std=gnu99)
if(UNIX)
    set(COMPILE_FLAGS "${COMPILE_FLAGS} -Wall")
endif(UNIX)
#message("DEBUG: '${DEBUG}' OPTIM: '${OPTIM}'")
if(DEBUG)
    # debug
    set(COMPILE_FLAGS "${COMPILE_FLAGS} -g")
elseif(OPTIM)
    # non-debug optimized
    set(COMPILE_FLAGS "${COMPILE_FLAGS} -march=native -O2 -pipe -fomit-frame-pointer -fforce-addr")
else(DEBUG)
    # general purpose binary
    set(COMPILE_FLAGS "${COMPILE_FLAGS} -O2 -pipe -fomit-frame-pointer -fforce-addr")
endif(DEBUG)

set_target_properties(sasfit PROPERTIES
    COMPILE_FLAGS "${COMPILE_FLAGS}"
    MACOSX_RPATH FALSE # cmake 3 compatibility
)

sasfit_copy_shared_libs(sasfit "sasfit.vfs/lib/Sasfit")

if(DEFINED WITH_STATIC)

    set(SRC_FILES)
    foreach(SF_MODULE f2c sasfit_common)
        if(IS_DIRECTORY ${CURRENT_DIR}/${SF_MODULE})
            foreach(SRC_FILE ${SOURCE_${SF_MODULE}})
                list(APPEND SRC_FILES "${SF_MODULE}/${SRC_FILE}")
            endforeach(SRC_FILE)
        endif(IS_DIRECTORY ${CURRENT_DIR}/${SF_MODULE})
    endforeach(SF_MODULE)

    add_library(sasfit_common_stat STATIC
        ${SRC_FILES}
    )

    if(UNIX)
        if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
            # required for static builds on 64bit
            set(COMPILE_FLAGS "${COMPILE_FLAGS} -fPIC")
        endif(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    endif(UNIX)
    set_target_properties(sasfit_common_stat PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS}")
    set_target_properties(sasfit_common_stat PROPERTIES COMPILE_DEFINITIONS STATIC_ENABLED)

endif(DEFINED WITH_STATIC)

 #############
### Plugins ###
 #############

# build all the plugins
add_subdirectory(plugins)

message(STATUS "The binary package will include the following plugins: ")
foreach(PLUGIN_NAME ${SASFIT_PLUGINS_INCLUDED})
    message(STATUS "* ${PLUGIN_NAME}")
endforeach(PLUGIN_NAME)

 ####################################
### create a sasfit binary package ###
 ####################################

if(CREATE_BINARY)
    # selects debug version if desired
    set(DEBUG_SUFFIX "")
    if(DEBUG)
        message(STATUS "Using a starpack with debug symbols.")
        set(DEBUG_SUFFIX "_debug")
    endif(DEBUG)

    set(SASFIT_PCKG_DIR_POSTFIX ${PLATFORM}${DEBUG_SUFFIX})
    if(NOT IS_LINUX)
        set(SASFIT_EXEC_FILENAME sasfit${CMAKE_EXECUTABLE_SUFFIX})
        sasfit_add2pckg(${SASFIT_EXEC_FILENAME})
        if(IS_MACOS)
            sasfit_add2pckg(${SASFIT_EXEC_FILENAME}.sh)
        endif()
    endif()

    # set the saskit filename, chose the debug version if required
    set(SASKIT_FILENAME "saskit_${SASFIT_PCKG_DIR_POSTFIX}${CMAKE_EXECUTABLE_SUFFIX}")
    build_saskit("${SASFIT_ROOT_DIR}" "${SASKIT_FILENAME}")
    message(STATUS "Using saskit file: '${SASKIT_FILENAME}'")
    appveyor_set_var(SASFIT_SASKIT_EXEC "${SASFIT_ROOT_DIR}/saskit/${SASKIT_FILENAME}")

    list_paths("" ${SASFIT_BIN_FILE_LIST})
    cmake_print_variables(CMAKE_BINARY_DIR)
    if(IS_LINUX)
        # add SASFIT_BIN_FILE_LIST to the included files in AppImageBuilder config
        string(JOIN "\n    - " SASFIT_BIN_FILE_LIST_FORMATTED ${SASFIT_BIN_FILE_LIST})
        file(READ ${SASFIT_ROOT_DIR}/src/AppImageBuilder.template.yml AppImageBuilderCfg)
        file(CONFIGURE OUTPUT ${CMAKE_BINARY_DIR}/AppImageBuilder.yml
            CONTENT "${AppImageBuilderCfg}"
            @ONLY NEWLINE_STYLE LF)
        # find the final AppImage file name to set an AppVeyor variable to test its existence
        set(SASFIT_EXEC_FILENAME "SASfit-${SASFIT_VERSION}-${CMAKE_SYSTEM_PROCESSOR}.AppImage")
        #file(CONFIGURE OUTPUT ${CMAKE_BINARY_DIR}/sasfit_exec_filename.cmake CONTENT "
        #    cmake_minimum_required(VERSION 3.0)
        #    set(CMAKE_MODULE_PATH \"\${SASFIT_ROOT_DIR}/src/cmake\")
        #    include(SasfitCmakeUtils)
        #    file(GLOB SASFIT_EXEC_FILENAME RELATIVE \"\${SASFIT_ROOT_DIR}\"
        #        \${SASFIT_ROOT_DIR}/*.AppImage)
        #    message(\"Found SASFIT_EXEC_FILENAME: '\${SASFIT_EXEC_FILENAME}'\")
        #    appveyor_set_var(SASFIT_EXEC \"\${SASFIT_ROOT_DIR}/\${SASFIT_EXEC_FILENAME}\")
        #    appveyor_set_var(SASFIT_PACKAGE_FILE \"\${SASFIT_ROOT_DIR}/\${SASFIT_EXEC_FILENAME}\")
        #    " @ONLY NEWLINE_STYLE LF)
        # build the AppImage at the end and set the AppImage file name
        add_custom_target(sasfit_binary ALL
            appimage-builder --recipe ${CMAKE_BINARY_DIR}/AppImageBuilder.yml --skip-tests
            #COMMAND ${CMAKE_COMMAND} -D SASFIT_ROOT_DIR:PATH=${SASFIT_ROOT_DIR}
            #    -P ${CMAKE_BINARY_DIR}/sasfit_exec_filename.cmake
            DEPENDS sasfit ${SASFIT_PLUGINS_INCLUDED}
            BYPRODUCTS ${SASFIT_ROOT_DIR}/${SASFIT_EXEC_FILENAME}
            WORKING_DIRECTORY ${SASFIT_ROOT_DIR}
            COMMENT "Creating sasfit binary ..."
            VERBATIM)
        appveyor_set_var(SASFIT_EXEC ${SASFIT_ROOT_DIR}/${SASFIT_EXEC_FILENAME})
        appveyor_set_var(SASFIT_PACKAGE_FILE ${SASFIT_ROOT_DIR}/${SASFIT_EXEC_FILENAME})
    else()
        appveyor_set_var(SASFIT_EXEC "${SASFIT_ROOT_DIR}/${SASFIT_EXEC_FILENAME}")
        get_saskit_dependencies("${SASFIT_ROOT_DIR}" "${SASKIT_FILENAME}")
        add_custom_target(sasfit_binary ALL
            ${CMAKE_COMMAND} -D SASFIT_ROOT_DIR:PATH=${SASFIT_ROOT_DIR}
                    -D SASKIT_FILENAME:STRING=${SASKIT_FILENAME}
                    -D SASFIT_EXEC_FILENAME:STRING=${SASFIT_EXEC_FILENAME}
                    -D SASFIT_DEBUG:BOOL=${DEBUG}
                    -P ${SASFIT_STARPACK_SCRIPT}
            DEPENDS sasfit ${SASFIT_PLUGINS_INCLUDED}
            COMMENT "Creating sasfit binary ..."
            VERBATIM)

        # write the list of filenames to a temporary file
        # (workaround for shell command length restrictions)
        set(SASFIT_LIST_FILE ${SASFIT_BIN_LIST_FILE}.tmp)
        file(WRITE ${SASFIT_LIST_FILE} "${SASFIT_BIN_FILE_LIST}")
        add_custom_target(sasfit_binary_package ALL
            ${CMAKE_COMMAND} -D SASFIT_ROOT_DIR:PATH=${SASFIT_ROOT_DIR}
                    -D SASFIT_PCKG_DIR_POSTFIX:STRING=${SASFIT_PCKG_DIR_POSTFIX}
                    -D SASFIT_LIST_FILE:FILEPATH=${SASFIT_LIST_FILE}
                    -D CM_GEN:STRING=${CMAKE_GENERATOR}
                    -D WIN32=${WIN32}
                    -D SASFIT_VERSION:STRING=${SASFIT_VERSION}
                    -P ${SASFIT_PCKG_SCRIPT}
            DEPENDS sasfit_binary
            COMMENT "Creating sasfit package ..."
            VERBATIM)
    endif()
endif(CREATE_BINARY)

# vim: set et ts=4 sw=4 sts=4 tw=0:
