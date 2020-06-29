/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_HARD_SPHERES_1D_H
#define SASFIT_PLUGIN_HARD_SPHERES_1D_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_hard_spheres_1d.h
 * Public available functions and descriptions of the hard_spheres_1d plugin.
 */

/**
 * \def sasfit_hard_spheres_1d_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_hard_spheres_1d_EXPORTS)
	#ifdef sasfit_hard_spheres_1d_DLLEXP
	#undef sasfit_hard_spheres_1d_DLLEXP
	#endif
	#define sasfit_hard_spheres_1d_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_hard_spheres_1d_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_hard_spheres_1d_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_one_d_hard_spheres_aligned ################ */
/**
 * \defgroup ff_one_d_hard_spheres_aligned 1D hard spheres (aligned)
 * \ingroup sq_plugins_hard_sphere
 *
 * \brief \<some brief description of 1D hard spheres (aligned) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius of object</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>packing fraction as eta=2*R*n with number density n</td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td>azimuthal angle in deg between stacking direction and reference direction (for psi=0 reference dir.=x-dir.)</td>
 *      </tr><tr>
 *       <td>\b phi</td>
 *       <td>polar angle in deg between stacking direction and reference direction x (psi=0)</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>direction in deg of Q in the detector plane</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_one_d_hard_spheres_aligned
 *
 * \sa sasfit_hard_spheres_1d.h, sq_plugins_hard_sphere
 */
sasfit_hard_spheres_1d_DLLEXP scalar sasfit_ff_one_d_hard_spheres_aligned(scalar q, sasfit_param * p);

/**
 * \ingroup ff_one_d_hard_spheres_aligned
 *
 * \sa sasfit_hard_spheres_1d.h, sq_plugins_hard_sphere
 */
sasfit_hard_spheres_1d_DLLEXP scalar sasfit_ff_one_d_hard_spheres_aligned_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_one_d_hard_spheres_aligned
 *
 * \sa sasfit_hard_spheres_1d.h, sq_plugins_hard_sphere
 */
sasfit_hard_spheres_1d_DLLEXP scalar sasfit_ff_one_d_hard_spheres_aligned_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_one_d_hard_spheres_aligned ################ */

/* ################ start sq_one_d_hard_spheres_random_orient ################ */
/**
 * \defgroup sq_one_d_hard_spheres_random_orient 1D hard spheres (random orient.)
 * \ingroup sq_plugins_hard_sphere
 *
 * \brief \<some brief description of 1D hard spheres (random orient.) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius if object</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td> packing fraction as eta=2*R*n  with number density n</td>
 *      </tr></table>
 */

/**
 * \ingroup sq__d_hard_spheres__random_orient__
 *
 * \sa sasfit_hard_spheres_1d.h, sq_plugins_hard_sphere
 */
sasfit_hard_spheres_1d_DLLEXP scalar sasfit_sq_one_d_hard_spheres_random_orient(scalar q, sasfit_param * p);

/**
 * \ingroup sq_one_d_hard_spheres_random_orient
 *
 * \sa sasfit_hard_spheres_1d.h, sq_plugins_hard_sphere
 */
sasfit_hard_spheres_1d_DLLEXP scalar sasfit_sq_one_d_hard_spheres_random_orient_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_one_d_hard_spheres_random_orient
 *
 * \sa sasfit_hard_spheres_1d.h, sq_plugins_hard_sphere
 */
sasfit_hard_spheres_1d_DLLEXP scalar sasfit_sq_one_d_hard_spheres_random_orient_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_one_d_hard_spheres_random_orient ################ */


#endif // this file

