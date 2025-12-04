/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_TILTSANS1_H
#define SASFIT_PLUGIN_TILTSANS1_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_tiltsans1.h
 * Public available functions and descriptions of the tiltsans1 plugin.
 */

/**
 * \def sasfit_tiltsans1_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_tiltsans1_EXPORTS)
	#ifdef sasfit_tiltsans1_DLLEXP
	#undef sasfit_tiltsans1_DLLEXP
	#endif
	#define sasfit_tiltsans1_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_tiltsans1_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_tiltsans1_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_tiltsans1 ################ */
/**
 * \defgroup ff_tiltsans1 tiltSANS1
 * \ingroup ff_sans
 *
 * \brief \<some brief description of tiltSANS1 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b tilt0</td>
 *       <td>selector tilting angle offset</td>
 *      </tr><tr>
 *       <td>\b Delta_tilt</td>
 *       <td>tilt angle difference</td>
 *      </tr><tr>
 *       <td>\b d001</td>
 *       <td>lattice constant for 1st peak, d_Agbeh=5.8378 nm</td>
 *      </tr><tr>
 *       <td>\b TOForAgBeh</td>
 *       <td><1: AgBeh, >=1 TOF</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_tiltsans1
 *
 * \sa sasfit_tiltsans1.h, ff_sans
 */
sasfit_tiltsans1_DLLEXP scalar sasfit_ff_tiltsans1(scalar q, sasfit_param * p);

/**
 * \ingroup ff_tiltsans1
 *
 * \sa sasfit_tiltsans1.h, ff_sans
 */
sasfit_tiltsans1_DLLEXP scalar sasfit_ff_tiltsans1_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_tiltsans1
 *
 * \sa sasfit_tiltsans1.h, ff_sans
 */
sasfit_tiltsans1_DLLEXP scalar sasfit_ff_tiltsans1_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_tiltsans1 ################ */


#endif // this file

