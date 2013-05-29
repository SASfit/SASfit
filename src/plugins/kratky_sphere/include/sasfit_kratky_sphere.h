/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_KRATKY_SPHERE_H
#define SASFIT_PLUGIN_KRATKY_SPHERE_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_kratky_sphere.h
 * Public available functions and descriptions of the kratky_sphere plugin.
 */

/**
 * \def sasfit_kratky_sphere_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_kratky_sphere_EXPORTS)
	#ifdef sasfit_kratky_sphere_DLLEXP
	#undef sasfit_kratky_sphere_DLLEXP
	#endif
	#define sasfit_kratky_sphere_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_kratky_sphere_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_kratky_sphere_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_kratkysphere ################ */
/**
 * \defgroup ff_kratkysphere KratkySphere
 * \ingroup ff_devel
 *
 * \brief \<some brief description of KratkySphere function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius</td>
 *      </tr><tr>
 *       <td>\b +-Delta_q_perp</td>
 *       <td>resolution parameter</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>contrast</td>
 *      </tr><tr>
 *       <td>\b R0</td>
 *       <td>should be similar to R</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>cluster size</td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td>fractal dimension</td>
 *      </tr><tr>
 *       <td>\b scale</td>
 *       <td>scale=0: no structure factor\n scale=1: with structure factor</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_kratkysphere
 *
 * \sa sasfit_kratky_sphere.h, ff_devel
 */
sasfit_kratky_sphere_DLLEXP scalar sasfit_ff_kratkysphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_kratkysphere
 *
 * \sa sasfit_kratky_sphere.h, ff_devel
 */
sasfit_kratky_sphere_DLLEXP scalar sasfit_ff_kratkysphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_kratkysphere
 *
 * \sa sasfit_kratky_sphere.h, ff_devel
 */
sasfit_kratky_sphere_DLLEXP scalar sasfit_ff_kratkysphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_kratkysphere ################ */


#endif // this file

