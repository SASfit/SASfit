/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_SPHEROID_SZ_H
#define SASFIT_PLUGIN_SPHEROID_SZ_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_spheroid_sz.h
 * Public available functions and descriptions of the spheroid_sz plugin.
 */

/**
 * \def sasfit_spheroid_sz_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_spheroid_sz_EXPORTS)
	#ifdef sasfit_spheroid_sz_DLLEXP
	#undef sasfit_spheroid_sz_DLLEXP
	#endif
	#define sasfit_spheroid_sz_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_spheroid_sz_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_spheroid_sz_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_spheroid_w__sz_distr ################ */
/**
 * \defgroup ff_spheroid_w__sz_distr spheroid w. g-size dist.
 * \ingroup ff_spheroid_obj
 *
 * \brief \<some brief description of spheroid w. g-size distr. function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b nu</td>
 *       <td>stretching factor, R_polar=nu*R_equatorial</td>
 *      </tr><tr>
 *       <td>\b R_equatorial</td>
 *       <td>most probable equatorial semi axis</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b k</td>
 *       <td>width parameter of gamma distribution (k>1)\nmode = (k-1) theta, variance = k theta^2</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of spheroid</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_spheroid_w__sz_distr
 *
 * \sa sasfit_spheroid_sz.h, ff_spheroid_obj
 */
sasfit_spheroid_sz_DLLEXP scalar sasfit_ff_spheroid_w__sz_distr(scalar q, sasfit_param * p);

/**
 * \ingroup ff_spheroid_w__sz_distr
 *
 * \sa sasfit_spheroid_sz.h, ff_spheroid_obj
 */
sasfit_spheroid_sz_DLLEXP scalar sasfit_ff_spheroid_w__sz_distr_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_spheroid_w__sz_distr
 *
 * \sa sasfit_spheroid_sz.h, ff_spheroid_obj
 */
sasfit_spheroid_sz_DLLEXP scalar sasfit_ff_spheroid_w__sz_distr_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_spheroid_w__sz_distr ################ */


#endif // this file

