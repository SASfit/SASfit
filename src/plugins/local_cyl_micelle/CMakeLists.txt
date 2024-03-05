cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_local_cyl_micelle)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	sasfit_ff_cyl_chains_rw__nagg.c
	sasfit_ff_cyl_chains_rw__rc.c
	sasfit_ff_cyl_chains_rw_.c
	sasfit_ff_worm_chains_rw__nagg.c
	sasfit_ff_worm_chains_rw__rc.c
	sasfit_ff_worm_chains_rw_.c
	sasfit_ff_rod_chains_rw__nagg.c
	sasfit_ff_rod_chains_rw__rc.c
	sasfit_ff_rod_chains_rw_.c
	sasfit_ff_rod__r__a_nagg.c
	sasfit_ff_rod__r__a_rc.c
	sasfit_ff_rod__r__a.c
	sasfit_ff_rod_gauss_nagg.c
	sasfit_ff_rod_gauss_rc.c
	sasfit_ff_rod_gauss.c
	sasfit_ff_rod_exp_nagg.c
	sasfit_ff_rod_exp_rc.c
	sasfit_ff_rod_exp.c
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
