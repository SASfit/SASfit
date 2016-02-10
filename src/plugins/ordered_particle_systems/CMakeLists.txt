cmake_minimum_required(VERSION 2.6)

# the project name
set(PRJ_NAME sasfit_ordered_particle_systems)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	sasfit_sq_sc_iso_lorentzian.c
	sasfit_sq_sc_iso_gaussian.c
	sasfit_sq_sc_iso_pearson.c
	sasfit_sq_sc_aniso_lorentzian.c
	sasfit_sq_sc_aniso_gaussian.c
	sasfit_sq_bcc_iso_lorentzian.c
	sasfit_sq_bcc_iso_gaussian.c
	sasfit_sq_bcc_aniso_lorentzian.c
	sasfit_sq_bcc_aniso_gaussian.c
	sasfit_sq_fcc_iso_lorentzian.c
	sasfit_sq_fcc_iso_gaussian.c
	sasfit_sq_fcc_aniso_lorentzian.c
	sasfit_sq_fcc_aniso_gaussian.c
	sasfit_sq_hcp_iso_lorentzian.c
	sasfit_sq_hcp_iso_gaussian.c
	sasfit_sq_hcp_aniso_lorentzian.c
	sasfit_sq_hcp_aniso_gaussian.c
	sasfit_sq_hex_iso_lorentzian.c
	sasfit_sq_hex_iso_gaussian.c
	sasfit_sq_hex_aniso_lorentzian.c
	sasfit_sq_hex_aniso_gaussian.c
	sasfit_sq_lam_iso_lorentzian.c
	sasfit_sq_lam_iso_gaussian.c
	sasfit_sq_lam_aniso_lorentzian.c
	sasfit_sq_lam_aniso_gaussian.c
	sasfit_sq_crec_iso_lorentzian.c
	sasfit_sq_crec_iso_gaussian.c
	sasfit_sq_crec_aniso_lorentzian.c
	sasfit_sq_crec_aniso_gaussian.c
	sasfit_sq_sq_iso_lorentzian.c
	sasfit_sq_sq_iso_gaussian.c
	sasfit_sq_sq_aniso_lorentzian.c
	sasfit_sq_sq_aniso_gaussian.c
	ops_utils.c
	interface.c
	erfcx.c
	err_fcts.c
	im_w_of_x.c
	w_of_z.c
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
