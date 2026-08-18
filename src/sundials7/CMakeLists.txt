# src/sundials7/CMakeLists.txt
#
# Independent, separately-versioned SUNDIALS build (currently 7.8.0),
# used ONLY by the robertus_shs plugin (see its CMakeLists.txt) --
# src/sasfit_oz/sasfit_oz_solver.c continues to use the project's
# existing bundled SUNDIALS 2.7.0 (src/sundials/) completely
# unmodified. The two versions export same-named-but-incompatible-ABI
# symbols (e.g. KINCreate takes no SUNContext in 2.7.0, requires one
# in 7.x), so whatever links this one MUST also pass
# -Wl,--exclude-libs=ALL (or an equivalent) to keep those symbols from
# being visible in its own shared object -- verified directly: without
# that flag, loading a library built against each version into the
# same process segfaults immediately during dynamic symbol resolution,
# before either version's own code even runs; with it, both solve
# correctly side by side. See robertus_shs's own CMakeLists.txt for
# where that flag is actually applied.
#
# Builds from a locally-downloaded source tarball (offline-buildable,
# same philosophy as src/sundials/'s bundled sundials-2.7.0.tar.gz)
# rather than CMake FetchContent -- place a sundials-*.tar.gz release
# source archive (as downloaded directly from
# https://github.com/LLNL/sundials/releases) in this directory.
#
# This does NOT reuse this project's own build_from_source() macro
# (see src/cmake/SasfitCmakeUtils.cmake): that macro assumes the
# tarball's extracted top-level directory name starts with the same
# name as its containing folder (matching e.g. src/sundials/'s
# "sundials-2.7.0.tar.gz" -> "sundials-2.7.0/" pattern, matching THIS
# directory's own "sundials" name) -- but GitHub's own release tarball
# for SUNDIALS extracts to "sundials-<version>/", which does not start
# with "sundials7", the name of THIS directory. Rather than fight that
# assumption or rename the extracted tree by hand, this file discovers
# whatever the tarball actually contains directly, below.

cmake_minimum_required(VERSION 3.14)

file(GLOB SUNDIALS7_TARBALL "${CMAKE_CURRENT_SOURCE_DIR}/sundials-*.tar.gz")
list(LENGTH SUNDIALS7_TARBALL _sundials7_n_tarballs)
if(_sundials7_n_tarballs EQUAL 0)
    message(FATAL_ERROR
        "sundials7: no sundials-*.tar.gz found in ${CMAKE_CURRENT_SOURCE_DIR}. "
        "Download a SUNDIALS 7.x release source tarball from "
        "https://github.com/LLNL/sundials/releases and place it in this "
        "directory.")
endif()
list(GET SUNDIALS7_TARBALL 0 SUNDIALS7_TARBALL)
message(STATUS "sundials7: found tarball ${SUNDIALS7_TARBALL}")

set(SUNDIALS7_EXTRACT_DIR "${CMAKE_CURRENT_BINARY_DIR}/src")
if(NOT EXISTS "${SUNDIALS7_EXTRACT_DIR}")
    file(MAKE_DIRECTORY "${SUNDIALS7_EXTRACT_DIR}")
    message(STATUS "sundials7: extracting ${SUNDIALS7_TARBALL} (one-time, cached in the build directory afterward)")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xzf "${SUNDIALS7_TARBALL}"
        WORKING_DIRECTORY "${SUNDIALS7_EXTRACT_DIR}"
        RESULT_VARIABLE _sundials7_extract_result
    )
    if(NOT _sundials7_extract_result EQUAL 0)
        message(FATAL_ERROR "sundials7: failed to extract ${SUNDIALS7_TARBALL}")
    endif()
endif()

# Find the extracted top-level directory, whatever it is actually
# named (see the big comment above for why this cannot be assumed).
file(GLOB SUNDIALS7_SRC_CANDIDATES "${SUNDIALS7_EXTRACT_DIR}/*")
set(SUNDIALS7_SRC_DIR "")
foreach(_cand ${SUNDIALS7_SRC_CANDIDATES})
    if(IS_DIRECTORY "${_cand}" AND EXISTS "${_cand}/CMakeLists.txt")
        set(SUNDIALS7_SRC_DIR "${_cand}")
        break()
    endif()
endforeach()
if(SUNDIALS7_SRC_DIR STREQUAL "")
    message(FATAL_ERROR "sundials7: could not find an extracted source directory with a CMakeLists.txt under ${SUNDIALS7_EXTRACT_DIR}")
endif()
message(STATUS "sundials7: using extracted source at ${SUNDIALS7_SRC_DIR}")

# Only KINSOL (+ its NVECTOR_SERIAL/SUNLINSOL_SPGMR dependencies, pulled
# in automatically) is actually needed here. Current (non-deprecated)
# option names -- SUNDIALS 7.x warns if the older BUILD_*/EXAMPLES_ENABLE_*
# names are used instead (they still work, just print a deprecation
# warning on every configure).
set(SUNDIALS_ENABLE_ARKODE OFF CACHE BOOL "" FORCE)
set(SUNDIALS_ENABLE_CVODE OFF CACHE BOOL "" FORCE)
set(SUNDIALS_ENABLE_CVODES OFF CACHE BOOL "" FORCE)
set(SUNDIALS_ENABLE_IDA OFF CACHE BOOL "" FORCE)
set(SUNDIALS_ENABLE_IDAS OFF CACHE BOOL "" FORCE)
set(SUNDIALS_ENABLE_KINSOL ON CACHE BOOL "" FORCE)
set(SUNDIALS_ENABLE_C_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

# Static, matching how src/sundials/ (2.7.0) is built and how
# Findsundials.cmake-style modules in this project locate libraries
# (glob for *.a) -- keeps the same "one self-contained plugin .so"
# deployment model used elsewhere in this project.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)

if(UNIX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()

add_subdirectory("${SUNDIALS7_SRC_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/build")

# vim: set ts=4 sts=4 sw=4 tw=0:
