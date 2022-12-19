/* 
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
	
	Author(s) of this file: Martin Schmiele (martin.schmiele@nbi.ku.dk)
*/

#ifndef SASFIT_PLUGIN_LINEAR_PEARLS_H
#define SASFIT_PLUGIN_LINEAR_PEARLS_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_linear_pearls.h
 * Public available functions and descriptions of the linear_pearls plugin.
 */

/**
 * \def sasfit_linear_pearls_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_linear_pearls_EXPORTS)
	#ifdef sasfit_linear_pearls_DLLEXP
	#undef sasfit_linear_pearls_DLLEXP
	#endif
	#define sasfit_linear_pearls_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_linear_pearls_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_linear_pearls_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_linear_pearls ################ */
/**
 * \defgroup ff_linear_pearls linear string of spheres
 * \ingroup ff_plugins_spheres_shells
 *
 * \brief \<some brief description of linear_pearls function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_pearl</td>
 *       <td>Radius of the pearls</td>
 *      </tr><tr>
 *       <td>\b edge_sep</td>
 *       <td>Length of the string segment - surface to surface</td>
 *      </tr><tr>
 *       <td>\b num_pearls</td>
 *       <td>Number of the pearls</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering length density of pearls</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */


/**
 * \ingroup ff_linear_pearls
 *
 * \sa sasfit_linear_pearls.h, ff_plugins_spheres_shells
 */
sasfit_linear_pearls_DLLEXP scalar sasfit_ff_linear_pearls(scalar q, sasfit_param * p);

/**
* \ingroup ff_linear_pearls
*
* \sa sasfit_linear_pearls.h, ff_plugins_spheres_shells
*/
// since it does not exist, it does return a dummy value (this function is required in build step)
sasfit_linear_pearls_DLLEXP scalar sasfit_ff_linear_pearls_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_linear_pearls
 *
 * \sa sasfit_linear_pearls.h, ff_plugins_spheres_shells
 */
sasfit_linear_pearls_DLLEXP scalar sasfit_ff_linear_pearls_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_linear_pearls ################ */


#endif // this file

