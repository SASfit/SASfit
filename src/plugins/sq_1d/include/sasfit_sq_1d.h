/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_SQ_1D_H
#define SASFIT_PLUGIN_SQ_1D_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_sq_1d.h
 * Public available functions and descriptions of the sq_1d plugin.
 */

/**
 * \def sasfit_sq_1d_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_sq_1d_EXPORTS)
	#ifdef sasfit_sq_1d_DLLEXP
	#undef sasfit_sq_1d_DLLEXP
	#endif
	#define sasfit_sq_1d_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_sq_1d_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_sq_1d_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq_paracrystal_1d_random ################ */
/**
 * \defgroup sq_paracrystal_1d_random 1D paracrystal (random)
 * \ingroup sq_plugins_hard_sphere
 *
 * \brief \<some brief description of 1D paracrystal (random) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b D</td>
 *       <td>next neighbour center-to-center distance</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>Gaussian standard deviation of stacking parameter D</td>
 *      </tr><tr>
 *       <td>\b N</td>
 *       <td>number of stacked particles</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_paracrystal_1d_random
 *
 * \sa sasfit_sq_1d.h, sq_plugins_hard_sphere
 */
sasfit_sq_1d_DLLEXP scalar sasfit_sq_paracrystal_1d_random(scalar q, sasfit_param * p);

/**
 * \ingroup sq_paracrysal_one_d_random
 *
 * \sa sasfit_sq_1d.h, sq_plugins_hard_sphere
 */
sasfit_sq_1d_DLLEXP scalar sasfit_sq_paracrystal_1d_random_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_paracrystal_1d_random
 *
 * \sa sasfit_sq_1d.h, sq_plugins_hard_sphere
 */
sasfit_sq_1d_DLLEXP scalar sasfit_sq_paracrystal_1d_random_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_paracrystal_1d_random ################ */


/* ################ start sq_paracrystal_1d_aligned ################ */
/**
 * \defgroup sq_paracrystal_1d_aligned 1D paracrystal (aligned)
 * \ingroup sq_plugins_hard_sphere
 *
 * \brief \<some brief description of 1D paracrystal (aligned) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b D</td>
 *       <td>next neighbour center-to-center distance</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>Gaussian standard deviation of stacking parameter D</td>
 *      </tr><tr>
 *       <td>\b N</td>
 *       <td>number of stacked particles</td>
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
 * \ingroup sq_paracrystal_1d_aligned
 *
 * \sa sasfit_sq_1d.h, sq_plugins_hard_sphere
 */
sasfit_sq_1d_DLLEXP scalar sasfit_sq_paracrystal_1d_aligned(scalar q, sasfit_param * p);

/**
 * \ingroup sq_paracrystal_1d_aligned
 *
 * \sa sasfit_sq_1d.h, sq_plugins_hard_sphere
 */
sasfit_sq_1d_DLLEXP scalar sasfit_sq_paracrystal_1d_aligned_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_paracrystal_1d_aligned
 *
 * \sa sasfit_sq_1d.h, sq_plugins_hard_sphere
 */
sasfit_sq_1d_DLLEXP scalar sasfit_sq_paracrystal_1d_aligned_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_paracrystal_1d_aligned ################ */

/* ################ start sq_paracrystal_orig_1d_random ################ */
/**
 * \defgroup sq_paracrystal_orig_1d_random 1D paracrystal (orig, random)
 * \ingroup sq_plugins_hard_sphere
 *
 * \brief \<some brief description of 1D paracrystal (orig, random) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b D</td>
 *       <td>next neighbour center-to-center distance</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>Gaussian standard deviation of stacking parameter D</td>
 *      </tr><tr>
 *       <td>\b N</td>
 *       <td>number of stacked particles</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_paracrystal_orig_1d_random
 *
 * \sa sasfit_sq_1d.h, sq_plugins_hard_sphere
 */
sasfit_sq_1d_DLLEXP scalar sasfit_sq_paracrystal_orig_1d_random(scalar q, sasfit_param * p);

/**
 * \ingroup sq_paracrystal_orig_1d_random
 *
 * \sa sasfit_sq_1d.h, sq_plugins_hard_sphere
 */
sasfit_sq_1d_DLLEXP scalar sasfit_sq_paracrystal_orig_1d_random_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_paracrystal_orig_1d_random
 *
 * \sa sasfit_sq_1d.h, sq_plugins_hard_sphere
 */
sasfit_sq_1d_DLLEXP scalar sasfit_sq_paracrystal_orig_1d_random_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_paracrystal_orig_1d_random ################ */


/* ################ start sq_paracrystal_orig_1d_aligned ################ */
/**
 * \defgroup sq_paracrystal_orig_1d_aligned 1D paracrystal (orig, aligned)
 * \ingroup sq_plugins_hard_sphere
 *
 * \brief \<some brief description of 1D paracrystal (orig, aligned) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b D</td>
 *       <td>next neighbour center-to-center distance</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>Gaussian standard deviation of stacking parameter D</td>
 *      </tr><tr>
 *       <td>\b N</td>
 *       <td>number of stacked particles</td>
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
 * \ingroup sq_paracrystal_orig_1d_aligned
 *
 * \sa sasfit_sq_1d.h, sq_plugins_hard_sphere
 */
sasfit_sq_1d_DLLEXP scalar sasfit_sq_paracrystal_orig_1d_aligned(scalar q, sasfit_param * p);

/**
 * \ingroup sq_paracrystal__orig1d_aligned
 *
 * \sa sasfit_sq_1d.h, sq_plugins_hard_sphere
 */
sasfit_sq_1d_DLLEXP scalar sasfit_sq_paracrystal_orig_1d_aligned_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_paracrystal_orig_1d_aligned
 *
 * \sa sasfit_sq_1d.h, sq_plugins_hard_sphere
 */
sasfit_sq_1d_DLLEXP scalar sasfit_sq_paracrystal_orig_1d_aligned_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_paracrystal_orig_1d_aligned ################ */



#endif // this file

