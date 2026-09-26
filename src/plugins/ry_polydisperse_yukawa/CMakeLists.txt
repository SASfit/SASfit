# cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_ry_polydisperse_yukawa)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# SUNDIALS' KINSOL provides the Anderson-accelerated fixed-point solve
# (KIN_FP) and the Newton-Krylov solve used by ry_polydisperse_core.c.
# SASfit already links SUNDIALS for its own OZ solver (see
# ${sundials_LIBRARIES} in src/CMakeLists.txt), so this adds no new
# dependency -- but the plugin does need the libraries named explicitly.
# sundials_core exists only in SUNDIALS 7 (which SASfit ships in
# src/sundials7); it is omitted automatically for 6.x.
set(SUNDIALS_LIBS sundials_kinsol sundials_nvecserial)
if(EXISTS "${CMAKE_SOURCE_DIR}/sundials7")
	list(APPEND SUNDIALS_LIBS sundials_core)
endif()

# source files
set(SOURCE_${PRJ_NAME}
	ry_polydisperse_core.c
	sasfit_ff_RYPolydisperseYukawa.c
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

target_link_libraries(${PRJ_NAME} ${SUNDIALS_LIBS})
