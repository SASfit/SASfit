/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_SQUARE_WELL_SHOULDER_H
#define SASFIT_PLUGIN_SQUARE_WELL_SHOULDER_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_square_well_shoulder.h
 * Public available functions and descriptions of the square_well_shoulder plugin.
 */

/**
 * \def sasfit_square_well_shoulder_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_square_well_shoulder_EXPORTS)
	#ifdef sasfit_square_well_shoulder_DLLEXP
	#undef sasfit_square_well_shoulder_DLLEXP
	#endif
	#define sasfit_square_well_shoulder_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_square_well_shoulder_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_square_well_shoulder_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq_square_well ################ */
/**
 * \defgroup sq_square_well square well
 * \ingroup sq_plugins_devel
 *
 * \brief \<some brief description of square well function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b RHS</td>
 *       <td>hard sphere repulsion radius</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>relative square well width, lambda>0, total width Delta=2RHS(lambda-1)</td>
 *      </tr><tr>
 *       <td>\b E/kBT</td>
 *       <td>square well depth in units of kBT</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>volume fraction</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_square_well
 *
 * \sa sasfit_square_well_shoulder.h, sq_plugins_devel
 */
sasfit_square_well_shoulder_DLLEXP scalar sasfit_sq_square_well(scalar q, sasfit_param * p);

/**
 * \ingroup sq_square_well
 *
 * \sa sasfit_square_well_shoulder.h, sq_plugins_devel
 */
sasfit_square_well_shoulder_DLLEXP scalar sasfit_sq_square_well_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_square_well
 *
 * \sa sasfit_square_well_shoulder.h, sq_plugins_devel
 */
sasfit_square_well_shoulder_DLLEXP scalar sasfit_sq_square_well_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_square_well ################ */

/* ################ start sq_square_shoulder ################ */
/**
 * \defgroup sq_square_shoulder square shoulder
 * \ingroup sq_plugins_devel
 *
 * \brief \<some brief description of square shoulder function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b RHS</td>
 *       <td>hard sphere repulsion radius</td>
 *      </tr><tr>
 *       <td>\b DUMMY</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>volume fraction</td>
 *      </tr><tr>
 *       <td>\b lambda1</td>
 *       <td>relative square shoulder width, lambda>0, total width Delta=2RHS(lambda-1)</td>
 *      </tr><tr>
 *       <td>\b E1/kBT</td>
 *       <td>square well depth in units of kBT</td>
 *      </tr><tr>
 *       <td>\b lambda2</td>
 *       <td>relative square shoulder width, lambda>0, total width Delta=2RHS(lambda-1)</td>
 *      </tr><tr>
 *       <td>\b E2/kBT</td>
 *       <td>square well depth in units of kBT</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_square_shoulder
 *
 * \sa sasfit_square_well_shoulder.h, sq_plugins_devel
 */
sasfit_square_well_shoulder_DLLEXP scalar sasfit_sq_square_shoulder(scalar q, sasfit_param * p);

/**
 * \ingroup sq_square_shoulder
 *
 * \sa sasfit_square_well_shoulder.h, sq_plugins_devel
 */
sasfit_square_well_shoulder_DLLEXP scalar sasfit_sq_square_shoulder_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_square_shoulder
 *
 * \sa sasfit_square_well_shoulder.h, sq_plugins_devel
 */
sasfit_square_well_shoulder_DLLEXP scalar sasfit_sq_square_shoulder_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_square_shoulder ################ */


#endif // this file

