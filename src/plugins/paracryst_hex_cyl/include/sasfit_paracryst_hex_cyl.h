/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_PARACRYST_HEX_CYL_H
#define SASFIT_PLUGIN_PARACRYST_HEX_CYL_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_paracryst_hex_cyl.h
 * Public available functions and descriptions of the paracryst_hex_cyl plugin.
 */

/**
 * \def sasfit_paracryst_hex_cyl_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_paracryst_hex_cyl_EXPORTS)
	#ifdef sasfit_paracryst_hex_cyl_DLLEXP
	#undef sasfit_paracryst_hex_cyl_DLLEXP
	#endif
	#define sasfit_paracryst_hex_cyl_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_paracryst_hex_cyl_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_paracryst_hex_cyl_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_paracryst_hex_cyl ################ */
/**
 * \defgroup ff_paracryst_hex_cyl paracryst_hex_cyl
 * \ingroup ff_plugins_cylindrical_obj
 *
 * \brief \<some brief description of paracryst_hex_cyl function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>avg. cylinder radius</td>
 *      </tr><tr>
 *       <td>\b dR_ratio</td>
 *       <td>Gaussian width parameter </td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b da_ratio</td>
 *       <td>paracrystalline distortion parameter</td>
 *      </tr><tr>
 *       <td>\b version(SQ)</td>
 *       <td>parameter how to handle forward scattering of S(q)</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>Length of cylinder</td>
 *      </tr><tr>
 *       <td>\b sigma_L</td>
 *       <td>LogNormal width parameter for cylinder length</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_paracryst_hex_cyl
 *
 * \sa sasfit_paracryst_hex_cyl.h, ff_plugins_cylindrical_obj
 */
sasfit_paracryst_hex_cyl_DLLEXP scalar sasfit_ff_paracryst_hex_cyl(scalar q, sasfit_param * p);

/**
 * \ingroup ff_paracryst_hex_cyl
 *
 * \sa sasfit_paracryst_hex_cyl.h, ff_plugins_cylindrical_obj
 */
sasfit_paracryst_hex_cyl_DLLEXP scalar sasfit_ff_paracryst_hex_cyl_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_paracryst_hex_cyl
 *
 * \sa sasfit_paracryst_hex_cyl.h, ff_plugins_cylindrical_obj
 */
sasfit_paracryst_hex_cyl_DLLEXP scalar sasfit_ff_paracryst_hex_cyl_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_paracryst_hex_cyl ################ */


#endif // this file

