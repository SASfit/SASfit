# cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_metalog)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	sasfit_sd_metalog_logit.c
	sasfit_sd_metalog_clipped.c
	sasfit_sd_metalog_log_clipped.c
	sasfit_ff_metalog_sphere.c
	sasfit_ff_metaloglog_sphere.c
	sasfit_ff_metaloglogit_sphere.c
	sasfit_ff_metalog_log_profile_sphere.c
	sasfit_ff_metalog_logit_profile_sphere.c
	sasfit_ff_metalog_clipped_profile_sphere.c
	metalog_utils.c
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
