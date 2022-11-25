cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_oriented_primitive_objects)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
    sasfit_opo_utils.c
    sasfit_polyhedra_utils.c
	sasfit_ff_cylinder_opo.c
	sasfit_ff_cylinder_opo_random.c
	sasfit_ff_parallelepiped_opo.c
	sasfit_ff_parallelepiped_opo_random.c
	sasfit_ff_ellipsoid_opo.c
	sasfit_ff_ellipsoid_opo_random.c
	sasfit_ff_superegg_opo.c
	sasfit_ff_superegg_opo_random.c
	sasfit_ff_superellipsoid_opo.c
	sasfit_ff_superellipsoid_opo_random.c
	sasfit_ff_superquadrics_opo.c
	sasfit_ff_superquadrics_opo_random.c
	sasfit_ff_supershape_opo.c
	sasfit_ff_supershape_opo_random.c
	sasfit_ff_supertoroid_helix_opo.c
	sasfit_ff_supertoroid_helix_opo_random.c
	sasfit_ff_rationalsupershape_opo.c
	sasfit_ff_rationalsupershape_opo_random.c
	sasfit_ff_tetrahedra__opo_.c
	sasfit_ff_tetrahedra__opo_random_.c
	sasfit_ff_octahedra__opo_.c
	sasfit_ff_octahedra__opo_random_.c
	sasfit_ff_rhombic_dodecahedra__opo_.c
	sasfit_ff_rhombic_dodecahedra__opo_random_.c
	sasfit_ff_icosahedra__opo_.c
	sasfit_ff_icosahedra__opo_random_.c
	sasfit_ff_hexahedra__opo_.c
	sasfit_ff_hexahedra__opo_random_.c
	sasfit_ff_cone_opo.c
	sasfit_ff_cone_opo_random.c
	sasfit_ff_pyramid4_opo.c
	sasfit_ff_pyramid4_opo_random.c
	sasfit_ff_cone6_opo.c
	sasfit_ff_cone6_opo_random.c
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
