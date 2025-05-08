# cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_t_r_y)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	sasfit_sd_fgld_.c
	sasfit_ff_fgld_profile.c
	sasfit_ff_fgld_profile_sphere.c
	sasfit_ff_fgld_profile_PcsCyl.c
	sasfit_ff_fgld_profile_PcsPlanar.c
	sasfit_ff_fgld_sphere.c
	sasfit_sd_egu_w_l.c
	sasfit_ff_egu_w_l_profile.c
	sasfit_ff_egu_w_l_profile_sphere.c
	sasfit_ff_egu_w_l_profile_PcsCyl.c
	sasfit_ff_egu_w_l_profile_PcsPlanar.c
	sasfit_ff_egu_w_l_sphere.c
	sasfit_sd_g_c_e__.c
	sasfit_sd_ll_l_gl__.c
	sasfit_sd_p_w_gl__.c
	sasfit_sd_n_u_gl__.c
	sasfit_sd_u_e_gl__.c
	t_lindley_y_utils.c
	sasfit_sd_w_l_e_.c
	sasfit_ff_w_l_e_profile.c
	sasfit_ff_w_l_e_profile_sphere.c
	sasfit_ff_w_l_e_profile_PcsCyl.c
	sasfit_ff_w_l_e_profile_PcsPlanar.c
	sasfit_ff_w_l_e_sphere.c
	sasfit_sd_n_l_c_.c
	sasfit_ff_n_l_c_profile.c
	sasfit_ff_n_l_c_profile_sphere.c
	sasfit_ff_n_l_c_profile_PcsCyl.c
	sasfit_ff_n_l_c_profile_PcsPlanar.c
	sasfit_ff_n_l_c_sphere.c

	sasfit_sd_e_l_ll_.c
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
