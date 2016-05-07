/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   8th of May 2016
 */

#ifndef SASFIT_PLUGIN_PEAK_H
#define SASFIT_PLUGIN_PEAK_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_peak.h
 * Public available functions and descriptions of the peak plugin.
 */

/**
 * \def sasfit_peak_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_peak_EXPORTS)
	#ifdef sasfit_peak_DLLEXP
	#undef sasfit_peak_DLLEXP
	#endif
	#define sasfit_peak_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_peak_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_peak_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start peak_gex_amplitude ################ */
/** 
 * \defgroup peak_gex_amplitude GEX Amplitude
 * \ingroup peak_peak_amplitudes
 *
 * \brief \<some brief description of GEX Amplitude function\>
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
 *       <td>\b gamma</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b X0</td>
 *       <td>shift parameter</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>first shape parameter</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>second shape parameter</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_gex_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gex_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gex_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_ff_gex_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gex_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_ff_gex_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_gex_amplitude ################ */

/* ################ start ff_gex_area ################ */
/** 
 * \defgroup ff_gex_area GEX Area
 * \ingroup ff_peak_area
 *
 * \brief \<some brief description of GEX Area function\>
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
 *       <td>\b gamma</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b X0</td>
 *       <td>shift parameter</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>first shape parameter</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>second shape parameter</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_gex_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_ff_gex_area(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gex_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_ff_gex_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gex_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_ff_gex_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_gex_area ################ */


#endif // this file

