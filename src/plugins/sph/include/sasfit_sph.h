/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_SPH_H
#define SASFIT_PLUGIN_SPH_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_sph.h
 * Public available functions and descriptions of the sph plugin.
 */

/**
 * \def sasfit_sph_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_sph_EXPORTS)
	#ifdef sasfit_sph_DLLEXP
	#undef sasfit_sph_DLLEXP
	#endif
	#define sasfit_sph_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_sph_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_sph_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_sphere ################ */
/** 
 * \defgroup ff_sphere sphere
 * \ingroup ff_devel
 *
 * \brief \<some brief description of sphere function\>
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
 *       <td>\b eta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_sphere
 *
 * \sa sasfit_sph.h, ff_devel
 */
sasfit_sph_DLLEXP scalar sasfit_ff_sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere
 *
 * \sa sasfit_sph.h, ff_devel
 */
sasfit_sph_DLLEXP scalar sasfit_ff_sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere
 *
 * \sa sasfit_sph.h, ff_devel
 */
sasfit_sph_DLLEXP scalar sasfit_ff_sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_sphere ################ */


#endif // this file

