/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_CLUSTERYUKWA_H
#define SASFIT_PLUGIN_CLUSTERYUKWA_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_clusteryukwa.h
 * Public available functions and descriptions of the clusteryukwa plugin.
 */

/**
 * \def sasfit_clusteryukwa_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_clusteryukwa_EXPORTS)
	#ifdef sasfit_clusteryukwa_DLLEXP
	#undef sasfit_clusteryukwa_DLLEXP
	#endif
	#define sasfit_clusteryukwa_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_clusteryukwa_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_clusteryukwa_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq_clusteryukawa ################ */
/** 
 * \defgroup sq_clusteryukawa ClusterYukawa
 * \ingroup sq_plugins_devel
 *
 * \brief \<some brief description of ClusterYukawa function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b K</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b phi</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b molarity</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b n</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b Delta</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sq_clusteryukawa
 *
 * \sa sasfit_clusteryukwa.h, sq_plugins_devel
 */
sasfit_clusteryukwa_DLLEXP scalar sasfit_sq_clusteryukawa(scalar q, sasfit_param * p);

/**
 * \ingroup sq_clusteryukawa
 *
 * \sa sasfit_clusteryukwa.h, sq_plugins_devel
 */
sasfit_clusteryukwa_DLLEXP scalar sasfit_sq_clusteryukawa_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_clusteryukawa
 *
 * \sa sasfit_clusteryukwa.h, sq_plugins_devel
 */
sasfit_clusteryukwa_DLLEXP scalar sasfit_sq_clusteryukawa_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_clusteryukawa ################ */


#endif // this file

