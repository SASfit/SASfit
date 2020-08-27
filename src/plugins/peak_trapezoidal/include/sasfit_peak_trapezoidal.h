/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_PEAK_TRAPEZOIDAL_H
#define SASFIT_PLUGIN_PEAK_TRAPEZOIDAL_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_peak_trapezoidal.h
 * Public available functions and descriptions of the peak_trapezoidal plugin.
 */

/**
 * \def sasfit_peak_trapezoidal_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_peak_trapezoidal_EXPORTS)
	#ifdef sasfit_peak_trapezoidal_DLLEXP
	#undef sasfit_peak_trapezoidal_DLLEXP
	#endif
	#define sasfit_peak_trapezoidal_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_peak_trapezoidal_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_peak_trapezoidal_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_trapez__area ################ */
/**
 * \defgroup ff_trapez__area trapez. Area
 * \ingroup ff_peak_area
 *
 * \brief \<some brief description of trapez. Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>start of peak</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>start of plateau</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>end of plateau</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>end of peak</td>
 *      </tr><tr>
 *       <td>\b c0</td>
 *       <td>background </td>
 *      </tr></table>
 */

/**
 * \ingroup ff_trapez__area
 *
 * \sa sasfit_peak_trapezoidal.h, ff_peak_area
 */
sasfit_peak_trapezoidal_DLLEXP scalar sasfit_ff_trapez__area(scalar q, sasfit_param * p);

/**
 * \ingroup ff_trapez__area
 *
 * \sa sasfit_peak_trapezoidal.h, ff_peak_area
 */
sasfit_peak_trapezoidal_DLLEXP scalar sasfit_ff_trapez__area_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_trapez__area
 *
 * \sa sasfit_peak_trapezoidal.h, ff_peak_area
 */
sasfit_peak_trapezoidal_DLLEXP scalar sasfit_ff_trapez__area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_trapez__area ################ */

/* ################ start ff_symm__trapez__area ################ */
/**
 * \defgroup ff_symm__trapez__area symm. trapez. Area
 * \ingroup ff_peak_area
 *
 * \brief \<some brief description of symm. trapez. Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>center of peak</td>
 *      </tr><tr>
 *       <td>\b wp</td>
 *       <td>width of plateau</td>
 *      </tr><tr>
 *       <td>\b we</td>
 *       <td>width of left or right edge</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b c0</td>
 *       <td>background</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_symm__trapez__area
 *
 * \sa sasfit_peak_trapezoidal.h, ff_peak_area
 */
sasfit_peak_trapezoidal_DLLEXP scalar sasfit_ff_symm__trapez__area(scalar q, sasfit_param * p);

/**
 * \ingroup ff_symm__trapez__area
 *
 * \sa sasfit_peak_trapezoidal.h, ff_peak_area
 */
sasfit_peak_trapezoidal_DLLEXP scalar sasfit_ff_symm__trapez__area_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_symm__trapez__area
 *
 * \sa sasfit_peak_trapezoidal.h, ff_peak_area
 */
sasfit_peak_trapezoidal_DLLEXP scalar sasfit_ff_symm__trapez__area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_symm__trapez__area ################ */

/* ################ start ff_general__trapez__area ################ */
/**
 * \defgroup ff_general__trapez__area general. trapez. Area
 * \ingroup ff_peak_area
 *
 * \brief \<some brief description of general. trapez. Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>start of peak</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>start of plateau</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>end of plateau</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>end of peak</td>
 *      </tr><tr>
 *       <td>\b phi</td>
 *       <td>boundary ratio parameter</td>
 *      </tr><tr>
 *       <td>\b n1</td>
 *       <td>shape parameter of left edge</td>
 *      </tr><tr>
 *       <td>\b n3</td>
 *       <td>shape parameter of right edge</td>
 *      </tr><tr>
 *       <td>\b c0</td>
 *       <td>background</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_general__trapez__area
 *
 * \sa sasfit_peak_trapezoidal.h, ff_peak_area
 */
sasfit_peak_trapezoidal_DLLEXP scalar sasfit_ff_general__trapez__area(scalar q, sasfit_param * p);

/**
 * \ingroup ff_general__trapez__area
 *
 * \sa sasfit_peak_trapezoidal.h, ff_peak_area
 */
sasfit_peak_trapezoidal_DLLEXP scalar sasfit_ff_general__trapez__area_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_general__trapez__area
 *
 * \sa sasfit_peak_trapezoidal.h, ff_peak_area
 */
sasfit_peak_trapezoidal_DLLEXP scalar sasfit_ff_general__trapez__area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_general__trapez__area ################ */

/* ################ start ff_symm__general__trapez__area ################ */
/**
 * \defgroup ff_symm__general__trapez__area symm. general. trapez. Area
 * \ingroup ff_peak_area
 *
 * \brief \<some brief description of symm. general. trapez. Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>center of peak</td>
 *      </tr><tr>
 *       <td>\b wp</td>
 *       <td>width of plateau</td>
 *      </tr><tr>
 *       <td>\b we</td>
 *       <td>width of left or right edge</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b phi</td>
 *       <td>boundary ratio parameter</td>
 *      </tr><tr>
 *       <td>\b n1</td>
 *       <td>shape parameter of left and right edge</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b c0</td>
 *       <td>background</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_symm__general__trapez__area
 *
 * \sa sasfit_peak_trapezoidal.h, ff_peak_area
 */
sasfit_peak_trapezoidal_DLLEXP scalar sasfit_ff_symm__general__trapez__area(scalar q, sasfit_param * p);

/**
 * \ingroup ff_symm__general__trapez__area
 *
 * \sa sasfit_peak_trapezoidal.h, ff_peak_area
 */
sasfit_peak_trapezoidal_DLLEXP scalar sasfit_ff_symm__general__trapez__area_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_symm__general__trapez__area
 *
 * \sa sasfit_peak_trapezoidal.h, ff_peak_area
 */
sasfit_peak_trapezoidal_DLLEXP scalar sasfit_ff_symm__general__trapez__area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_symm__general__trapez__area ################ */

/* ################ start ff_trapez__ampl_ ################ */
/**
 * \defgroup ff_trapez__ampl_ trapez. Ampl.
 * \ingroup ff_peak_amplitudes
 *
 * \brief \<some brief description of trapez. Ampl. function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>amplitude</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>start of peak</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>start of plateau</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>end of plateau</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>end of peak</td>
 *      </tr><tr>
 *       <td>\b c0</td>
 *       <td>background</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_trapez__ampl_
 *
 * \sa sasfit_peak_trapezoidal.h, ff_peak_amplitudes
 */
sasfit_peak_trapezoidal_DLLEXP scalar sasfit_ff_trapez__ampl_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_trapez__ampl_
 *
 * \sa sasfit_peak_trapezoidal.h, ff_peak_amplitudes
 */
sasfit_peak_trapezoidal_DLLEXP scalar sasfit_ff_trapez__ampl__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_trapez__ampl_
 *
 * \sa sasfit_peak_trapezoidal.h, ff_peak_amplitudes
 */
sasfit_peak_trapezoidal_DLLEXP scalar sasfit_ff_trapez__ampl__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_trapez__ampl_ ################ */

/* ################ start ff_symm__trapez__ampl_ ################ */
/**
 * \defgroup ff_symm__trapez__ampl_ symm. trapez. Ampl.
 * \ingroup ff_peak_amplitudes
 *
 * \brief \<some brief description of symm. trapez. Ampl. function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>amplitude</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>centere of peak</td>
 *      </tr><tr>
 *       <td>\b wp</td>
 *       <td>width of plateau</td>
 *      </tr><tr>
 *       <td>\b we</td>
 *       <td>width of left or right edge</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b c0</td>
 *       <td>background</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_symm__trapez__ampl_
 *
 * \sa sasfit_peak_trapezoidal.h, ff_peak_amplitudes
 */
sasfit_peak_trapezoidal_DLLEXP scalar sasfit_ff_symm__trapez__ampl_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_symm__trapez__ampl_
 *
 * \sa sasfit_peak_trapezoidal.h, ff_peak_amplitudes
 */
sasfit_peak_trapezoidal_DLLEXP scalar sasfit_ff_symm__trapez__ampl__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_symm__trapez__ampl_
 *
 * \sa sasfit_peak_trapezoidal.h, ff_peak_amplitudes
 */
sasfit_peak_trapezoidal_DLLEXP scalar sasfit_ff_symm__trapez__ampl__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_symm__trapez__ampl_ ################ */

/* ################ start ff_general__trapez__ampl_ ################ */
/**
 * \defgroup ff_general__trapez__ampl_ general. trapez. Ampl.
 * \ingroup ff_peak_amplitudes
 *
 * \brief \<some brief description of general. trapez. Ampl. function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>amplitude</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>start of peak</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>start of plateau</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>end of plateau</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>end of peak</td>
 *      </tr><tr>
 *       <td>\b phi</td>
 *       <td>boundary ratio parameter</td>
 *      </tr><tr>
 *       <td>\b n1</td>
 *       <td>shape parameter of left edge</td>
 *      </tr><tr>
 *       <td>\b n3</td>
 *       <td>shape parameter of right edge</td>
 *      </tr><tr>
 *       <td>\b c0</td>
 *       <td>background</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_general__trapez__ampl_
 *
 * \sa sasfit_peak_trapezoidal.h, ff_peak_amplitudes
 */
sasfit_peak_trapezoidal_DLLEXP scalar sasfit_ff_general__trapez__ampl_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_general__trapez__ampl_
 *
 * \sa sasfit_peak_trapezoidal.h, ff_peak_amplitudes
 */
sasfit_peak_trapezoidal_DLLEXP scalar sasfit_ff_general__trapez__ampl__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_general__trapez__ampl_
 *
 * \sa sasfit_peak_trapezoidal.h, ff_peak_amplitudes
 */
sasfit_peak_trapezoidal_DLLEXP scalar sasfit_ff_general__trapez__ampl__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_general__trapez__ampl_ ################ */

/* ################ start ff_symm__general__trapez__ampl_ ################ */
/**
 * \defgroup ff_symm__general__trapez__ampl_ symm. general. trapez. Ampl.
 * \ingroup ff_peak_amplitudes
 *
 * \brief \<some brief description of symm. general. trapez. Ampl. function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>amplitude</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>center of peak</td>
 *      </tr><tr>
 *       <td>\b wp</td>
 *       <td>width of plateau</td>
 *      </tr><tr>
 *       <td>\b we</td>
 *       <td>width of left or right edge</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b phi</td>
 *       <td>boundary ratio parameter</td>
 *      </tr><tr>
 *       <td>\b n1</td>
 *       <td>shape parameter of left and right edge</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b c0</td>
 *       <td>background</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_symm__general__trapez__ampl_
 *
 * \sa sasfit_peak_trapezoidal.h, ff_peak_amplitudes
 */
sasfit_peak_trapezoidal_DLLEXP scalar sasfit_ff_symm__general__trapez__ampl_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_symm__general__trapez__ampl_
 *
 * \sa sasfit_peak_trapezoidal.h, ff_peak_amplitudes
 */
sasfit_peak_trapezoidal_DLLEXP scalar sasfit_ff_symm__general__trapez__ampl__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_symm__general__trapez__ampl_
 *
 * \sa sasfit_peak_trapezoidal.h, ff_peak_amplitudes
 */
sasfit_peak_trapezoidal_DLLEXP scalar sasfit_ff_symm__general__trapez__ampl__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_symm__general__trapez__ampl_ ################ */


#endif // this file

