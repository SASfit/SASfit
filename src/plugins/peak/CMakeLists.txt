cmake_minimum_required(VERSION 2.6)

# the project name
set(PRJ_NAME sasfit_peak)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	sasfit_peak_beta_amplitude.c
	sasfit_peak_beta_area.c
	sasfit_peak_chi_squared_amplitude.c
	sasfit_peak_chi_squared_area.c
	sasfit_peak_erfc_amplitude.c
	sasfit_peak_erfc_area.c
	sasfit_peak_error_amplitude.c
	sasfit_peak_error_area.c
	sasfit_peak_exponentially_modified_gaussian_amplitude.c
	sasfit_peak_exponentially_modified_gaussian_area.c
	sasfit_peak_extreme_value_amplitude.c
	sasfit_peak_extreme_value_area.c
	sasfit_peak_gex_amplitude.c
	sasfit_peak_gex_area.c
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
