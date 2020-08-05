/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_SUPERELLIPSOID_H
#define SASFIT_PLUGIN_SUPERELLIPSOID_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_superellipsoid.h
 * Public available functions and descriptions of the superellipsoid plugin.
 */

/**
 * \def sasfit_superellipsoid_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_superellipsoid_EXPORTS)
	#ifdef sasfit_superellipsoid_DLLEXP
	#undef sasfit_superellipsoid_DLLEXP
	#endif
	#define sasfit_superellipsoid_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_superellipsoid_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_superellipsoid_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start peaks_superegg_random ################ */
/** 
 * \defgroup peaks_superegg_random SuperEgg-random
 * \ingroup ff_devel
 *
 * \brief \<some brief description of SuperEgg-random function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b epsilon</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta_c</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup peaks_superegg_random
 *
 * \sa sasfit_superellipsoid.h, ff_devel
 */
sasfit_superellipsoid_DLLEXP scalar sasfit_peaks_superegg_random(scalar q, sasfit_param * p);

/**
 * \ingroup peaks_superegg_random
 *
 * \sa sasfit_superellipsoid.h, ff_devel
 */
sasfit_superellipsoid_DLLEXP scalar sasfit_peaks_superegg_random_f(scalar q, sasfit_param * p);

/**
 * \ingroup peaks_superegg_random
 *
 * \sa sasfit_superellipsoid.h, ff_devel
 */
sasfit_superellipsoid_DLLEXP scalar sasfit_peaks_superegg_random_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peaks_superegg_random ################ */

/* ################ start ff_superegg_opo ################ */
/** 
 * \defgroup ff_superegg_opo SuperEgg-opo
 * \ingroup ff_devel
 *
 * \brief \<some brief description of SuperEgg-opo function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_superegg_opo
 *
 * \sa sasfit_superellipsoid.h, ff_devel
 */
sasfit_superellipsoid_DLLEXP scalar sasfit_ff_superegg_opo(scalar q, sasfit_param * p);

/**
 * \ingroup ff_superegg_opo
 *
 * \sa sasfit_superellipsoid.h, ff_devel
 */
sasfit_superellipsoid_DLLEXP scalar sasfit_ff_superegg_opo_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_superegg_opo
 *
 * \sa sasfit_superellipsoid.h, ff_devel
 */
sasfit_superellipsoid_DLLEXP scalar sasfit_ff_superegg_opo_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_superegg_opo ################ */

/* ################ start ff_superellipsoid_random ################ */
/** 
 * \defgroup ff_superellipsoid_random SuperEllipsoid-random
 * \ingroup ff_devel
 *
 * \brief \<some brief description of SuperEllipsoid-random function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_superellipsoid_random
 *
 * \sa sasfit_superellipsoid.h, ff_devel
 */
sasfit_superellipsoid_DLLEXP scalar sasfit_ff_superellipsoid_random(scalar q, sasfit_param * p);

/**
 * \ingroup ff_superellipsoid_random
 *
 * \sa sasfit_superellipsoid.h, ff_devel
 */
sasfit_superellipsoid_DLLEXP scalar sasfit_ff_superellipsoid_random_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_superellipsoid_random
 *
 * \sa sasfit_superellipsoid.h, ff_devel
 */
sasfit_superellipsoid_DLLEXP scalar sasfit_ff_superellipsoid_random_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_superellipsoid_random ################ */

/* ################ start ff_superellipsoid_opo ################ */
/** 
 * \defgroup ff_superellipsoid_opo SuperEllipsoid-opo
 * \ingroup ff_devel
 *
 * \brief \<some brief description of SuperEllipsoid-opo function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>max half length in x direction</td>
 *      </tr><tr>
 *       <td>\b b</td>
 *       <td>max half length in y direction</td>
 *      </tr><tr>
 *       <td>\b c</td>
 *       <td>max half length in z direction</td>
 *      </tr><tr>
 *       <td>\b q</td>
 *       <td>shape parameter (xy)</td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td>shape parameter (z)</td>
 *      </tr><tr>
 *       <td>\b eta_c</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_superellipsoid_opo
 *
 * \sa sasfit_superellipsoid.h, ff_devel
 */
sasfit_superellipsoid_DLLEXP scalar sasfit_ff_superellipsoid_opo(scalar q, sasfit_param * p);

/**
 * \ingroup ff_superellipsoid_opo
 *
 * \sa sasfit_superellipsoid.h, ff_devel
 */
sasfit_superellipsoid_DLLEXP scalar sasfit_ff_superellipsoid_opo_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_superellipsoid_opo
 *
 * \sa sasfit_superellipsoid.h, ff_devel
 */
sasfit_superellipsoid_DLLEXP scalar sasfit_ff_superellipsoid_opo_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_superellipsoid_opo ################ */


#endif // this file

