cmake_minimum_required(VERSION 2.6)

# the project name
set(PRJ_NAME sasfit_stochastic_models)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	sasfit_ff_boolean_intersection_model.c
	sasfit_ff_boolean_union_model.c
	sasfit_ff_dead_leaves_model.c
	sasfit_ff_clipped_random_waves_1.c
	sasfit_ff_clipped_random_waves_2.c
	sasfit_ff_clipped_random_waves_3.c
	sasfit_ff_clipped_random_waves_4.c
	sasfit_ff_clipped_random_waves_5.c
	sasfit_ff_clipped_random_waves_twocut_1.c
	sasfit_ff_clipped_random_waves_twocut_2.c
	sasfit_ff_clipped_random_waves_twocut_3.c
	sasfit_ff_clipped_random_waves_twocut_4.c
	sasfit_ff_clipped_random_waves_twocut_5.c
	sasfit_ff_clipped_random_waves_twocut_intersection_1.c
	sasfit_ff_clipped_random_waves_twocut_intersection_2.c
	sasfit_ff_clipped_random_waves_twocut_intersection_3.c
	sasfit_ff_clipped_random_waves_twocut_intersection_4.c
	sasfit_ff_clipped_random_waves_twocut_intersection_5.c
	sasfit_stochastic_utils.c
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
