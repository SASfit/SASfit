/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_GUWELOP_H
#define SASFIT_PLUGIN_GUWELOP_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_guwelop.h
 * Public available functions and descriptions of the guwelop plugin.
 */

/**
 * \def sasfit_guwelop_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_guwelop_EXPORTS)
	#ifdef sasfit_guwelop_DLLEXP
	#undef sasfit_guwelop_DLLEXP
	#endif
	#define sasfit_guwelop_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_guwelop_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_guwelop_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sd_p_guwelop ################ */
/**
 * \defgroup sd_p_guwelop p_GUWELOP
 * \ingroup sd_plugins_quantile
 *
 * \brief \<some brief description of p_GUWELOP function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sd_p_guwelop
 *
 * \sa sasfit_guwelop.h, sd_plugins_quantile
 */
sasfit_guwelop_DLLEXP scalar sasfit_sd_p_guwelop(scalar q, sasfit_param * p);

/**
 * \ingroup sd_p_guwelop
 *
 * \sa sasfit_guwelop.h, sd_plugins_quantile
 */
sasfit_guwelop_DLLEXP scalar sasfit_sd_p_guwelop_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_p_guwelop
 *
 * \sa sasfit_guwelop.h, sd_plugins_quantile
 */
sasfit_guwelop_DLLEXP scalar sasfit_sd_p_guwelop_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_p_guwelop ################ */

/* ################ start ff_guwelop_sphere ################ */
/**
 * \defgroup ff_guwelop_sphere GUWELOP sphere
 * \ingroup ff_plugins_spheres_shells
 *
 * \brief \<some brief description of GUWELOP sphere function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b eta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_guwelop_sphere
 *
 * \sa sasfit_guwelop.h, ff_plugins_spheres_shells
 */
sasfit_guwelop_DLLEXP scalar sasfit_ff_guwelop_sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_guwelop_sphere
 *
 * \sa sasfit_guwelop.h, ff_plugins_spheres_shells
 */
sasfit_guwelop_DLLEXP scalar sasfit_ff_guwelop_sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_guwelop_sphere
 *
 * \sa sasfit_guwelop.h, ff_plugins_spheres_shells
 */
sasfit_guwelop_DLLEXP scalar sasfit_ff_guwelop_sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_guwelop_sphere ################ */

/* ################ start ff_guwelop_profile ################ */
/**
 * \defgroup ff_guwelop_profile GUWELOP profile
 * \ingroup ff_plugins_spheres_shells
 *
 * \brief \<some brief description of GUWELOP profile function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b eta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_guwelop_profile
 *
 * \sa sasfit_guwelop.h, ff_plugins_spheres_shells
 */
sasfit_guwelop_DLLEXP scalar sasfit_ff_guwelop_profile(scalar q, sasfit_param * p);

/**
 * \ingroup ff_guwelop_profile
 *
 * \sa sasfit_guwelop.h, ff_plugins_spheres_shells
 */
sasfit_guwelop_DLLEXP scalar sasfit_ff_guwelop_profile_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_guwelop_profile
 *
 * \sa sasfit_guwelop.h, ff_plugins_spheres_shells
 */
sasfit_guwelop_DLLEXP scalar sasfit_ff_guwelop_profile_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_guwelop_profile ################ */


#endif // this file

