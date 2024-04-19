# cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_generalized_johnson_distribution)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	sasfit_sd_g_n_pdf.c
	sasfit_sd_g_l_pdf.c
	sasfit_sd_g_c_pdf.c
	sasfit_sd_g_n_pdf_s.c
	sasfit_sd_g_l_pdf_s.c
	sasfit_sd_g_c_pdf_s.c
	sasfit_sd_g_n_pdf_b.c
	sasfit_sd_g_l_pdf_b.c
	sasfit_sd_g_c_pdf_b.c
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
