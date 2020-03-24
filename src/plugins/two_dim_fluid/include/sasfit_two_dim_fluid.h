/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_TWO_DIM_FLUID_H
#define SASFIT_PLUGIN_TWO_DIM_FLUID_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_two_dim_fluid.h
 * Public available functions and descriptions of the two_dim_fluid plugin.
 */

/**
 * \def sasfit_two_dim_fluid_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_two_dim_fluid_EXPORTS)
	#ifdef sasfit_two_dim_fluid_DLLEXP
	#undef sasfit_two_dim_fluid_DLLEXP
	#endif
	#define sasfit_two_dim_fluid_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_two_dim_fluid_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_two_dim_fluid_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq__d_hard_disks__rosenfeld_aligned ################ */
/**
 * \defgroup sq__d_hard_disks__rosenfeld_aligned 2D hard disks (Rosenfeld,aligned)
 * \ingroup sq_plugins_hard_sphere
 *
 * \brief \<some brief description of 2D hard disks (Rosenfeld,aligned) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius of the discs</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>surface coverage</td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td>azimuthal angle in deg between stacking direction and reference direction z (beam direction)</td>
 *      </tr><tr>
 *       <td>\b phi</td>
 *       <td>polar angle in deg between stacking direction and reference direction z (beam direction)</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>direction in deg of Q in the detector plane</td>
 *      </tr></table>
 */

/**
 * \ingroup sq__d_hard_disks__rosenfeld_aligned
 *
 * \sa sasfit_two_dim_fluid.h, sq_plugins_hard_sphere
 */
sasfit_two_dim_fluid_DLLEXP scalar sasfit_sq__d_hard_disks__rosenfeld_aligned(scalar q, sasfit_param * p);

/**
 * \ingroup sq__d_hard_disks__rosenfeld_aligned
 *
 * \sa sasfit_two_dim_fluid.h, sq_plugins_hard_sphere
 */
sasfit_two_dim_fluid_DLLEXP scalar sasfit_sq__d_hard_disks__rosenfeld_aligned_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq__d_hard_disks__rosenfeld_aligned
 *
 * \sa sasfit_two_dim_fluid.h, sq_plugins_hard_sphere
 */
sasfit_two_dim_fluid_DLLEXP scalar sasfit_sq__d_hard_disks__rosenfeld_aligned_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq__d_hard_disks__rosenfeld_aligned ################ */

/* ################ start sq__d_hard_disks__guo_aligned ################ */
/**
 * \defgroup sq__d_hard_disks__guo_aligned 2D hard disks (Guo,aligned)
 * \ingroup sq_plugins_hard_sphere
 *
 * \brief \<some brief description of 2D hard disks (Guo,aligned) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius of hard disks</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>surface coverage</td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td>azimuthal angle in deg between stacking direction and reference direction z (beam direction)</td>
 *      </tr><tr>
 *       <td>\b phi</td>
 *       <td>polar angle in deg between stacking direction and reference direction z (beam direction)</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>direction in deg of Q in the detector plane</td>
 *      </tr></table>
 */

/**
 * \ingroup sq__d_hard_disks__guo_aligned
 *
 * \sa sasfit_two_dim_fluid.h, sq_plugins_hard_sphere
 */
sasfit_two_dim_fluid_DLLEXP scalar sasfit_sq__d_hard_disks__guo_aligned(scalar q, sasfit_param * p);

/**
 * \ingroup sq__d_hard_disks__guo_aligned
 *
 * \sa sasfit_two_dim_fluid.h, sq_plugins_hard_sphere
 */
sasfit_two_dim_fluid_DLLEXP scalar sasfit_sq__d_hard_disks__guo_aligned_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq__d_hard_disks__guo_aligned
 *
 * \sa sasfit_two_dim_fluid.h, sq_plugins_hard_sphere
 */
sasfit_two_dim_fluid_DLLEXP scalar sasfit_sq__d_hard_disks__guo_aligned_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq__d_hard_disks__guo_aligned ################ */

/* ################ start sq__d_hard_disks__rosenfeld_random ################ */
/**
 * \defgroup sq__d_hard_disks__rosenfeld_random 2D hard disks (Rosenfeld, random)
 * \ingroup sq_plugins_hard_sphere
 *
 * \brief \<some brief description of 2D hard disks (Rosenfeld, random) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius of the discs</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>surface coverage</td>
 *      </tr></table>
 */

/**
 * \ingroup sq__d_hard_disks__rosenfeld_random
 *
 * \sa sasfit_two_dim_fluid.h, sq_plugins_hard_sphere
 */
sasfit_two_dim_fluid_DLLEXP scalar sasfit_sq__d_hard_disks__rosenfeld_random(scalar q, sasfit_param * p);

/**
 * \ingroup sq__d_hard_disks__rosenfeld_random
 *
 * \sa sasfit_two_dim_fluid.h, sq_plugins_hard_sphere
 */
sasfit_two_dim_fluid_DLLEXP scalar sasfit_sq__d_hard_disks__rosenfeld_random_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq__d_hard_disks__rosenfeld_random
 *
 * \sa sasfit_two_dim_fluid.h, sq_plugins_hard_sphere
 */
sasfit_two_dim_fluid_DLLEXP scalar sasfit_sq__d_hard_disks__rosenfeld_random_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq__d_hard_disks__rosenfeld_random ################ */

/* ################ start sq__d_hard_disks__guo_random ################ */
/**
 * \defgroup sq__d_hard_disks__guo_random 2D hard disks (Guo,random)
 * \ingroup sq_plugins_hard_sphere
 *
 * \brief \<some brief description of 2D hard disks (Guo,random) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius of hard disks</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>surface coverage</td>
 *      </tr></table>
 */

/**
 * \ingroup sq__d_hard_disks__guo_random
 *
 * \sa sasfit_two_dim_fluid.h, sq_plugins_hard_sphere
 */
sasfit_two_dim_fluid_DLLEXP scalar sasfit_sq__d_hard_disks__guo_random(scalar q, sasfit_param * p);

/**
 * \ingroup sq__d_hard_disks__guo_random
 *
 * \sa sasfit_two_dim_fluid.h, sq_plugins_hard_sphere
 */
sasfit_two_dim_fluid_DLLEXP scalar sasfit_sq__d_hard_disks__guo_random_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq__d_hard_disks__guo_random
 *
 * \sa sasfit_two_dim_fluid.h, sq_plugins_hard_sphere
 */
sasfit_two_dim_fluid_DLLEXP scalar sasfit_sq__d_hard_disks__guo_random_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq__d_hard_disks__guo_random ################ */

#endif // this file

