cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_thin_objects)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	sasfit_ff_disc_sd_homoxs.c
	sasfit_ff_ellsh_sd_homoxs.c
	sasfit_ff_cylsh_sd_homoxs.c
	sasfit_ff_disc_sd_bilayergauss.c
	sasfit_ff_ellsh_sd_bilayergauss.c
	sasfit_ff_cylsh_sd_bilayergauss.c

	sasfit_ff_rod_sd_ellcylsh.c
	sasfit_ff_worm_ps1_sd_ellcylsh.c
	sasfit_ff_worm_ps2_sd_ellcylsh.c
	sasfit_ff_worm_ps3_sd_ellcylsh.c
	sasfit_ff_worm_kholodenko_sd_ellcylsh.c

	sasfit_ff_pcs_homogeneousplate.c
	sasfit_ff_pcs_plate_chains_rw_.c
	sasfit_ff_pcs_layeredcentrosymmetricxs.c
	sasfit_ff_pcs_bilayergaus.c
	sasfit_ff_pcs_twoinfinitelythinlayers.c
	
	sasfit_ff_pcs_homogeneouscyl.c
	sasfit_ff_pcs_ellcylsh.c

	sasfit_sq_p__q___thin_disc.c
	sasfit_sq_p__q___thin_spherical_shell.c
	sasfit_sq_p__q___thin_ellipsoidal_shell.c
	sasfit_sq_p__q___thin_hollow_cylinder.c
	
	sasfit_sq_p__q___rod.c
	sasfit_sq_p__q___worm_ps1_.c
	sasfit_sq_p__q___worm_ps2_.c
	sasfit_sq_p__q___worm_ps3_.c
	sasfit_sq_p__q___worm_ps3_RPA.c
	sasfit_sq_p__q___worm_ps3_PRISM1.c
	sasfit_sq_p__q___worm_ps3_PRISM2.c
	sasfit_sq_p__q___worm_ps3_PRISM3.c
	sasfit_sq_p__q___kholodenko_worm.c
	sasfit_sq_freely_joined_chain_of_rods.c
	sasfit_sq_koyama_worms.c

	sasfit_ao_utils.c
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
