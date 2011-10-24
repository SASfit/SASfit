/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_SPHERE_WITH_THREE_SHELLS_H
#define SASFIT_PLUGIN_SPHERE_WITH_THREE_SHELLS_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_sphere_with_3_shells.h
 * Public available functions and descriptions of the sphere_with_3_shells plugin.
 */

/**
 * \def sasfit_sphere_with_3_shells_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_sphere_with_3_shells_EXPORTS)
	#ifdef sasfit_sphere_with_3_shells_DLLEXP
	#undef sasfit_sphere_with_3_shells_DLLEXP
	#endif
	#define sasfit_sphere_with_3_shells_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_sphere_with_3_shells_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_sphere_with_3_shells_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_sphere_with_3_shells ################ */
/**
 * \defgroup ff_sphere_with_3_shells sphere with 3 shells
 * \ingroup ff_plugins
 *
 * \brief \<some brief description of sphere with 3 shells function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_core</td>
 *       <td>radius of core</td>
 *      </tr><tr>
 *       <td>\b t_sh1</td>
 *       <td>thickness of shell 1</td>
 *      </tr><tr>
 *       <td>\b t_sh2</td>
 *       <td>thickness of shell 2</td>
 *      </tr><tr>
 *       <td>\b t_sh3</td>
 *       <td>thickness of shell 3</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh1</td>
 *       <td>cattering length density of shell 1</td>
 *      </tr><tr>
 *       <td>\b eta_sh2</td>
 *       <td>cattering length density of shell 2</td>
 *      </tr><tr>
 *       <td>\b eta_sh3</td>
 *       <td>cattering length density of shell 3</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_sphere_with_3_shells
 *
 * \sa sasfit_sphere_with_3_shells.h, ff_plugins
 */
sasfit_sphere_with_3_shells_DLLEXP scalar sasfit_ff_sphere_with_3_shells(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere_with_3_shells
 *
 * \sa sasfit_sphere_with_3_shells.h, ff_plugins
 */
sasfit_sphere_with_3_shells_DLLEXP scalar sasfit_ff_sphere_with_3_shells_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere_with_3_shells
 *
 * \sa sasfit_sphere_with_3_shells.h, ff_plugins
 */
sasfit_sphere_with_3_shells_DLLEXP scalar sasfit_ff_sphere_with_3_shells_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_sphere_with_3_shells ################ */


#endif // this file

