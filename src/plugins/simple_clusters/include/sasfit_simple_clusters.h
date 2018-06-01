/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_SIMPLE_CLUSTERS_H
#define SASFIT_PLUGIN_SIMPLE_CLUSTERS_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_simple_clusters.h
 * Public available functions and descriptions of the simple_clusters plugin.
 */

/**
 * \def sasfit_simple_clusters_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_simple_clusters_EXPORTS)
	#ifdef sasfit_simple_clusters_DLLEXP
	#undef sasfit_simple_clusters_DLLEXP
	#endif
	#define sasfit_simple_clusters_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_simple_clusters_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_simple_clusters_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq_cluster__tetrahedron_ ################ */
/**
 * \defgroup sq_cluster__tetrahedron_ Cluster (Tetrahedron)
 * \ingroup sq_plugins_devel
 *
 * \brief \<some brief description of Cluster (Tetrahedron) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b L_2</td>
 *       <td>half distance between nearest neigboured particles</td>
 *      </tr><tr>
 *       <td>\b n</td>
 *       <td>number of particles in cluster  (5>=n>=1, n:integer)</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_cluster__tetrahedron_
 *
 * \sa sasfit_simple_clusters.h, sq_plugins_devel
 */
sasfit_simple_clusters_DLLEXP scalar sasfit_sq_cluster__tetrahedron_(scalar q, sasfit_param * p);

/**
 * \ingroup sq_cluster__tetrahedron_
 *
 * \sa sasfit_simple_clusters.h, sq_plugins_devel
 */
sasfit_simple_clusters_DLLEXP scalar sasfit_sq_cluster__tetrahedron__f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_cluster__tetrahedron_
 *
 * \sa sasfit_simple_clusters.h, sq_plugins_devel
 */
sasfit_simple_clusters_DLLEXP scalar sasfit_sq_cluster__tetrahedron__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_cluster__tetrahedron_ ################ */

/* ################ start sq_cluster__linear_chain_ ################ */
/**
 * \defgroup sq_cluster__linear_chain_ Cluster (linear chain)
 * \ingroup sq_plugins_devel
 *
 * \brief \<some brief description of Cluster (linear chain) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b L_2</td>
 *       <td>half distance between nearest neigboured particles</td>
 *      </tr><tr>
 *       <td>\b n</td>
 *       <td>number of particles in cluster (n>=1, n:integer)</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_cluster__linear_chain_
 *
 * \sa sasfit_simple_clusters.h, sq_plugins_devel
 */
sasfit_simple_clusters_DLLEXP scalar sasfit_sq_cluster__linear_chain_(scalar q, sasfit_param * p);

/**
 * \ingroup sq_cluster__linear_chain_
 *
 * \sa sasfit_simple_clusters.h, sq_plugins_devel
 */
sasfit_simple_clusters_DLLEXP scalar sasfit_sq_cluster__linear_chain__f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_cluster__linear_chain_
 *
 * \sa sasfit_simple_clusters.h, sq_plugins_devel
 */
sasfit_simple_clusters_DLLEXP scalar sasfit_sq_cluster__linear_chain__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_cluster__linear_chain_ ################ */


#endif // this file

