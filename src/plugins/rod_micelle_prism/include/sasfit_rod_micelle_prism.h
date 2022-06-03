/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_ROD_MICELLE_PRISM_H
#define SASFIT_PLUGIN_ROD_MICELLE_PRISM_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_rod_micelle_prism.h
 * Public available functions and descriptions of the rod_micelle_prism plugin.
 */

/**
 * \def sasfit_rod_micelle_prism_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_rod_micelle_prism_EXPORTS)
	#ifdef sasfit_rod_micelle_prism_DLLEXP
	#undef sasfit_rod_micelle_prism_DLLEXP
	#endif
	#define sasfit_rod_micelle_prism_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_rod_micelle_prism_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_rod_micelle_prism_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_rodlike_micelle_plusprism ################ */
/**
 * \defgroup ff_rodlike_micelle_plusprism Rodlike_Micelle_PlusPrism
 * \ingroup ff_user2
 *
 * \brief \<some brief description of Rodlike_Micelle_PlusPrism function\>
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
 *       <td>\b epsilon</td>
 *       <td>stretching factor</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>length of cylinder</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>solvent scattering length density</td>
 *      </tr><tr>
 *       <td>\b phi</td>
 *       <td>volume fraction</td>
 *      </tr><tr>
 *       <td>\b d_eff/B3</td>
 *      </tr><tr>
 *       <td>\b how</td>
 *       <td>how 1: Cotton, 2: JS Pedersen, 3: B3SPT 4: B3FMT 5: B3eff</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_rodlike_micelle_plusprism
 *
 * \sa sasfit_rod_micelle_prism.h, ff_user2
 */
sasfit_rod_micelle_prism_DLLEXP scalar sasfit_ff_rodlike_micelle_plusprism(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rodlike_micelle_plusprism
 *
 * \sa sasfit_rod_micelle_prism.h, ff_user2
 */
sasfit_rod_micelle_prism_DLLEXP scalar sasfit_ff_rodlike_micelle_plusprism_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rodlike_micelle_plusprism
 *
 * \sa sasfit_rod_micelle_prism.h, ff_user2
 */
sasfit_rod_micelle_prism_DLLEXP scalar sasfit_ff_rodlike_micelle_plusprism_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rodlike_micelle_plusprism ################ */


#endif // this file

