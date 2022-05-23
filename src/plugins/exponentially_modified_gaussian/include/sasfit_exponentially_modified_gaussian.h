/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_EXPONENTIALLY_MODIFIED_GAUSSIAN_H
#define SASFIT_PLUGIN_EXPONENTIALLY_MODIFIED_GAUSSIAN_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_exponentially_modified_gaussian.h
 * Public available functions and descriptions of the exponentially_modified_gaussian plugin.
 */

/**
 * \def sasfit_exponentially_modified_gaussian_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_exponentially_modified_gaussian_EXPORTS)
	#ifdef sasfit_exponentially_modified_gaussian_DLLEXP
	#undef sasfit_exponentially_modified_gaussian_DLLEXP
	#endif
	#define sasfit_exponentially_modified_gaussian_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_exponentially_modified_gaussian_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_exponentially_modified_gaussian_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sd_exponentially_modified_gaussian ################ */
/**
 * \defgroup sd_exponentially_modified_gaussian exponentially modified Gaussian
 * \ingroup sd_plugins_normal
 *
 * \brief \<some brief description of exponentially modified Gaussian function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>number density of particles</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>sigma^2: variance of Gaussian component</td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td>mean of Gaussian component</td>
 *      </tr><tr>
 *       <td>\b tau</td>
 *       <td>exponent relaxation time</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_exponentially_modified_gaussian
 *
 * \sa sasfit_exponentially_modified_gaussian.h, sd_plugins_normal
 */
sasfit_exponentially_modified_gaussian_DLLEXP scalar sasfit_sd_exponentially_modified_gaussian(scalar q, sasfit_param * p);

/**
 * \ingroup sd_exponentially_modified_gaussian
 *
 * \sa sasfit_exponentially_modified_gaussian.h, sd_plugins_normal
 */
sasfit_exponentially_modified_gaussian_DLLEXP scalar sasfit_sd_exponentially_modified_gaussian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_exponentially_modified_gaussian
 *
 * \sa sasfit_exponentially_modified_gaussian.h, sd_plugins_normal
 */
sasfit_exponentially_modified_gaussian_DLLEXP scalar sasfit_sd_exponentially_modified_gaussian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_exponentially_modified_gaussian ################ */


#endif // this file

