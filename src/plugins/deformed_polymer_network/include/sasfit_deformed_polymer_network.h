/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_DEFORMED_POLYMER_NETWORK_H
#define SASFIT_PLUGIN_DEFORMED_POLYMER_NETWORK_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_deformed_polymer_network.h
 * Public available functions and descriptions of the deformed_polymer_network plugin.
 */

/**
 * \def sasfit_deformed_polymer_network_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_deformed_polymer_network_EXPORTS)
	#ifdef sasfit_deformed_polymer_network_DLLEXP
	#undef sasfit_deformed_polymer_network_DLLEXP
	#endif
	#define sasfit_deformed_polymer_network_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_deformed_polymer_network_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_deformed_polymer_network_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_we__deformed_polym__netw_ ################ */
/** 
 * \defgroup ff_we__deformed_polym__netw_ WE: deformed polym. netw.
 * \ingroup ff_plugins_deformed_textured
 *
 * \brief \<some brief description of WE: deformed polym. netw. function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of undeformed polymer network</td>
 *      </tr><tr>
 *       <td>\b d0</td>
 *       <td>equilibrium tube diameter</td>
 *      </tr><tr>
 *       <td>\b nu</td>
 *       <td>exponent (nu=1/2, nu=1:affine deformation)</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>deformation ratio along deformation axis</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>direction_of_scattering_vector</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>deforming dirfection (deg)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_we__deformed_polym__netw_
 *
 * \sa sasfit_deformed_polymer_network.h, ff_plugins_deformed_textured
 */
sasfit_deformed_polymer_network_DLLEXP scalar sasfit_ff_we__deformed_polym__netw_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_we__deformed_polym__netw_
 *
 * \sa sasfit_deformed_polymer_network.h, ff_plugins_deformed_textured
 */
sasfit_deformed_polymer_network_DLLEXP scalar sasfit_ff_we__deformed_polym__netw__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_we__deformed_polym__netw_
 *
 * \sa sasfit_deformed_polymer_network.h, ff_plugins_deformed_textured
 */
sasfit_deformed_polymer_network_DLLEXP scalar sasfit_ff_we__deformed_polym__netw__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_we__deformed_polym__netw_ ################ */

/* ################ start ff_hsh__deformed_polym__netw_ ################ */
/** 
 * \defgroup ff_hsh__deformed_polym__netw_ HSH: deformed polym. netw.
 * \ingroup ff_plugins_deformed_textured
 *
 * \brief \<some brief description of HSH: deformed polym. netw. function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>deformed polymer network</td>
 *      </tr><tr>
 *       <td>\b d0</td>
 *       <td>equilibrium tube diameter</td>
 *      </tr><tr>
 *       <td>\b nu</td>
 *       <td>exponent (nu=1/2, nu=1:affine deformation)</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>deformation ratio along deformation axis</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>retraction coeefficient (1<gamma ~<2)</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>direction_of_scattering_vector</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>deforming dirfection (deg)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_hsh__deformed_polym__netw_
 *
 * \sa sasfit_deformed_polymer_network.h, ff_plugins_deformed_textured
 */
sasfit_deformed_polymer_network_DLLEXP scalar sasfit_ff_hsh__deformed_polym__netw_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_hsh__deformed_polym__netw_
 *
 * \sa sasfit_deformed_polymer_network.h, ff_plugins_deformed_textured
 */
sasfit_deformed_polymer_network_DLLEXP scalar sasfit_ff_hsh__deformed_polym__netw__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_hsh__deformed_polym__netw_
 *
 * \sa sasfit_deformed_polymer_network.h, ff_plugins_deformed_textured
 */
sasfit_deformed_polymer_network_DLLEXP scalar sasfit_ff_hsh__deformed_polym__netw__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_hsh__deformed_polym__netw_ ################ */


/* ################ start ff_we__deformed_ringpolym__netw_ ################ */
/** 
 * \defgroup ff_we__deformed_ringpolym__netw_ WE: deformed ring polym. netw.
 * \ingroup ff_plugins_deformed_textured
 *
 * \brief \<some brief description of WE: deformed ring polym. netw. function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of undeformed polymer network</td>
 *      </tr><tr>
 *       <td>\b d0</td>
 *       <td>equilibrium tube diameter</td>
 *      </tr><tr>
 *       <td>\b nu</td>
 *       <td>exponent (nu=1/2, nu=1:affine deformation)</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>deformation ratio along deformation axis</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>direction_of_scattering_vector</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>deforming dirfection (deg)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_we__deformed_ringpolym__netw_
 *
 * \sa sasfit_deformed_polymer_network.h, ff_plugins_deformed_textured
 */
sasfit_deformed_polymer_network_DLLEXP scalar sasfit_ff_we__deformed_ringpolym__netw_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_we__deformed_ringpolym__netw_
 *
 * \sa sasfit_deformed_polymer_network.h, ff_plugins_deformed_textured
 */
sasfit_deformed_polymer_network_DLLEXP scalar sasfit_ff_we__deformed_ringpolym__netw__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_we__deformed_ringpolym__netw_
 *
 * \sa sasfit_deformed_polymer_network.h, ff_plugins_deformed_textured
 */
sasfit_deformed_polymer_network_DLLEXP scalar sasfit_ff_we__deformed_ringpolym__netw__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_we__deformed_ringpolym__netw_ ################ */

/* ################ start ff_hsh__deformed_ringpolym__netw_ ################ */
/** 
 * \defgroup ff_hsh__deformed_ringpolym__netw_ HSH: deformed ring polym. netw.
 * \ingroup ff_plugins_deformed_textured
 *
 * \brief \<some brief description of HSH: deformed ring polym. netw. function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>deformed polymer network</td>
 *      </tr><tr>
 *       <td>\b d0</td>
 *       <td>equilibrium tube diameter</td>
 *      </tr><tr>
 *       <td>\b nu</td>
 *       <td>exponent (nu=1/2, nu=1:affine deformation)</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>deformation ratio along deformation axis</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>retraction coeefficient (1<gamma ~<2)</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>direction_of_scattering_vector</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>deforming dirfection (deg)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_hsh__deformed_ringpolym__netw_
 *
 * \sa sasfit_deformed_polymer_network.h, ff_plugins_deformed_textured
 */
sasfit_deformed_polymer_network_DLLEXP scalar sasfit_ff_hsh__deformed_ringpolym__netw_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_hsh__deformed_ringpolym__netw_
 *
 * \sa sasfit_deformed_polymer_network.h, ff_plugins_deformed_textured
 */
sasfit_deformed_polymer_network_DLLEXP scalar sasfit_ff_hsh__deformed_ringpolym__netw__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_hsh__deformed_ringpolym__netw_
 *
 * \sa sasfit_deformed_polymer_network.h, ff_plugins_deformed_textured
 */
sasfit_deformed_polymer_network_DLLEXP scalar sasfit_ff_hsh__deformed_ringpolym__netw__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_hsh__deformed_ringpolym__netw_ ################ */

#endif // this file

