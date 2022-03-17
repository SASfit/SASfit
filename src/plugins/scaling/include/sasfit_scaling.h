/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_SCALING_H
#define SASFIT_PLUGIN_SCALING_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_scaling.h
 * Public available functions and descriptions of the scaling plugin.
 */

/**
 * \def sasfit_scaling_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_scaling_EXPORTS)
	#ifdef sasfit_scaling_DLLEXP
	#undef sasfit_scaling_DLLEXP
	#endif
	#define sasfit_scaling_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_scaling_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_scaling_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq_scaling ################ */
/**
 * \defgroup sq_scaling scaling
 * \ingroup sq_plugins_devel
 *
 * \brief \< l^(-alpha) \>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b l</td>
 *       <td>length</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>dimension, combined with monodisp. app, distribution can be number/vol./int. distr, </td>
 *      </tr></table>
 */

/**
 * \ingroup sq_scaling
 *
 * \sa sasfit_scaling.h, sq_plugins_devel
 */
sasfit_scaling_DLLEXP scalar sasfit_sq_scaling(scalar q, sasfit_param * p);

/**
 * \ingroup sq_scaling
 *
 * \sa sasfit_scaling.h, sq_plugins_devel
 */
sasfit_scaling_DLLEXP scalar sasfit_sq_scaling_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_scaling
 *
 * \sa sasfit_scaling.h, sq_plugins_devel
 */
sasfit_scaling_DLLEXP scalar sasfit_sq_scaling_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_scaling ################ */


#endif // this file

