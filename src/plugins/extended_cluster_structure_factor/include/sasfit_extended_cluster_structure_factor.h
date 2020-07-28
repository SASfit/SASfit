/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_EXTENDED_CLUSTER_STRUCTURE_FACTOR_H
#define SASFIT_PLUGIN_EXTENDED_CLUSTER_STRUCTURE_FACTOR_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_extended_cluster_structure_factor.h
 * Public available functions and descriptions of the extended_cluster_structure_factor plugin.
 */

/**
 * \def sasfit_extended_cluster_structure_factor_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_extended_cluster_structure_factor_EXPORTS)
	#ifdef sasfit_extended_cluster_structure_factor_DLLEXP
	#undef sasfit_extended_cluster_structure_factor_DLLEXP
	#endif
	#define sasfit_extended_cluster_structure_factor_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_extended_cluster_structure_factor_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_extended_cluster_structure_factor_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq_extended_cluster_sq ################ */
/**
 * \defgroup sq_extended_cluster_sq extended cluster Sq
 * \ingroup sq_plugins_fractal_objects
 *
 * \brief \<some brief description of extended cluster Sq function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius of particles in cluster</td>
 *      </tr><tr>
 *       <td>\b phi</td>
 *       <td>local volume fraction inside cluster</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>size (correlation length) of cluster</td>
 *      </tr><tr>
 *       <td>\b epsilon</td>
 *       <td>shape parameter of cluster (<1:disc; >1:rod)</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_extended_cluster_sq
 *
 * \sa sasfit_extended_cluster_structure_factor.h, sq_plugins_fractal_objects
 */
sasfit_extended_cluster_structure_factor_DLLEXP scalar sasfit_sq_extended_cluster_sq(scalar q, sasfit_param * p);

/**
 * \ingroup sq_extended_cluster_sq
 *
 * \sa sasfit_extended_cluster_structure_factor.h, sq_plugins_fractal_objects
 */
sasfit_extended_cluster_structure_factor_DLLEXP scalar sasfit_sq_extended_cluster_sq_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_extended_cluster_sq
 *
 * \sa sasfit_extended_cluster_structure_factor.h, sq_plugins_fractal_objects
 */
sasfit_extended_cluster_structure_factor_DLLEXP scalar sasfit_sq_extended_cluster_sq_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_extended_cluster_sq ################ */


#endif // this file

