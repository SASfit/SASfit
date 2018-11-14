/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_DEFORMED_RANDOM_ORIENTED_GENERALIZED_GAUSSIAN_COIL_H
#define SASFIT_PLUGIN_DEFORMED_RANDOM_ORIENTED_GENERALIZED_GAUSSIAN_COIL_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_deformed_random_oriented_generalized_gaussian_coil.h
 * Public available functions and descriptions of the deformed_random_oriented_generalized_gaussian_coil plugin.
 */

/**
 * \def sasfit_deformed_random_oriented_generalized_gaussian_coil_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_deformed_random_oriented_generalized_gaussian_coil_EXPORTS)
	#ifdef sasfit_deformed_random_oriented_generalized_gaussian_coil_DLLEXP
	#undef sasfit_deformed_random_oriented_generalized_gaussian_coil_DLLEXP
	#endif
	#define sasfit_deformed_random_oriented_generalized_gaussian_coil_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_deformed_random_oriented_generalized_gaussian_coil_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_deformed_random_oriented_generalized_gaussian_coil_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_stretched_random_oriented_generalized_gaussian_coil ################ */
/** 
 * \defgroup ff_stretched_random_oriented_generalized_gaussian_coil stretched random oriented generalized gaussian coil
 * \ingroup ff_devel
 *
 * \brief \<some brief description of stretched random oriented generalized gaussian coil function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg_p</td>
 *       <td>Rg in stretching direction</td>
 *      </tr><tr>
 *       <td>\b Rg_e</td>
 *       <td>Rg perpendicular to stretching direction</td>
 *      </tr><tr>
 *       <td>\b nu</td>
 *       <td>Flory exponent</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_stretched_random_oriented_generalized_gaussian_coil
 *
 * \sa sasfit_deformed_random_oriented_generalized_gaussian_coil.h, ff_devel
 */
sasfit_deformed_random_oriented_generalized_gaussian_coil_DLLEXP scalar sasfit_ff_stretched_random_oriented_generalized_gaussian_coil(scalar q, sasfit_param * p);

/**
 * \ingroup ff_stretched_random_oriented_generalized_gaussian_coil
 *
 * \sa sasfit_deformed_random_oriented_generalized_gaussian_coil.h, ff_devel
 */
sasfit_deformed_random_oriented_generalized_gaussian_coil_DLLEXP scalar sasfit_ff_stretched_random_oriented_generalized_gaussian_coil_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_stretched_random_oriented_generalized_gaussian_coil
 *
 * \sa sasfit_deformed_random_oriented_generalized_gaussian_coil.h, ff_devel
 */
sasfit_deformed_random_oriented_generalized_gaussian_coil_DLLEXP scalar sasfit_ff_stretched_random_oriented_generalized_gaussian_coil_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_stretched_random_oriented_generalized_gaussian_coil ################ */


#endif // this file

