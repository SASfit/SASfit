/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_GENERALIZED_BETA_H
#define SASFIT_PLUGIN_GENERALIZED_BETA_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_generalized_beta.h
 * Public available functions and descriptions of the generalized_beta plugin.
 */

/**
 * \def sasfit_generalized_beta_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_generalized_beta_EXPORTS)
	#ifdef sasfit_generalized_beta_DLLEXP
	#undef sasfit_generalized_beta_DLLEXP
	#endif
	#define sasfit_generalized_beta_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_generalized_beta_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_generalized_beta_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sd_wgb1 ################ */
/**
 * \defgroup sd_wgb1 WGB1
 * \ingroup sd_plugins
 *
 * \brief \< weighted generalized beta distribution of first kind \>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b b</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b k</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b q</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sd_wgb1
 *
 * \sa sasfit_generalized_beta.h, sd_plugins
 */
sasfit_generalized_beta_DLLEXP scalar sasfit_sd_wgb1(scalar q, sasfit_param * p);

/**
 * \ingroup sd_wgb1
 *
 * \sa sasfit_generalized_beta.h, sd_plugins
 */
sasfit_generalized_beta_DLLEXP scalar sasfit_sd_wgb1_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_wgb1
 *
 * \sa sasfit_generalized_beta.h, sd_plugins
 */
sasfit_generalized_beta_DLLEXP scalar sasfit_sd_wgb1_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_wgb1 ################ */

/* ################ start sd_wgb2 ################ */
/**
 * \defgroup sd_wgb2 WGB2
 * \ingroup sd_plugins
 *
 * \brief \< weighted generalized beta distribution of second kind \>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b b</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b k</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b q</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sd_wgb2
 *
 * \sa sasfit_generalized_beta.h, sd_plugins
 */
sasfit_generalized_beta_DLLEXP scalar sasfit_sd_wgb2(scalar q, sasfit_param * p);

/**
 * \ingroup sd_wgb2
 *
 * \sa sasfit_generalized_beta.h, sd_plugins
 */
sasfit_generalized_beta_DLLEXP scalar sasfit_sd_wgb2_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_wgb2
 *
 * \sa sasfit_generalized_beta.h, sd_plugins
 */
sasfit_generalized_beta_DLLEXP scalar sasfit_sd_wgb2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_wgb2 ################ */

/* ################ start sd_gb ################ */
/**
 * \defgroup sd_gb GB
 * \ingroup sd_plugins
 *
 * \brief \< generalized beta distribution \>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a</td>
 *       <td>a>0</td>
 *      </tr><tr>
 *       <td>\b b</td>
 *       <td>b>0</td>
 *      </tr><tr>
 *       <td>\b c</td>
 *       <td>0<c<1</td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td>p>0</td>
 *      </tr><tr>
 *       <td>\b q</td>
 *       <td>q>0</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_gb
 *
 * \sa sasfit_generalized_beta.h, sd_plugins
 */
sasfit_generalized_beta_DLLEXP scalar sasfit_sd_gb(scalar q, sasfit_param * p);

/**
 * \ingroup sd_gb
 *
 * \sa sasfit_generalized_beta.h, sd_plugins
 */
sasfit_generalized_beta_DLLEXP scalar sasfit_sd_gb_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_gb
 *
 * \sa sasfit_generalized_beta.h, sd_plugins
 */
sasfit_generalized_beta_DLLEXP scalar sasfit_sd_gb_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_gb ################ */


#endif // this file

