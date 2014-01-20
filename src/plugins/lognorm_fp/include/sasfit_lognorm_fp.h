/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_LOGNORM_FP_H
#define SASFIT_PLUGIN_LOGNORM_FP_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_lognorm_fp.h
 * Public available functions and descriptions of the lognorm_fp plugin.
 */

/**
 * \def sasfit_lognorm_fp_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_lognorm_fp_EXPORTS)
	#ifdef sasfit_lognorm_fp_DLLEXP
	#undef sasfit_lognorm_fp_DLLEXP
	#endif
	#define sasfit_lognorm_fp_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_lognorm_fp_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_lognorm_fp_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sd_lognorm_fp ################ */
/**
 * \defgroup sd_lognorm_fp LogNorm_fp
 * \ingroup sd_plugins
 *
 * \brief \<some brief description of LogNorm_fp function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b fp</td>
 *       <td>volume fraction</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b length2</td>
 *       <td>second length parameter</td>
 *      </tr><tr>
 *       <td>\b length3</td>
 *       <td>third length parameter</td>
 *      </tr><tr>
 *       <td>\b shape</td>
 *       <td>spahe parameter (1: sphere (dR), 2: cylinder (dR), 3: cylinder (dL), 4: sph. shell (dR), 5: sph. shell (dDR)</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_lognorm_fp
 *
 * \sa sasfit_lognorm_fp.h, sd_plugins
 */
sasfit_lognorm_fp_DLLEXP scalar sasfit_sd_lognorm_fp(scalar q, sasfit_param * p);

/**
 * \ingroup sd_lognorm_fp
 *
 * \sa sasfit_lognorm_fp.h, sd_plugins
 */
sasfit_lognorm_fp_DLLEXP scalar sasfit_sd_lognorm_fp_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_lognorm_fp
 *
 * \sa sasfit_lognorm_fp.h, sd_plugins
 */
sasfit_lognorm_fp_DLLEXP scalar sasfit_sd_lognorm_fp_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_lognorm_fp ################ */


#endif // this file

