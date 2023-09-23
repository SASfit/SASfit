cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_fractal_obj)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	sasfit_sq_mass_fractal__exp__x_a__cut_off_.c
	sasfit_sq_mass_fractal__overapsph_cut_off_.c
	sasfit_sq_mass_fractal__gaussian_cut_off_.c
	sasfit_sq_mass_fractal__exp_cut_off_.c
	sasfit_ff_fisher_burford.c
	sasfit_ff_massfractexp.c
	sasfit_ff_massfractgauss.c
	sasfit_ff_aggregate__exp__x_a__cut_off_.c
	sasfit_ff_aggregate__overlapsph_cut_off_.c
	sasfit_ff_dlcaggregation.c
	sasfit_ff_rlcaggregation.c
	sasfit_ff_stackdiscs.c
	sasfit_ff_dumbbellshell.c
	sasfit_ff_two_attached_spheres.c
	sasfit_ff_two_attached_spheres_w_distr.c
	sasfit_ff_doubleshellchain.c
	sasfit_ff_tetrahedrondoubleshell.c
	interface.c
	sasfit_fractal_utils.c
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
