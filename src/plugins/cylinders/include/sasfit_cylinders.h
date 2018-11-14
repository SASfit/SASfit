/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_CYLINDERS_H
#define SASFIT_PLUGIN_CYLINDERS_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_cylinders.h
 * Public available functions and descriptions of the cylinders plugin.
 */

/**
 * \def sasfit_cylinders_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_cylinders_EXPORTS)
	#ifdef sasfit_cylinders_DLLEXP
	#undef sasfit_cylinders_DLLEXP
	#endif
	#define sasfit_cylinders_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_cylinders_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_cylinders_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_rod ################ */
/**
 * \defgroup ff_rod Rod
 * \ingroup ff_plugins_cylindrical_obj
 *
 * \brief \<some brief description of Rod function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b L</td>
 *       <td>length of rod</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_rod
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_rod(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_rod_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_rod_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rod ################ */

/* ################ start ff_disc ################ */
/**
 * \defgroup ff_disc Disc
 * \ingroup ff_plugins_cylindrical_obj
 *
 * \brief \<some brief description of Disc function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>disc radius</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_disc
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_disc(scalar q, sasfit_param * p);

/**
 * \ingroup ff_disc
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_disc_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_disc
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_disc_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_disc ################ */

/* ################ start ff_porodcylinder ################ */
/**
 * \defgroup ff_porodcylinder PorodCylinder
 * \ingroup ff_plugins_cylindrical_obj
 *
 * \brief \<some brief description of PorodCylinder function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>cylinder radius</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>cylinder length</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_porodcylinder
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_porodcylinder(scalar q, sasfit_param * p);

/**
 * \ingroup ff_porodcylinder
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_porodcylinder_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_porodcylinder
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_porodcylinder_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_porodcylinder ################ */

/* ################ start ff_longcylinder ################ */
/**
 * \defgroup ff_longcylinder LongCylinder
 * \ingroup ff_plugins_cylindrical_obj
 *
 * \brief \<some brief description of LongCylinder function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>cylinder radius</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>cylinder length</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_longcylinder
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_longcylinder(scalar q, sasfit_param * p);

/**
 * \ingroup ff_longcylinder
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_longcylinder_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_longcylinder
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_longcylinder_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_longcylinder ################ */

/* ################ start ff_flatcylinder ################ */
/**
 * \defgroup ff_flatcylinder FlatCylinder
 * \ingroup ff_plugins_cylindrical_obj
 *
 * \brief \<some brief description of FlatCylinder function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>cylinder radius</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>cylinder length</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_flatcylinder
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_flatcylinder(scalar q, sasfit_param * p);

/**
 * \ingroup ff_flatcylinder
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_flatcylinder_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_flatcylinder
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_flatcylinder_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_flatcylinder ################ */

/* ################ start ff_cylinder ################ */
/**
 * \defgroup ff_cylinder Cylinder
 * \ingroup ff_plugins_cylindrical_obj
 *
 * \brief \<some brief description of Cylinder function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>cylinder radius</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>cylinder length</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_cylinder
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_cylinder(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylinder
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_cylinder_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylinder
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_cylinder_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_cylinder ################ */

/* ################ start ff_longcylshell ################ */
/**
 * \defgroup ff_longcylshell LongCylShell
 * \ingroup ff_plugins_cylindrical_obj
 *
 * \brief \<some brief description of LongCylShell function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>cylinder radius</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>cylinder length</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_longcylshell
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_longcylshell(scalar q, sasfit_param * p);

/**
 * \ingroup ff_longcylshell
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_longcylshell_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_longcylshell
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_longcylshell_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_longcylshell ################ */

/* ################ start ff_ellcylshell1 ################ */
/**
 * \defgroup ff_ellcylshell1 ellCylShell1
 * \ingroup ff_plugins_cylindrical_obj
 *
 * \brief \<some brief description of ellCylShell1 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>cylinder radius</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b epsilon</td>
 *       <td>eccentricity of ellipsoid</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>cylinder length</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>ing length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of core</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ellcylshell1
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_ellcylshell1(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellcylshell1
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_ellcylshell1_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellcylshell1
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_ellcylshell1_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ellcylshell1 ################ */

/* ################ start ff_ellcylshell2 ################ */
/**
 * \defgroup ff_ellcylshell2 ellCylShell2
 * \ingroup ff_plugins_cylindrical_obj
 *
 * \brief \<some brief description of ellCylShell2 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>cylinder radius</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b epsilon</td>
 *       <td>eccentricity of ellipsoid</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>cylinder length</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>ing length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ellcylshell2
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_ellcylshell2(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellcylshell2
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_ellcylshell2_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellcylshell2
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_ellcylshell2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ellcylshell2 ################ */

/* ################ start ff_cylshell1 ################ */
/**
 * \defgroup ff_cylshell1 CylShell1
 * \ingroup ff_plugins_cylindrical_obj
 *
 * \brief \<some brief description of CylShell1 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>cylinder radius</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>cylinder length</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>ing length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_cylshell1
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_cylshell1(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylshell1
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_cylshell1_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylshell1
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_cylshell1_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_cylshell1 ################ */

/* ################ start ff_cylshell2 ################ */
/**
 * \defgroup ff_cylshell2 CylShell2
 * \ingroup ff_plugins_cylindrical_obj
 *
 * \brief \<some brief description of CylShell2 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>cylinder radius</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>cylinder length</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>ing length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_cylshell2
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_cylshell2(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylshell2
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_cylshell2_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylshell2
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_cylshell2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_cylshell2 ################ */

/* ################ start ff_alignedcylshell ################ */
/**
 * \defgroup ff_alignedcylshell alignedCylShell
 * \ingroup ff_plugins_cylindrical_obj
 *
 * \brief \<some brief description of alignedCylShell function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>cylinder radius</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>cylinder length</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>ing length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b phi</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_alignedcylshell
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_alignedcylshell(scalar q, sasfit_param * p);

/**
 * \ingroup ff_alignedcylshell
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_alignedcylshell_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_alignedcylshell
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_alignedcylshell_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_alignedcylshell ################ */

/* ################ start ff_torus ################ */
/**
 * \defgroup ff_torus torus
 * \ingroup ff_plugins_cylindrical_obj
 *
 * \brief \<some brief description of torus function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>distance from the center of the tube to the center of the torus</td>
 *      </tr><tr>
 *       <td>\b a</td>
 *       <td>radius of the tube core</td>
 *      </tr><tr>
 *       <td>\b Delta_a</td>
 *       <td>shell thickness of tube shell</td>
 *      </tr><tr>
 *       <td>\b nu</td>
 *       <td>eccentricity of elliptical torus cross-section</td>
 *      </tr><tr>
 *       <td>\b eta_c</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_torus
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_torus(scalar q, sasfit_param * p);

/**
 * \ingroup ff_torus
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_torus_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_torus
 *
 * \sa sasfit_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_cylinders_DLLEXP scalar sasfit_ff_torus_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_torus ################ */


#endif // this file

