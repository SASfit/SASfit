/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_HOLLOW_SPH_H
#define SASFIT_PLUGIN_HOLLOW_SPH_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_hollow_sph.h
 * Public available functions and descriptions of the hollow_sph plugin.
 */

/**
 * \def sasfit_hollow_sph_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_hollow_sph_EXPORTS)
	#ifdef sasfit_hollow_sph_DLLEXP
	#undef sasfit_hollow_sph_DLLEXP
	#endif
	#define sasfit_hollow_sph_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_hollow_sph_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_hollow_sph_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_hollowsphere ################ */
/** 
 * \defgroup ff_hollowsphere HollowSphere
 * \ingroup ff_devel
 *
 * \brief \<some brief description of HollowSphere function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b DR</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b Contrast</td>
 *       <td>scattering length density of shell</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_hollowsphere
 *
 * \sa sasfit_hollow_sph.h, ff_devel
 */
sasfit_hollow_sph_DLLEXP scalar sasfit_ff_hollowsphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_hollowsphere
 *
 * \sa sasfit_hollow_sph.h, ff_devel
 */
sasfit_hollow_sph_DLLEXP scalar sasfit_ff_hollowsphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_hollowsphere
 *
 * \sa sasfit_hollow_sph.h, ff_devel
 */
sasfit_hollow_sph_DLLEXP scalar sasfit_ff_hollowsphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_hollowsphere ################ */


#endif // this file

