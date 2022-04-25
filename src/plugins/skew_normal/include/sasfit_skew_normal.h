/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_SKEW_NORMAL_H
#define SASFIT_PLUGIN_SKEW_NORMAL_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_skew_normal.h
 * Public available functions and descriptions of the skew_normal plugin.
 */

/**
 * \def sasfit_skew_normal_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_skew_normal_EXPORTS)
	#ifdef sasfit_skew_normal_DLLEXP
	#undef sasfit_skew_normal_DLLEXP
	#endif
	#define sasfit_skew_normal_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_skew_normal_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_skew_normal_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sd_skew_normal ################ */
/** 
 * \defgroup sd_skew_normal skew normal
 * \ingroup sd_plugins
 *
 * \brief \<some brief description of skew normal function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>number density of particles</td>
 *      </tr><tr>
 *       <td>\b omega</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>shape parameter</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_skew_normal
 *
 * \sa sasfit_skew_normal.h, sd_plugins
 */
sasfit_skew_normal_DLLEXP scalar sasfit_sd_skew_normal(scalar q, sasfit_param * p);

/**
 * \ingroup sd_skew_normal
 *
 * \sa sasfit_skew_normal.h, sd_plugins
 */
sasfit_skew_normal_DLLEXP scalar sasfit_sd_skew_normal_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_skew_normal
 *
 * \sa sasfit_skew_normal.h, sd_plugins
 */
sasfit_skew_normal_DLLEXP scalar sasfit_sd_skew_normal_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_skew_normal ################ */


#endif // this file

