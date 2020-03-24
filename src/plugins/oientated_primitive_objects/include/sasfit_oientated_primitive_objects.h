/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_OIENTATED_PRIMITIVE_OBJECTS_H
#define SASFIT_PLUGIN_OIENTATED_PRIMITIVE_OBJECTS_H

#include <sasfit_common_shared_exports.h>
#include "sasfit_opo_lib.h"

/**
 * \file sasfit_oientated_primitive_objects.h
 * Public available functions and descriptions of the oientated_primitive_objects plugin.
 */

/**
 * \def sasfit_oientated_primitive_objects_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_oientated_primitive_objects_EXPORTS)
	#ifdef sasfit_oientated_primitive_objects_DLLEXP
	#undef sasfit_oientated_primitive_objects_DLLEXP
	#endif
	#define sasfit_oientated_primitive_objects_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_oientated_primitive_objects_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_oientated_primitive_objects_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_cylinder__opo_ ################ */
/**
 * \defgroup ff_cylinder_opo cylinder (OPO)
 * \ingroup ff_devel
 *
 * \brief \<some brief description of cylinder (OPO) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first axis</td>
 *      </tr><tr>
 *       <td>\b ea_x</td>
 *       <td>x component of first axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ea_y</td>
 *       <td>y component of first axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ea_z</td>
 *       <td>z component of first axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b b</td>
 *       <td>length of second axis</td>
 *      </tr><tr>
 *       <td>\b eb_x</td>
 *       <td>x component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eb_y</td>
 *       <td>z component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eb_z</td>
 *       <td>z component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b c</td>
 *       <td>length of third axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eta_p</td>
 *       <td>scattering length density of particle</td>
 *      </tr><tr>
 *       <td>\b eta_m</td>
 *       <td>scattering length density of matrix</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>first Euler angle</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>second Euler angle</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>third Euler angle</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>direction of Q on detector (psi=0: direction to the right)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_cylinder_opo
 *
 * \sa sasfit_oientated_primitive_objects.h, ff_devel
 */
sasfit_oientated_primitive_objects_DLLEXP scalar sasfit_ff_cylinder_opo(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylinder__opo_
 *
 * \sa sasfit_oientated_primitive_objects.h, ff_devel
 */
sasfit_oientated_primitive_objects_DLLEXP scalar sasfit_ff_cylinder_opo_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylinder__opo_
 *
 * \sa sasfit_oientated_primitive_objects.h, ff_devel
 */
sasfit_oientated_primitive_objects_DLLEXP scalar sasfit_ff_cylinder_opo_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_cylinder_opo ################ */

/* ################ start ff_parallelepiped_opo ################ */
/**
 * \defgroup ff_parallelepiped_opo parallelepiped (OPO)
 * \ingroup ff_devel
 *
 * \brief \<some brief description of parallelepiped (OPO) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first axis</td>
 *      </tr><tr>
 *       <td>\b ea_x</td>
 *       <td>x component of first axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ea_y</td>
 *       <td>y component of first axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ea_z</td>
 *       <td>z component of first axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b b</td>
 *       <td>length of second axis</td>
 *      </tr><tr>
 *       <td>\b eb_x</td>
 *       <td>x component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eb_y</td>
 *       <td>z component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eb_z</td>
 *       <td>z component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b c</td>
 *       <td>length of third axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eta_p</td>
 *       <td>scattering length density of particle</td>
 *      </tr><tr>
 *       <td>\b eta_m</td>
 *       <td>scattering length density of matrix</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>first Euler angle</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>second Euler angle</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>third Euler angle</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>direction of Q on detector (psi=0: direction to the right)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_parallelepiped_opo
 *
 * \sa sasfit_oientated_primitive_objects.h, ff_devel
 */
sasfit_oientated_primitive_objects_DLLEXP scalar sasfit_ff_parallelepiped_opo(scalar q, sasfit_param * p);

/**
 * \ingroup ff_parallelepiped_opo
 *
 * \sa sasfit_oientated_primitive_objects.h, ff_devel
 */
sasfit_oientated_primitive_objects_DLLEXP scalar sasfit_ff_parallelepiped_opo_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_parallelepiped_opo
 *
 * \sa sasfit_oientated_primitive_objects.h, ff_devel
 */
sasfit_oientated_primitive_objects_DLLEXP scalar sasfit_ff_parallelepiped_opo_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_parallelepiped_opo ################ */

/* ################ start ff_ellipsoid_opo ################ */
/**
 * \defgroup ff_ellipsoid_opo ellipsoid (OPO)
 * \ingroup ff_devel
 *
 * \brief \<some brief description of ellipsoid (OPO) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first axis</td>
 *      </tr><tr>
 *       <td>\b ea_x</td>
 *       <td>x component of first axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ea_y</td>
 *       <td>y component of first axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ea_z</td>
 *       <td>z component of first axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b b</td>
 *       <td>length of second axis</td>
 *      </tr><tr>
 *       <td>\b eb_x</td>
 *       <td>x component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eb_y</td>
 *       <td>z component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eb_z</td>
 *       <td>z component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b c</td>
 *       <td>length of third axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eta_p</td>
 *       <td>scattering length density of particle</td>
 *      </tr><tr>
 *       <td>\b eta_m</td>
 *       <td>scattering length density of matrix</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>first Euler angle</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>second Euler angle</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>third Euler angle</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>direction of Q on detector (psi=0: direction to the right)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ellipsoid_opo
 *
 * \sa sasfit_oientated_primitive_objects.h, ff_devel
 */
sasfit_oientated_primitive_objects_DLLEXP scalar sasfit_ff_ellipsoid_opo(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellipsoid_opo
 *
 * \sa sasfit_oientated_primitive_objects.h, ff_devel
 */
sasfit_oientated_primitive_objects_DLLEXP scalar sasfit_ff_ellipsoid_opo_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellipsoid_opo
 *
 * \sa sasfit_oientated_primitive_objects.h, ff_devel
 */
sasfit_oientated_primitive_objects_DLLEXP scalar sasfit_ff_ellipsoid_opo_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ellipsoid_opo ################ */

/* ################ start ff_superellipsoid_opo ################ */
/**
 * \defgroup ff_superellipsoid_opo super ellipsoid (OPO)
 * \ingroup ff_devel
 *
 * \brief \<some brief description of super ellipsoid (OPO) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first axis</td>
 *      </tr><tr>
 *       <td>\b ea_x</td>
 *       <td>x component of first axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ea_y</td>
 *       <td>y component of first axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ea_z</td>
 *       <td>z component of first axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b b</td>
 *       <td>length of second axis</td>
 *      </tr><tr>
 *       <td>\b eb_x</td>
 *       <td>x component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eb_y</td>
 *       <td>z component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eb_z</td>
 *       <td>z component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b c</td>
 *       <td>length of third axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eta_p</td>
 *       <td>scattering length density of particle</td>
 *      </tr><tr>
 *       <td>\b eta_m</td>
 *       <td>scattering length density of matrix</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>first Euler angle</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>second Euler angle</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>third Euler angle</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>direction of Q on detector (psi=0: direction to the right)</td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td>shape parameter, ex-ey (p>0)</td>
 *      </tr><tr>
 *       <td>\b q</td>
 *       <td>shape parameter, ez (q>0)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_superellipsoid_opo
 *
 * \sa sasfit_oientated_primitive_objects.h, ff_devel
 */
sasfit_oientated_primitive_objects_DLLEXP scalar sasfit_ff_superellipsoid_opo(scalar q, sasfit_param * p);

/**
 * \ingroup ff_superellipsoid_opo
 *
 * \sa sasfit_oientated_primitive_objects.h, ff_devel
 */
sasfit_oientated_primitive_objects_DLLEXP scalar sasfit_ff_superellipsoid_opo_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_superellipsoid_opo
 *
 * \sa sasfit_oientated_primitive_objects.h, ff_devel
 */
sasfit_oientated_primitive_objects_DLLEXP scalar sasfit_ff_superellipsoid_opo_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_superellipsoid_opo ################ */

/* ################ start ff_supershape_opo ################ */
/**
 * \defgroup ff_supershape_opo super shape (OPO)
 * \ingroup ff_devel
 *
 * \brief \<some brief description of super shape (OPO) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first axis</td>
 *      </tr><tr>
 *       <td>\b ea_x</td>
 *       <td>x component of first axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ea_y</td>
 *       <td>y component of first axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ea_z</td>
 *       <td>z component of first axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b b</td>
 *       <td>length of second axis</td>
 *      </tr><tr>
 *       <td>\b eb_x</td>
 *       <td>x component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eb_y</td>
 *       <td>z component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eb_z</td>
 *       <td>z component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b c</td>
 *       <td>length of third axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eta_p</td>
 *       <td>scattering length density of particle</td>
 *      </tr><tr>
 *       <td>\b eta_m</td>
 *       <td>scattering length density of matrix</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>first Euler angle</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>second Euler angle</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>third Euler angle</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>direction of Q on detector (psi=0: direction to the right)</td>
 *      </tr><tr>
 *       <td>\b m</td>
 *       <td> symmetry (x-y)</td>
 *      </tr><tr>
 *       <td>\b theta0</td>
 *       <td> zero theta in (xy) plane</td>
 *      </tr><tr>
 *       <td>\b n1</td>
 *       <td> shape exponent 1</td>
 *      </tr><tr>
 *       <td>\b n2</td>
 *       <td> shape exponent 2</td>
 *      </tr><tr>
 *       <td>\b n3</td>
 *       <td> shape exponent 3</td>
 *      </tr><tr>
 *       <td>\b a</td>
 *       <td> length 1 parameter</td>
 *      </tr><tr>
 *       <td>\b b</td>
 *       <td> length 2 parameter</td>
 *      </tr><tr>
 *       <td>\b sm</td>
 *       <td> 2nd symmetry (x-y)</td>
 *      </tr><tr>
 *       <td>\b phi0</td>
 *       <td> zero phi in (xy-z) plane</td>
 *      </tr><tr>
 *       <td>\b N1</td>
 *       <td> 2nd shape exponent 1</td>
 *      </tr><tr>
 *       <td>\b N2</td>
 *       <td> 2nd shape exponent 2</td>
 *      </tr><tr>
 *       <td>\b N3</td>
 *       <td> 2nd shape exponent 3</td>
 *      </tr><tr>
 *       <td>\b A</td>
 *       <td> 2nd length 1 parameter</td>
 *      </tr><tr>
 *       <td>\b B</td>
 *       <td> 2nd length 2 parameter</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_supershape_opo
 *
 * \sa sasfit_oientated_primitive_objects.h, ff_devel
 */
sasfit_oientated_primitive_objects_DLLEXP scalar sasfit_ff_supershape_opo(scalar q, sasfit_param * p);

/**
 * \ingroup ff_supershape_opo
 *
 * \sa sasfit_oientated_primitive_objects.h, ff_devel
 */
sasfit_oientated_primitive_objects_DLLEXP scalar sasfit_ff_supershape_opo_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_supershape_opo
 *
 * \sa sasfit_oientated_primitive_objects.h, ff_devel
 */
sasfit_oientated_primitive_objects_DLLEXP scalar sasfit_ff_supershape_opo_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_supershape_opo ################ */

/* ################ start ff_supertoroid_helix_opo ################ */
/**
 * \defgroup ff_supertoroid_helix_opo super shaped toroid/helix (OPO)
 * \ingroup ff_devel
 *
 * \brief \<some brief description of super shaped toroid/helix (OPO) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first axis</td>
 *      </tr><tr>
 *       <td>\b ea_x</td>
 *       <td>x component of first axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ea_y</td>
 *       <td>y component of first axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ea_z</td>
 *       <td>z component of first axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b b</td>
 *       <td>length of second axis</td>
 *      </tr><tr>
 *       <td>\b eb_x</td>
 *       <td>x component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eb_y</td>
 *       <td>z component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eb_z</td>
 *       <td>z component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b c</td>
 *       <td>length of third axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eta_p</td>
 *       <td>scattering length density of particle</td>
 *      </tr><tr>
 *       <td>\b eta_m</td>
 *       <td>scattering length density of matrix</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>first Euler angle</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>second Euler angle</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>third Euler angle</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>direction of Q on detector (psi=0: direction to the right)</td>
 *      </tr><tr>
 *       <td>\b m</td>
 *       <td> symmetry (x-y)</td>
 *      </tr><tr>
 *       <td>\b theta0</td>
 *       <td> zero theta in (xy) plane</td>
 *      </tr><tr>
 *       <td>\b n1</td>
 *       <td> shape exponent 1</td>
 *      </tr><tr>
 *       <td>\b n2</td>
 *       <td> shape exponent 2</td>
 *      </tr><tr>
 *       <td>\b n3</td>
 *       <td> shape exponent 3</td>
 *      </tr><tr>
 *       <td>\b a</td>
 *       <td> length 1 parameter</td>
 *      </tr><tr>
 *       <td>\b b</td>
 *       <td> length 2 parameter</td>
 *      </tr><tr>
 *       <td>\b sm</td>
 *       <td> 2nd symmetry (x-y)</td>
 *      </tr><tr>
 *       <td>\b phi0</td>
 *       <td> zero phi in (xy-z) plane</td>
 *      </tr><tr>
 *       <td>\b N1</td>
 *       <td> 2nd shape exponent 1</td>
 *      </tr><tr>
 *       <td>\b N2</td>
 *       <td> 2nd shape exponent 2</td>
 *      </tr><tr>
 *       <td>\b N3</td>
 *       <td> 2nd shape exponent 3</td>
 *      </tr><tr>
 *       <td>\b A</td>
 *       <td> 2nd length 1 parameter</td>
 *      </tr><tr>
 *       <td>\b B</td>
 *       <td> 2nd length 2 parameter</td>
 *      </tr><tr>
 *       <td>\b R </td>
 *       <td> torus/helix radius</td>
 *      </tr><tr>
 *       <td>\b P</td>
 *       <td> pitch of helix (P=0:torus)</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td> twist angle of strand</td>
 *      </tr><tr>
 *       <td>\b T</td>
 *       <td> number of turns</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_supertoroid_helix_opo
 *
 * \sa sasfit_oientated_primitive_objects.h, ff_devel
 */
sasfit_oientated_primitive_objects_DLLEXP scalar sasfit_ff_supertoroid_helix_opo(scalar q, sasfit_param * p);

/**
 * \ingroup ff_supertoroid_helix_opo
 *
 * \sa sasfit_oientated_primitive_objects.h, ff_devel
 */
sasfit_oientated_primitive_objects_DLLEXP scalar sasfit_ff_supertoroid_helix_opo_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_supertoroid_helix_opo
 *
 * \sa sasfit_oientated_primitive_objects.h, ff_devel
 */
sasfit_oientated_primitive_objects_DLLEXP scalar sasfit_ff_supertoroid_helix_opo_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_supertoroid_helix_opo ################ */

/* ################ start ff_rationalsupershape_opo ################ */
/**
 * \defgroup ff_rationalsupershape_opo rational super shape (OPO)
 * \ingroup ff_devel
 *
 * \brief \<some brief description of rational super shape (OPO) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first axis</td>
 *      </tr><tr>
 *       <td>\b ea_x</td>
 *       <td>x component of first axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ea_y</td>
 *       <td>y component of first axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ea_z</td>
 *       <td>z component of first axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b b</td>
 *       <td>length of second axis</td>
 *      </tr><tr>
 *       <td>\b eb_x</td>
 *       <td>x component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eb_y</td>
 *       <td>z component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eb_z</td>
 *       <td>z component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b c</td>
 *       <td>length of third axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of second axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eta_p</td>
 *       <td>scattering length density of particle</td>
 *      </tr><tr>
 *       <td>\b eta_m</td>
 *       <td>scattering length density of matrix</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>first Euler angle</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>second Euler angle</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>third Euler angle</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>direction of Q on detector (psi=0: direction to the right)</td>
 *      </tr><tr>
 *       <td>\b m</td>
 *       <td> symmetry (x-y)</td>
 *      </tr><tr>
 *       <td>\b theta0</td>
 *       <td> zero theta in (xy) plane</td>
 *      </tr><tr>
 *       <td>\b n1</td>
 *       <td> shape exponent 1</td>
 *      </tr><tr>
 *       <td>\b n2</td>
 *       <td> shape exponent 2</td>
 *      </tr><tr>
 *       <td>\b n3</td>
 *       <td> shape exponent 3</td>
 *      </tr><tr>
 *       <td>\b a</td>
 *       <td> length 1 parameter</td>
 *      </tr><tr>
 *       <td>\b b</td>
 *       <td> length 2 parameter</td>
 *      </tr><tr>
 *       <td>\b sm</td>
 *       <td> 2nd symmetry (x-y)</td>
 *      </tr><tr>
 *       <td>\b phi0</td>
 *       <td> zero phi in (xy-z) plane</td>
 *      </tr><tr>
 *       <td>\b N1</td>
 *       <td> 2nd shape exponent 1</td>
 *      </tr><tr>
 *       <td>\b N2</td>
 *       <td> 2nd shape exponent 2</td>
 *      </tr><tr>
 *       <td>\b N3</td>
 *       <td> 2nd shape exponent 3</td>
 *      </tr><tr>
 *       <td>\b A</td>
 *       <td> 2nd length 1 parameter</td>
 *      </tr><tr>
 *       <td>\b B</td>
 *       <td> 2nd length 2 parameter</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_rationalsupershape_opo
 *
 * \sa sasfit_oientated_primitive_objects.h, ff_devel
 */
sasfit_oientated_primitive_objects_DLLEXP scalar sasfit_ff_rationalsupershape_opo(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rationalsupershape_opo
 *
 * \sa sasfit_oientated_primitive_objects.h, ff_devel
 */
sasfit_oientated_primitive_objects_DLLEXP scalar sasfit_ff_rationalsupershape_opo_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rationalsupershape_opo
 *
 * \sa sasfit_oientated_primitive_objects.h, ff_devel
 */
sasfit_oientated_primitive_objects_DLLEXP scalar sasfit_ff_rationalsupershape_opo_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rationalsupershape_opo ################ */
#endif // this file

