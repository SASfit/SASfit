/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_PARALLEL_EPIPED_H
#define SASFIT_PLUGIN_PARALLEL_EPIPED_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_parallel_epiped.h
 * Public available functions and descriptions of the parallel_epiped plugin.
 */

/**
 * \def sasfit_parallel_epiped_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_parallel_epiped_EXPORTS)
	#ifdef sasfit_parallel_epiped_DLLEXP
	#undef sasfit_parallel_epiped_DLLEXP
	#endif
	#define sasfit_parallel_epiped_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_parallel_epiped_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_parallel_epiped_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_parallelepiped_abc ################ */
/** 
 * \defgroup ff_parallelepiped_abc Parallelepiped_abc
 * \ingroup ff_plugins_anisotropic
 *
 * \brief \<some brief description of Parallelepiped_abc function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>first side length</td>
 *      </tr><tr>
 *       <td>\b b</td>
 *       <td>second side length</td>
 *      </tr><tr>
 *       <td>\b c</td>
 *       <td>third side length</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_parallelepiped_abc
 *
 * \sa sasfit_parallel_epiped.h, ff_plugins_anisotropic
 */
sasfit_parallel_epiped_DLLEXP scalar sasfit_ff_parallelepiped_abc(scalar q, sasfit_param * p);

/**
 * \ingroup ff_parallelepiped_abc
 *
 * \sa sasfit_parallel_epiped.h, ff_plugins_anisotropic
 */
sasfit_parallel_epiped_DLLEXP scalar sasfit_ff_parallelepiped_abc_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_parallelepiped_abc
 *
 * \sa sasfit_parallel_epiped.h, ff_plugins_anisotropic
 */
sasfit_parallel_epiped_DLLEXP scalar sasfit_ff_parallelepiped_abc_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_parallelepiped_abc ################ */


#endif // this file

