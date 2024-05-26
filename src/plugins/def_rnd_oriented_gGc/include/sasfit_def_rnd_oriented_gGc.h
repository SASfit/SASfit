/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_def_rnd_oriented_gGc_H
#define SASFIT_PLUGIN_def_rnd_oriented_gGc_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_def_rnd_oriented_gGc.h
 * Public available functions and descriptions of the def_rnd_oriented_gGc plugin.
 */

/**
 * \def sasfit_def_rnd_oriented_gGc_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_def_rnd_oriented_gGc_EXPORTS)
	#ifdef sasfit_def_rnd_oriented_gGc_DLLEXP
	#undef sasfit_def_rnd_oriented_gGc_DLLEXP
	#endif
	#define sasfit_def_rnd_oriented_gGc_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_def_rnd_oriented_gGc_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_def_rnd_oriented_gGc_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_def_rnd_oriented_gGc ################ */
/** 
 * \defgroup ff_def_rnd_oriented_gGc stretched random oriented generalized gaussian coil
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
 * \ingroup ff_def_rnd_oriented_gGc
 *
 * \sa sasfit_def_rnd_oriented_gGc.h, ff_devel
 */
sasfit_def_rnd_oriented_gGc_DLLEXP scalar sasfit_ff_def_rnd_oriented_gGc(scalar q, sasfit_param * p);

/**
 * \ingroup ff_def_rnd_oriented_gGc
 *
 * \sa sasfit_def_rnd_oriented_gGc.h, ff_devel
 */
sasfit_def_rnd_oriented_gGc_DLLEXP scalar sasfit_ff_def_rnd_oriented_gGc_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_def_rnd_oriented_gGc
 *
 * \sa sasfit_def_rnd_oriented_gGc.h, ff_devel
 */
sasfit_def_rnd_oriented_gGc_DLLEXP scalar sasfit_ff_def_rnd_oriented_gGc_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_def_rnd_oriented_gGc ################ */


#endif // this file

