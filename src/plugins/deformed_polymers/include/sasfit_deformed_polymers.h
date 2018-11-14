/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_DEFORMED_POLYMERS_H
#define SASFIT_PLUGIN_DEFORMED_POLYMERS_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_deformed_polymers.h
 * Public available functions and descriptions of the deformed_polymers plugin.
 */

/**
 * \def sasfit_deformed_polymers_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_deformed_polymers_EXPORTS)
	#ifdef sasfit_deformed_polymers_DLLEXP
	#undef sasfit_deformed_polymers_DLLEXP
	#endif
	#define sasfit_deformed_polymers_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_deformed_polymers_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_deformed_polymers_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_deformed_ring_polymer__bmb_ ################ */
/** 
 * \defgroup ff_deformed_ring_polymer__bmb_ deformed ring polymer (BMB)
 * \ingroup ff_plugins_deformed_textured
 *
 * \brief \<some brief description of deformed ring polymer (BMB) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg_ll</td>
 *       <td>radius of gyration parallel to deformation direction</td>
 *      </tr><tr>
 *       <td>\b Rg_perp</td>
 *       <td>radius of gyration perpendicular to deformation direction</td>
 *      </tr><tr>
 *       <td>\b nu</td>
 *       <td>Flory exponent</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr><tr>
 *       <td>\b theta_0</td>
 *       <td>stretching direction</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between q and horizontal direction</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_deformed_ring_polymer__bmb_
 *
 * \sa sasfit_deformed_polymers.h, ff_plugins_deformed_textured
 */
sasfit_deformed_polymers_DLLEXP scalar sasfit_ff_deformed_ring_polymer__bmb_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_deformed_ring_polymer__bmb_
 *
 * \sa sasfit_deformed_polymers.h, ff_plugins_deformed_textured
 */
sasfit_deformed_polymers_DLLEXP scalar sasfit_ff_deformed_ring_polymer__bmb__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_deformed_ring_polymer__bmb_
 *
 * \sa sasfit_deformed_polymers.h, ff_plugins_deformed_textured
 */
sasfit_deformed_polymers_DLLEXP scalar sasfit_ff_deformed_ring_polymer__bmb__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_deformed_ring_polymer__bmb_ ################ */

/* ################ start ff_deformed_generalized_gaussian_chain ################ */
/** 
 * \defgroup ff_deformed_generalized_gaussian_chain deformed generalized Gaussian chain
 * \ingroup ff_plugins_deformed_textured
 *
 * \brief \<some brief description of deformed generalized Gaussian chain function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg_ll</td>
 *       <td>radius of gyration parallel to deformation direction</td>
 *      </tr><tr>
 *       <td>\b Rg_perp</td>
 *       <td>radius of gyration perpendicular to deformation direction</td>
 *      </tr><tr>
 *       <td>\b nu</td>
 *       <td>Flory exponent</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr><tr>
 *       <td>\b theta_0</td>
 *       <td>stretching direction</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between q and horizontal direction</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_deformed_generalized_gaussian_chain
 *
 * \sa sasfit_deformed_polymers.h, ff_plugins_deformed_textured
 */
sasfit_deformed_polymers_DLLEXP scalar sasfit_ff_deformed_generalized_gaussian_chain(scalar q, sasfit_param * p);

/**
 * \ingroup ff_deformed_generalized_gaussian_chain
 *
 * \sa sasfit_deformed_polymers.h, ff_plugins_deformed_textured
 */
sasfit_deformed_polymers_DLLEXP scalar sasfit_ff_deformed_generalized_gaussian_chain_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_deformed_generalized_gaussian_chain
 *
 * \sa sasfit_deformed_polymers.h, ff_plugins_deformed_textured
 */
sasfit_deformed_polymers_DLLEXP scalar sasfit_ff_deformed_generalized_gaussian_chain_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_deformed_generalized_gaussian_chain ################ */

/* ################ start ff_deformed_ring_polymer__bz_ ################ */
/** 
 * \defgroup ff_deformed_ring_polymer__bz_ deformed ring polymer (BZ)
 * \ingroup ff_plugins_deformed_textured
 *
 * \brief \<some brief description of deformed ring polymer (BZ) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg_ll</td>
 *       <td>radius of gyration parallel to deformation direction</td>
 *      </tr><tr>
 *       <td>\b Rg_perp</td>
 *       <td>radius of gyration perpendicular to deformation direction</td>
 *      </tr><tr>
 *       <td>\b nu</td>
 *       <td>Flory exponent</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr><tr>
 *       <td>\b theta_0</td>
 *       <td>stretching direction</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between q and horizontal direction</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_deformed_ring_polymer__bz_
 *
 * \sa sasfit_deformed_polymers.h, ff_plugins_deformed_textured
 */
sasfit_deformed_polymers_DLLEXP scalar sasfit_ff_deformed_ring_polymer__bz_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_deformed_ring_polymer__bz_
 *
 * \sa sasfit_deformed_polymers.h, ff_plugins_deformed_textured
 */
sasfit_deformed_polymers_DLLEXP scalar sasfit_ff_deformed_ring_polymer__bz__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_deformed_ring_polymer__bz_
 *
 * \sa sasfit_deformed_polymers.h, ff_plugins_deformed_textured
 */
sasfit_deformed_polymers_DLLEXP scalar sasfit_ff_deformed_ring_polymer__bz__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_deformed_ring_polymer__bz_ ################ */


#endif // this file

