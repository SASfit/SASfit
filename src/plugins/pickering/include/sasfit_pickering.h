/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_PICKERING_H
#define SASFIT_PLUGIN_PICKERING_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_pickering.h
 * Public available functions and descriptions of the pickering plugin.
 */

/**
 * \def sasfit_pickering_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_pickering_EXPORTS)
	#ifdef sasfit_pickering_DLLEXP
	#undef sasfit_pickering_DLLEXP
	#endif
	#define sasfit_pickering_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_pickering_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_pickering_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq_s_pp_mc_pickering ################ */
/**
 * \defgroup sq_s_pp_mc_pickering S_pp MC pickering
 * \ingroup sq_plugins_devel
 *
 * \brief \<some brief description of S_pp MC pickering function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_o</td>
 *       <td>radius of core </td>
 *      </tr><tr>
 *       <td>\b R_p</td>
 *       <td>radius of the particles on </td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>penetration depth of pickering particles (-1,1)</td>
 *      </tr><tr>
 *       <td>\b n</td>
 *       <td>number of pickering particles</td>
 *      </tr><tr>
 *       <td>\b exvol</td>
 *       <td>exvol==0 (random); exvol!=0 no overlapp allowed</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_s_pp_mc_pickering
 *
 * \sa sasfit_pickering.h, sq_plugins_devel
 */
sasfit_pickering_DLLEXP scalar sasfit_sq_s_pp_mc_pickering(scalar q, sasfit_param * p);

/**
 * \ingroup sq_s_pp_mc_pickering
 *
 * \sa sasfit_pickering.h, sq_plugins_devel
 */
sasfit_pickering_DLLEXP scalar sasfit_sq_s_pp_mc_pickering_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_s_pp_mc_pickering
 *
 * \sa sasfit_pickering.h, sq_plugins_devel
 */
sasfit_pickering_DLLEXP scalar sasfit_sq_s_pp_mc_pickering_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_s_pp_mc_pickering ################ */

/* ################ start sq_s_pp_pickering ################ */
/**
 * \defgroup sq_s_pp_pickering S_pp pickering
 * \ingroup sq_plugins_devel
 *
 * \brief \<some brief description of S_pp pickering function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_o</td>
 *       <td>radius of core </td>
 *      </tr><tr>
 *       <td>\b R_p</td>
 *       <td>radius of the particles on </td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>penetration depth of pickering particles (-1,1)</td>
 *      </tr><tr>
 *       <td>\b n</td>
 *       <td>number of pickering particles</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_s_pp_pickering
 *
 * \sa sasfit_pickering.h, sq_plugins_devel
 */
sasfit_pickering_DLLEXP scalar sasfit_sq_s_pp_pickering(scalar q, sasfit_param * p);

/**
 * \ingroup sq_s_pp_pickering
 *
 * \sa sasfit_pickering.h, sq_plugins_devel
 */
sasfit_pickering_DLLEXP scalar sasfit_sq_s_pp_pickering_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_s_pp_pickering
 *
 * \sa sasfit_pickering.h, sq_plugins_devel
 */
sasfit_pickering_DLLEXP scalar sasfit_sq_s_pp_pickering_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_s_pp_pickering ################ */


#endif // this file

