# cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_RMSA)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	rmsa.c
	polyroots.c
	rmsa_physical.c
	sasfit_sq_RMSA.c
	interface.c
)

# add plugin path for finding 'Find*.cmake' files for new <LIBS_EXT>
set(REL_CMAKE_MODULE_PATH .)

#                                                       #
### no adjustments required below this line (usually) ###
#                                                       #

project(${PRJ_NAME} C)

set(LIB_IS_PLUGIN true)

set(SRC_DIR ${${PRJ_NAME}_SOURCE_DIR})
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${SRC_DIR}/${MODULE_PATH}")

include(SasfitCmakeUtils)
sasfit_cmake_plugin()

# Enable OpenMP for this plugin's multi-core g(r)-test summation (see
# rmsa.c's #ifdef _OPENMP / #pragma omp sites, and this project's
# README.md "OpenMP" section) when SASfit's own SASFIT_WITH_OPENMP
# option is on. SASFIT_WITH_OPENMP and SASFIT_OPENMP_LINK_LIBS are set
# in the top-level src/CMakeLists.txt, before add_subdirectory(plugins)
# -- both are visible here via normal CMake parent-to-child directory
# scoping (confirmed directly: src/plugins/CMakeLists.txt adds every
# plugin via a plain add_subdirectory(), with no EXCLUDE_FROM_ALL or
# other scope-breaking mechanism), so no find_package(OpenMP) of this
# plugin's own is needed here. SASFIT_OPENMP_LINK_LIBS is already ""
# unless SASFIT_WITH_OPENMP is on AND OpenMP was actually found, so
# checking it alone is sufficient; this is a no-op by default
# (SASFIT_WITH_OPENMP defaults OFF), matching current behavior exactly.
# Linking the OpenMP::OpenMP_C imported target (rather than adding
# -fopenmp manually) also brings in its INTERFACE_COMPILE_OPTIONS
# automatically -- no separate compile-flag step needed.
if(SASFIT_OPENMP_LINK_LIBS)
	target_link_libraries(${PRJ_NAME} ${SASFIT_OPENMP_LINK_LIBS})
endif()
