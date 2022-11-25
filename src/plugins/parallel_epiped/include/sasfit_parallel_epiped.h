/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_PARALLEL_EPIPED_H
#define SASFIT_PLUGIN_PARALLEL_EPIPED_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_parallel_epiped.h
 * Public available functions and descriptions of the parallel_epiped plugin.
 */

/**
 * \def sasfit_parallel_epiped_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_parallel_epiped_EXPORTS)
	#ifdef sasfit_parallel_epiped_DLLEXP
	#undef sasfit_parallel_epiped_DLLEXP
	#endif
	#define sasfit_parallel_epiped_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_parallel_epiped_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_parallel_epiped_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_parallelepiped_abc ################ */
/** 
 * \defgroup ff_parallelepiped_abc Parallelepiped_abc
 * \ingroup ff_plugins_polyhedra
 *
 * \brief \<some brief description of Parallelepiped_abc function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>first side length</td>
 *      </tr><tr>
 *       <td>\b B</td>
 *       <td>second side length</td>
 *      </tr><tr>
 *       <td>\b C</td>
 *       <td>third side length</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_parallelepiped_abc
 *
 * \sa sasfit_parallel_epiped.h, ff_plugins_polyhedra
 */
sasfit_parallel_epiped_DLLEXP scalar sasfit_ff_parallelepiped_abc(scalar q, sasfit_param * p);

/**
 * \ingroup ff_parallelepiped_abc
 *
 * \sa sasfit_parallel_epiped.h, ff_plugins_polyhedra
 */
sasfit_parallel_epiped_DLLEXP scalar sasfit_ff_parallelepiped_abc_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_parallelepiped_abc
 *
 * \sa sasfit_parallel_epiped.h, ff_plugins_polyhedra
 */
sasfit_parallel_epiped_DLLEXP scalar sasfit_ff_parallelepiped_abc_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_parallelepiped_abc ################ */

/* ################ start ff_parallelepiped_abc1 ################ */
/** 
 * \defgroup ff_parallelepiped_abc1 Parallelepiped_abc1
 * \ingroup ff_plugins_polyhedra
 *
 * \brief \<some brief description of Parallelepiped_abc1 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b nu*a</td>
 *       <td>first side length with size distribution</td>
 *      </tr><tr>
 *       <td>\b B</td>
 *       <td>second side length</td>
 *      </tr><tr>
 *       <td>\b C</td>
 *       <td>third side length</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>lognormal width</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_parallelepiped_abc1
 *
 * \sa sasfit_parallel_epiped.h, ff_plugins_polyhedra
 */
sasfit_parallel_epiped_DLLEXP scalar sasfit_ff_parallelepiped_abc1(scalar q, sasfit_param * p);

/**
 * \ingroup ff_parallelepiped_abc1
 *
 * \sa sasfit_parallel_epiped.h, ff_plugins_polyhedra
 */
sasfit_parallel_epiped_DLLEXP scalar sasfit_ff_parallelepiped_abc1_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_parallelepiped_abc1
 *
 * \sa sasfit_parallel_epiped.h, ff_plugins_polyhedra
 */
sasfit_parallel_epiped_DLLEXP scalar sasfit_ff_parallelepiped_abc1_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_parallelepiped_abc1 ################ */

/* ################ start ff_parallelepiped_abc2 ################ */
/** 
 * \defgroup ff_parallelepiped_abc2 Parallelepiped_abc2
 * \ingroup ff_plugins_polyhedra
 *
 * \brief \<some brief description of Parallelepiped_abc function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b nu*a</td>
 *       <td>first side length with size distribution</td>
 *      </tr><tr>
 *       <td>\b nu*b</td>
 *       <td>second side length with size distribution</td>
 *      </tr><tr>
 *       <td>\b C</td>
 *       <td>third side length</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>lognormal width</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_parallelepiped_abc2
 *
 * \sa sasfit_parallel_epiped.h, ff_plugins_polyhedra
 */
sasfit_parallel_epiped_DLLEXP scalar sasfit_ff_parallelepiped_abc2(scalar q, sasfit_param * p);

/**
 * \ingroup ff_parallelepiped_abc2
 *
 * \sa sasfit_parallel_epiped.h, ff_plugins_polyhedra
 */
sasfit_parallel_epiped_DLLEXP scalar sasfit_ff_parallelepiped_abc2_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_parallelepiped_abc2
 *
 * \sa sasfit_parallel_epiped.h, ff_plugins_polyhedra
 */
sasfit_parallel_epiped_DLLEXP scalar sasfit_ff_parallelepiped_abc2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_parallelepiped_abc2 ################ */

/* ################ start ff_parallelepiped_abc ################ */
/** 
 * \defgroup ff_parallelepiped_abc3 Parallelepiped_abc3
 * \ingroup ff_plugins_polyhedra
 *
 * \brief \<some brief description of Parallelepiped_abc function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b nu*a</td>
 *       <td>first side length with size distribution</td>
 *      </tr><tr>
 *       <td>\b nu*b</td>
 *       <td>second side length with size distribution</td>
 *      </tr><tr>
 *       <td>\b nu*c</td>
 *       <td>third side length with size distribution</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>lognormal width</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_parallelepiped_abc3
 *
 * \sa sasfit_parallel_epiped.h, ff_plugins_polyhedra
 */
sasfit_parallel_epiped_DLLEXP scalar sasfit_ff_parallelepiped_abc3(scalar q, sasfit_param * p);

/**
 * \ingroup ff_parallelepiped_abc3
 *
 * \sa sasfit_parallel_epiped.h, ff_plugins_polyhedra
 */
sasfit_parallel_epiped_DLLEXP scalar sasfit_ff_parallelepiped_abc3_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_parallelepiped_abc3
 *
 * \sa sasfit_parallel_epiped.h, ff_plugins_polyhedra
 */
sasfit_parallel_epiped_DLLEXP scalar sasfit_ff_parallelepiped_abc3_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_parallelepiped_abc3 ################ */


#endif // this file

