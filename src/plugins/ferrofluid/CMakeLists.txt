cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_ferrofluid)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	sasfit_ff_ff_chain_rw_psi.c
	sasfit_ff_ff_chain_rw_radial.c
	sasfit_ff_ff_chain_rw_magnetic.c
	sasfit_ff_ff_chain_rw_cross_term.c
	sasfit_ff_ff_chain_rw_cross_term_radial.c
	sasfit_ff_ff_chain_rw_psi_POLARIS.c
	sasfit_ff_ff_chain_rw_psi_pp.c
	sasfit_ff_ff_chain_rw_psi_mm.c
	sasfit_ff_ff_chain_rw_psi_pm.c
	sasfit_ff_ff_chain_rw_psi_mp.c
	sasfit_ff_ff_chain_saw_psi.c
	sasfit_ff_ff_chain_saw_radial.c
	sasfit_ff_ff_chain_saw_magnetic.c
	sasfit_ff_ff_chain_saw_cross_term.c
	sasfit_ff_ff_chain_saw_cross_term_radial.c
	sasfit_ff_ff_chain_saw_psi_POLARIS.c
	sasfit_ff_ff_chain_saw_psi_pp.c
	sasfit_ff_ff_chain_saw_psi_mm.c
	sasfit_ff_ff_chain_saw_psi_pm.c
	sasfit_ff_ff_chain_saw_psi_mp.c
	sasfit_ff_ff_chain_parabolic_psi.c
	sasfit_ff_ff_chain_parabolic_radial.c
	sasfit_ff_ff_chain_parabolic_magnetic.c
	sasfit_ff_ff_chain_parabolic_cross_term.c
	sasfit_ff_ff_chain_parabolic_cross_term_radial.c
	sasfit_ff_ff_chain_parabolic_psi_POLARIS.c
	sasfit_ff_ff_chain_parabolic_psi_pp.c
	sasfit_ff_ff_chain_parabolic_psi_mm.c
	sasfit_ff_ff_chain_parabolic_psi_pm.c
	sasfit_ff_ff_chain_parabolic_psi_mp.c
	sasfit_ff_chain_parabolic_profile.c
	sasfit_ff_ferrofluid_utils.c
	interface.c
)

# path to 'Find*.cmake' files in sasfit package, 
# relative to the location of this file
set(REL_CMAKE_MODULE_PATH ../../cmake)

#                                                       #
### no adjustments required below this line (usually) ###
#                                                       #

project(${PRJ_NAME} C)

set(LIB_IS_PLUGIN true)

set(SRC_DIR ${${PRJ_NAME}_SOURCE_DIR})
set(CMAKE_MODULE_PATH ${SRC_DIR}/${REL_CMAKE_MODULE_PATH})

include(SasfitCmakeUtils)
sasfit_cmake_plugin()
