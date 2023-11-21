/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_oriented_PRIMITIVE_OBJECTS_H
#define SASFIT_PLUGIN_oriented_PRIMITIVE_OBJECTS_H

#include <sasfit_common_shared_exports.h>
#include "sasfit_opo_lib.h"

/**
 * \file sasfit_oriented_primitive_objects.h
 * Public available functions and descriptions of the oriented_primitive_objects plugin.
 */

/**
 * \def sasfit_oriented_primitive_objects_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_oriented_primitive_objects_EXPORTS)
	#ifdef sasfit_oriented_primitive_objects_DLLEXP
	#undef sasfit_oriented_primitive_objects_DLLEXP
	#endif
	#define sasfit_oriented_primitive_objects_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_oriented_primitive_objects_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_oriented_primitive_objects_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_cylinder__opo_ ################ */
/**
 * \defgroup ff_cylinder_opo cylinder (OPO)
 * \ingroup ff_plugins_oriented_primitive_objects
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
 *       <td>length of first half axis</td>
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
 *       <td>length of second half axis</td>
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
 *       <td>length of third half axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
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
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_cylinder_opo(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylinder__opo_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_cylinder_opo_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylinder__opo_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_cylinder_opo_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_cylinder_opo ################ */

/* ################ start ff_cylinder_opo_random ################ */
/**
 * \defgroup ff_cylinder_opo_random cylinder (OPO,random)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief \<some brief description of cylinder (OPO,random) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first half axis</td>
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
 *       <td>length of second half axis</td>
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
 *       <td>length of third half axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eta_p</td>
 *       <td>scattering length density of particle</td>
 *      </tr><tr>
 *       <td>\b eta_m</td>
 *       <td>scattering length density of matrix</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_cylinder_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_cylinder_opo_random(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylinder_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_cylinder_opo_random_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylinder_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_cylinder_opo_random_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_cylinder_opo_random ################ */

/* ################ start ff_parallelepiped_opo ################ */
/**
 * \defgroup ff_parallelepiped_opo parallelepiped (OPO)
 * \ingroup ff_plugins_oriented_primitive_objects
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
 *       <td>length of first half axis</td>
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
 *       <td>\b mu=b/a</td>
 *       <td>b=mu*a: length of second half axis</td>
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
 *       <td>\b nu=c/a</td>
 *       <td>c=nu*a: length of third half axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
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
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_parallelepiped_opo(scalar q, sasfit_param * p);

/**
 * \ingroup ff_parallelepiped_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_parallelepiped_opo_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_parallelepiped_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_parallelepiped_opo_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_parallelepiped_opo ################ */

/* ################ start ff_parallelepiped_opo_random ################ */
/**
 * \defgroup ff_parallelepiped_opo_random parallelepiped (OPO,random)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief \<some brief description of parallelepiped (OPO,random) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first half axis</td>
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
 *       <td>\b mu=b/a</td>
 *       <td>b=mu*a: length of second half axis</td>
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
 *       <td>\b nu=c/a</td>
 *       <td>c=nu*a: length of third half axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eta_p</td>
 *       <td>scattering length density of particle</td>
 *      </tr><tr>
 *       <td>\b eta_m</td>
 *       <td>scattering length density of matrix</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_parallelepiped_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_parallelepiped_opo_random(scalar q, sasfit_param * p);

/**
 * \ingroup ff_parallelepiped_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_parallelepiped_opo_random_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_parallelepiped_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_parallelepiped_opo_random_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_parallelepiped_opo_random ################ */

/* ################ start ff_ellipsoid_opo ################ */
/**
 * \defgroup ff_ellipsoid_opo ellipsoid (OPO)
 * \ingroup ff_plugins_oriented_primitive_objects
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
 *       <td>length of first half axis</td>
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
 *       <td>length of second half axis</td>
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
 *       <td>length of third half axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
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
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_ellipsoid_opo(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellipsoid_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_ellipsoid_opo_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellipsoid_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_ellipsoid_opo_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ellipsoid_opo ################ */

/* ################ start ff_ellipsoid_opo_random ################ */
/**
 * \defgroup ff_ellipsoid_opo_random ellipsoid (OPO,random)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief \<some brief description of ellipsoid (OPO,random) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first half axis</td>
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
 *       <td>length of second half axis</td>
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
 *       <td>length of third half axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eta_p</td>
 *       <td>scattering length density of particle</td>
 *      </tr><tr>
 *       <td>\b eta_m</td>
 *       <td>scattering length density of matrix</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ellipsoid_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_ellipsoid_opo_random(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellipsoid_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_ellipsoid_opo_random_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellipsoid_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_ellipsoid_opo_random_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ellipsoid_opo_random ################ */

/* ################ start ff_superegg_opo ################ */
/**
 * \defgroup ff_superegg_opo superegg (OPO)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief some brief description of superegg (OPO) function
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first half axis</td>
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
 *       <td>\b mu=b/a</td>
 *       <td>b=mu*a: length of second half axis</td>
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
 *       <td>\b nu=c/a=</td>
 *       <td>c=nu*a: length of third half axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
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
 *       <td>\b eps1</td>
 *       <td>shape parameter (eps1>0, eps1=1:circular, eps1<1:rectangular)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_superegg_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_superegg_opo(scalar q, sasfit_param * p);

/**
 * \ingroup ff_superegg_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_superegg_opo_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_superegg_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_superegg_opo_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_superegg_opo ################ */

/* ################ start ff_superegg_opo_random ################ */
/**
 * \defgroup ff_superegg_opo_random superegg (OPO,random)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief some brief description of superegg (OPO,random) function
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first half axis</td>
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
 *       <td>\b mu=b/a=</td>
 *       <td>b=nu*a: length of second half axis</td>
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
 *       <td>\b nu=c/a=</td>
 *       <td>c=nu*a: length of third half axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eta_p</td>
 *       <td>scattering length density of particle</td>
 *      </tr><tr>
 *       <td>\b eta_m</td>
 *       <td>scattering length density of matrix</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eps1</td>
 *       <td>shape parameter (eps1>0, eps1=1:circular, eps1<1:rectangular)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_superegg_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_superegg_opo_random(scalar q, sasfit_param * p);

/**
 * \ingroup ff_superegg_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_superegg_opo_random_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_superegg_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_superegg_opo_random_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_superegg_opo_random ################ */

/* ################ start ff_superellipsoid_opo ################ */
/**
 * \defgroup ff_superellipsoid_opo super ellipsoid (OPO)
 * \ingroup ff_plugins_oriented_primitive_objects
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
 *       <td>length of first half axis</td>
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
 *       <td>\b mu=b/a</td>
 *       <td>b=mu*a: length of second half axis</td>
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
 *       <td>\b nu</td>
 *       <td>c=nu*a: length of third half axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
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
 *       <td>\b eps_2</td>
 *       <td>shape parameter, ex-ey (eps_2>0)</td>
 *      </tr><tr>
 *       <td>\b eps_1</td>
 *       <td>shape parameter, ez (eps_1>0)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_superellipsoid_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_superellipsoid_opo(scalar q, sasfit_param * p);

/**
 * \ingroup ff_superellipsoid_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_superellipsoid_opo_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_superellipsoid_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_superellipsoid_opo_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_superellipsoid_opo ################ */

/* ################ start ff_superellipsoid_opo_random ################ */
/**
 * \defgroup ff_superellipsoid_opo_random super ellipsoid (OPO,random)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief \<some brief description of super ellipsoid (OPO,random) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first half axis</td>
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
 *       <td>\b mu=b/a</td>
 *       <td>b=mu*a: length of second half axis</td>
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
 *       <td>\b nu</td>
 *       <td>c=nu*a: length of third half axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eta_p</td>
 *       <td>scattering length density of particle</td>
 *      </tr><tr>
 *       <td>\b eta_m</td>
 *       <td>scattering length density of matrix</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eps_2</td>
 *       <td>shape parameter, ex-ey (eps_2>0)</td>
 *      </tr><tr>
 *       <td>\b eps_1</td>
 *       <td>shape parameter, ez (eps_1>0)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_superellipsoid_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_superellipsoid_opo_random(scalar q, sasfit_param * p);

/**
 * \ingroup ff_superellipsoid_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_superellipsoid_opo_random_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_superellipsoid_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_superellipsoid_opo_random_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_superellipsoid_opo_random ################ */

/* ################ start ff_superquadrics_opo ################ */
/**
 * \defgroup ff_superquadrics_opo superquadrics (OPO)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief some brief description of superquadrics (OPO) function
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first half axis</td>
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
 *       <td>length of second half axis</td>
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
 *       <td>length of third half axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
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
 * \ingroup ff_superquadrics_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_superquadrics_opo(scalar q, sasfit_param * p);

/**
 * \ingroup ff_superquadrics_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_superquadrics_opo_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_superquadrics_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_superquadrics_opo_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_superquadrics_opo ################ */

/* ################ start ff_superquadrics_opo_random ################ */
/**
 * \defgroup ff_superquadrics_opo_random superquadrics (OPO,random)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief some brief description of superquadrics (OPO,random) function
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first half axis</td>
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
 *       <td>length of second half axis</td>
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
 *       <td>length of third half axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eta_p</td>
 *       <td>scattering length density of particle</td>
 *      </tr><tr>
 *       <td>\b eta_m</td>
 *       <td>scattering length density of matrix</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td>shape parameter, ex-ey (p>0)</td>
 *      </tr><tr>
 *       <td>\b q</td>
 *       <td>shape parameter, ez (q>0)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_superquadrics_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_superquadrics_opo_random(scalar q, sasfit_param * p);

/**
 * \ingroup ff_superquadrics_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_superquadrics_opo_random_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_superquadrics_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_superquadrics_opo_random_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_superquadrics_opo_random ################ */

/* ################ start ff_supershape_opo ################ */
/**
 * \defgroup ff_supershape_opo super shape (OPO)
 * \ingroup ff_plugins_oriented_primitive_objects
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
 *       <td>length of first half axis</td>
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
 *       <td>length of second half axis</td>
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
 *       <td>length of third half axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
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
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_supershape_opo(scalar q, sasfit_param * p);

/**
 * \ingroup ff_supershape_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_supershape_opo_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_supershape_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_supershape_opo_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_supershape_opo ################ */

/* ################ start ff_supershape_opo_random ################ */
/**
 * \defgroup ff_supershape_opo_random super shape (OPO,random)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief \<some brief description of super shape (OPO,random) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first half axis</td>
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
 *       <td>length of second half axis</td>
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
 *       <td>length of third half axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eta_p</td>
 *       <td>scattering length density of particle</td>
 *      </tr><tr>
 *       <td>\b eta_m</td>
 *       <td>scattering length density of matrix</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy/td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
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
 * \ingroup ff_supershape_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_supershape_opo_random(scalar q, sasfit_param * p);

/**
 * \ingroup ff_supershape_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_supershape_opo_random_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_supershape_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_supershape_opo_random_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_supershape_opo_random ################ */

/* ################ start ff_supertoroid_helix_opo ################ */
/**
 * \defgroup ff_supertoroid_helix_opo super shaped toroid/helix (OPO)
 * \ingroup ff_plugins_oriented_primitive_objects
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
 *       <td>length of first half axis</td>
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
 *       <td>length of second half axis</td>
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
 *       <td>length of third half axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
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
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_supertoroid_helix_opo(scalar q, sasfit_param * p);

/**
 * \ingroup ff_supertoroid_helix_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_supertoroid_helix_opo_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_supertoroid_helix_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_supertoroid_helix_opo_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_supertoroid_helix_opo ################ */

/* ################ start ff_supertoroid_helix_opo_random ################ */
/**
 * \defgroup ff_supertoroid_helix_opo_random super shaped toroid/helix (OPO,random)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief \<some brief description of super shaped toroid/helix (OPO,random) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first half axis</td>
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
 *       <td>length of second half axis</td>
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
 *       <td>length of third half axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eta_p</td>
 *       <td>scattering length density of particle</td>
 *      </tr><tr>
 *       <td>\b eta_m</td>
 *       <td>scattering length density of matrix</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
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
 * \ingroup ff_supertoroid_helix_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_supertoroid_helix_opo_random(scalar q, sasfit_param * p);

/**
 * \ingroup ff_supertoroid_helix_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_supertoroid_helix_opo_random_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_supertoroid_helix_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_supertoroid_helix_opo_random_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_supertoroid_helix_opo_random ################ */

/* ################ start ff_rationalsupershape_opo ################ */
/**
 * \defgroup ff_rationalsupershape_opo rational super shape (OPO)
 * \ingroup ff_plugins_oriented_primitive_objects
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
 *       <td>length of first half axis</td>
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
 *       <td>length of second half axis</td>
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
 *       <td>length of third half axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
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
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_rationalsupershape_opo(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rationalsupershape_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_rationalsupershape_opo_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rationalsupershape_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_rationalsupershape_opo_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rationalsupershape_opo ################ */

/* ################ start ff_rationalsupershape_opo_random ################ */
/**
 * \defgroup ff_rationalsupershape_opo_random rational super shape (OPO,random)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief \<some brief description of rational super shape (OPO,random) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first half axis</td>
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
 *       <td>length of second half axis</td>
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
 *       <td>length of third half axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eta_p</td>
 *       <td>scattering length density of particle</td>
 *      </tr><tr>
 *       <td>\b eta_m</td>
 *       <td>scattering length density of matrix</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
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
 * \ingroup ff_rationalsupershape_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_rationalsupershape_opo_random(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rationalsupershape_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_rationalsupershape_opo_random_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rationalsupershape_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_rationalsupershape_opo_random_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rationalsupershape_opo_random ################ */

/* ################ start ff_tetrahedra__opo_ ################ */
/**
 * \defgroup ff_tetrahedra__opo_ Tetrahedron (OPO)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief \<some brief description of Tetrahedron (OPO) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first half axis</td>
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
 *       <td>length of second half axis</td>
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
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
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
 *       <td>\b tilt</td>
 *       <td>tilt angle of conical side wall of unit cone</td>
 *      </tr><tr>
 *       <td>\b H_R</td>
 *       <td>height of unit 6-fold symmetric cone</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_tetrahedra__opo_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_tetrahedra__opo_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_tetrahedra__opo_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_tetrahedra__opo__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_tetrahedra__opo_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_tetrahedra__opo__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_tetrahedra__opo_ ################ */

/* ################ start ff_tetrahedra__opo_random_ ################ */
/**
 * \defgroup ff_tetrahedra__opo_random_ Tetrahedron (OPO,random)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief \<some brief description of Tetrahedron (OPO,random) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first half axis</td>
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
 *       <td>length of second half axis</td>
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
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eta_p</td>
 *       <td>scattering length density of particle</td>
 *      </tr><tr>
 *       <td>\b eta_m</td>
 *       <td>scattering length density of matrix</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b tilt</td>
 *       <td>tilt angle of conical side wall of unit cone</td>
 *      </tr><tr>
 *       <td>\b H_R</td>
 *       <td>height of unit 6-fold symmetric cone</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_tetrahedra__opo_random_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_tetrahedra__opo_random_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_tetrahedra__opo_random_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_tetrahedra__opo_random__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_tetrahedra__opo_random_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_tetrahedra__opo_random__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_tetrahedra__opo_random_ ################ */

/* ################ start ff_octahedra__opo_ ################ */
/**
 * \defgroup ff_octahedra__opo_ Octahedron (OPO)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief \<some brief description of Octahedron (OPO) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first half axis</td>
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
 *       <td>\b mu=b/a</td>
 *       <td>b=mu*a: length of second half axis</td>
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
 *       <td>\b nu=c/a</td>
 *       <td>c=nu*a: length of third half axis</td>
 *      </tr><tr>
 *       <td>\b ec_x</td>
 *       <td>x component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_y</td>
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
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
 * \ingroup ff_octahedra__opo_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_octahedra__opo_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_octahedra__opo_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_octahedra__opo__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_octahedra__opo_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_octahedra__opo__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_octahedra__opo_ ################ */

/* ################ start ff_octahedra__opo_random_ ################ */
/**
 * \defgroup ff_octahedra__opo_random_ Octahedron (OPO,random)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief \<some brief description of Octahedron (OPO,random) function\>
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
 *       <td>\b mu=b/a</td>
 *       <td>b=mu*a: length of second half axis</td>
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
 *       <td>\b nu=c/a</td>
 *       <td>c=nu*a: length of third half axis</td>
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
 *      </tr></table>
 */

/**
 * \ingroup ff_octahedra__opo_random_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_octahedra__opo_random_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_octahedra__opo_random_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_octahedra__opo_random__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_octahedra__opo_random_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_octahedra__opo_random__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_octahedra__opo_random_ ################ */

/* ################ start ff_rhombic_dodecahedra__opo_ ################ */
/**
 * \defgroup ff_rhombic_dodecahedra__opo_ rhombic Dodecahedron (OPO)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief \<some brief description of rhombic Dodecahedron (OPO) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first half axis</td>
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
 *       <td>length of second half axis</td>
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
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
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
 * \ingroup ff_rhombic_dodecahedra__opo_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_rhombic_dodecahedra__opo_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rhombic_dodecahedra__opo_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_rhombic_dodecahedra__opo__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rhombic_dodecahedra__opo_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_rhombic_dodecahedra__opo__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rhombic_dodecahedra__opo_ ################ */

/* ################ start ff_rhombic_dodecahedra__opo_random_ ################ */
/**
 * \defgroup ff_rhombic_dodecahedra__opo_random_ rhombic Dodecahedron (OPO,random)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief \< some brief description of rhombic Dodecahedron (OPO,random) function \>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first half axis</td>
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
 *       <td>length of second half axis</td>
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
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eta_p</td>
 *       <td>scattering length density of particle</td>
 *      </tr><tr>
 *       <td>\b eta_m</td>
 *       <td>scattering length density of matrix</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_rhombic_dodecahedra__opo_random_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_rhombic_dodecahedra__opo_random_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rhombic_dodecahedra__opo_random_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_rhombic_dodecahedra__opo_random__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rhombic_dodecahedra__opo_random_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_rhombic_dodecahedra__opo_random__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rhombic_dodecahedra__opo_random_ ################ */

/* ################ start ff_icosahedra__opo_ ################ */
/**
 * \defgroup ff_icosahedra__opo_ Icosahedron (OPO)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief \<some brief description of Icosahedron (OPO) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_icosahedra__opo_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_icosahedra__opo_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_icosahedra__opo_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_icosahedra__opo__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_icosahedra__opo_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_icosahedra__opo__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_icosahedra__opo_ ################ */

/* ################ start ff_icosahedra__opo_random_ ################ */
/**
 * \defgroup ff_icosahedra__opo_random_ Icosahedron (OPO,random)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief \<some brief description of Icosahedron (OPO,random) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_icosahedra__opo_random_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_icosahedra__opo_random_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_icosahedra__opo_random_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_icosahedra__opo_random__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_icosahedra__opo_random_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_icosahedra__opo_random__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_icosahedra__opo_random_ ################ */

/* ################ start ff_hexahedra__opo_ ################ */
/**
 * \defgroup ff_hexahedra__opo_ Hexahedron (OPO)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief \<some brief description of Hexahedron (OPO) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_hexahedra__opo_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_hexahedra__opo_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_hexahedra__opo_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_hexahedra__opo__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_hexahedra__opo_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_hexahedra__opo__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_hexahedra__opo_ ################ */

/* ################ start ff_hexahedra__opo_random_ ################ */
/**
 * \defgroup ff_hexahedra__opo_random_ Hexahedron (OPO,random)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief \<some brief description of Hexahedron (OPO,random) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_hexahedra__opo_random_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_hexahedra__opo_random_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_hexahedra__opo_random_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_hexahedra__opo_random__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_hexahedra__opo_random_
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_hexahedra__opo_random__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_hexahedra__opo_random_ ################ */

/* ################ start ff_cone__opo_ ################ */
/**
 * \defgroup ff_cone_opo cone (OPO)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief \<some brief description of cone (OPO) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first half axis</td>
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
 *       <td>length of second half axis</td>
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
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
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
 *       <td>\b tilt</td>
 *       <td>tilt angle of conical side wall of unit cone</td>
 *      </tr><tr>
 *       <td>\b H_R</td>
 *       <td>hight of unit cone</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_cone_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_cone_opo(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cone_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_cone_opo_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cone_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_cone_opo_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_cone_opo ################ */

/* ################ start ff_cone_opo_random ################ */
/**
 * \defgroup ff_cone_opo_random cone (OPO,random)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief \<some brief description of cone (OPO,random) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first half axis</td>
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
 *       <td>length of second half axis</td>
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
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eta_p</td>
 *       <td>scattering length density of particle</td>
 *      </tr><tr>
 *       <td>\b eta_m</td>
 *       <td>scattering length density of matrix</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b tilt</td>
 *       <td>tilt angle of conical side wall of unit cone</td>
 *      </tr><tr>
 *       <td>\b H_R</td>
 *       <td>hight of unit cone</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_cone_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_cone_opo_random(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cone_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_cone_opo_random_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cone_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_cone_opo_random_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_cone_opo_random ################ */

/* ################ start ff_pyramid4__opo_ ################ */
/**
 * \defgroup ff_pyramid4_opo pyramid4 (OPO)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief \<some brief description of pyramid4 (OPO) function\>
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
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
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
 *       <td>\b tilt</td>
 *       <td>tilt angle of conical side wall of unit pyramid4</td>
 *      </tr><tr>
 *       <td>\b H_R</td>
 *       <td>hight of unit pyramid4</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_pyramid4_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_pyramid4_opo(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pyramid4_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_pyramid4_opo_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pyramid4_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_pyramid4_opo_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_pyramid4_opo ################ */

/* ################ start ff_pyramid4_opo_random ################ */
/**
 * \defgroup ff_pyramid4_opo_random pyramid4 (OPO,random)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief \<some brief description of pyramid4 (OPO,random) function\>
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
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eta_p</td>
 *       <td>scattering length density of particle</td>
 *      </tr><tr>
 *       <td>\b eta_m</td>
 *       <td>scattering length density of matrix</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b tilt</td>
 *       <td>tilt angle of conical side wall of unit pyramid4</td>
 *      </tr><tr>
 *       <td>\b H_R</td>
 *       <td>hight of unit pyramid4</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_pyramid4_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_pyramid4_opo_random(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pyramid4_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_pyramid4_opo_random_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pyramid4_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_pyramid4_opo_random_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_pyramid4_opo_random ################ */

/* ################ start ff_cone6_opo_ ################ */
/**
 * \defgroup ff_cone6_opo cone6 (OPO)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief \<some brief description of cone6 (OPO) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first half axis</td>
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
 *       <td>length of second half axis</td>
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
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
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
 *       <td>\b tilt</td>
 *       <td>tilt angle of conical side wall of unit cone</td>
 *      </tr><tr>
 *       <td>\b H_R</td>
 *       <td>height of unit 6-fold symmetric cone</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_cone6_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_cone6_opo(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cone6_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_cone6_opo_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cone6_opo
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_cone6_opo_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_cone6_opo ################ */

/* ################ start ff_cone6_opo_random ################ */
/**
 * \defgroup ff_cone6_opo_random cone6 (OPO,random)
 * \ingroup ff_plugins_oriented_primitive_objects
 *
 * \brief \<some brief description of cone6 (OPO,random) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>length of first half axis</td>
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
 *       <td>length of second half axis</td>
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
 *       <td>y component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b ec_z</td>
 *       <td>z component of third axis (will be normalized to 1)</td>
 *      </tr><tr>
 *       <td>\b eta_p</td>
 *       <td>scattering length density of particle</td>
 *      </tr><tr>
 *       <td>\b eta_m</td>
 *       <td>scattering length density of matrix</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b tilt</td>
 *       <td>tilt angle of conical side wall of unit cone</td>
 *      </tr><tr>
 *       <td>\b H_R</td>
 *       <td>height of unit 6-fold symmetric cone</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_cone6_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_cone6_opo_random(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cone6_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_cone6_opo_random_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cone6_opo_random
 *
 * \sa sasfit_oriented_primitive_objects.h, ff_plugins_oriented_primitive_objects
 */
sasfit_oriented_primitive_objects_DLLEXP scalar sasfit_ff_cone6_opo_random_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_cone6_opo_random ################ */

#endif // this file

