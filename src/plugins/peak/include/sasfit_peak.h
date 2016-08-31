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


/* ################ start peak_beta_amplitude ################ */
/** 
 * \defgroup peak_beta_amplitude Beta Amplitude
 * \ingroup ff_peak_amplitudes
 *
 * \brief \<some brief description of Beta Amplitude function\>
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
 *       <td>\b x_min</td>
 *       <td>lower bound/td>
 *      </tr><tr>
 *       <td>\b x_max</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>first shape parameter</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>second shape parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_beta_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_beta_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_beta_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_beta_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_beta_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_beta_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_beta_amplitude ################ */

/* ################ start peak_beta_area ################ */
/** 
 * \defgroup peak_beta_area Beta Area
 * \ingroup ff_peak_area
 *
 * \brief \<some brief description of Beta Area function\>
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
 *       <td>\b x_min</td>
 *       <td>lower bound/td>
 *      </tr><tr>
 *       <td>\b x_max</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>first shape parameter</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>second shape parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_beta_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_beta_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_beta_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_beta_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_beta_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_beta_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_beta_area ################ */

/* ################ start peak_chi_squared_amplitude ################ */
/** 
 * \defgroup peak_chi_squared_amplitude Chi-squared Amplitude
 * \ingroup ff_peak_amplitudes
 *
 * \brief \<some brief description of Chi-squared Amplitude function\>
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
 *       <td>\b mode</td>
 *       <td>mode of distribution</td>
 *      </tr><tr>
 *       <td>\b X0</td>
 *       <td>shift parameter</td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b shape</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_chi_squared_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_chi_squared_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_chi_squared_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_chi_squared_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_chi_squared_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_chi_squared_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_chi_squared_amplitude ################ */

/* ################ start peak_chi_squared_area ################ */
/** 
 * \defgroup peak_chi_squared_area Chi-squared Area
 * \ingroup ff_peak_area
 *
 * \brief \<some brief description of Chi-squared Area function\>
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
 *       <td>\b mode</td>
 *       <td>mode of distribution</td>
 *      </tr><tr>
 *       <td>\b X0</td>
 *       <td>shift parameter</td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b shape</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_chi_squared_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_chi_squared_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_chi_squared_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_chi_squared_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_chi_squared_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_chi_squared_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_chi_squared_area ################ */

/* ################ start peak_erfc_amplitude ################ */
/** 
 * \defgroup peak_erfc_amplitude Erfc Amplitude
 * \ingroup ff_peak_amplitudes
 *
 * \brief \<some brief description of Erfc Amplitude function\>
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
 *       <td>location parameter/td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_erfc_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_erfc_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_erfc_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_erfc_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_erfc_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_erfc_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_erfc_amplitude ################ */

/* ################ start peak_erfc_area ################ */
/** 
 * \defgroup peak_erfc_area Erfc Area
 * \ingroup ff_peak_area
 *
 * \brief \<some brief description of Erfc Area function\>
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
 *       <td>location parameter/td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_erfc_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_erfc_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_erfc_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_erfc_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_erfc_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_erfc_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_erfc_area ################ */

/* ################ start peak_error_amplitude ################ */
/** 
 * \defgroup peak_error_amplitude Error Amplitude
 * \ingroup ff_peak_amplitudes
 *
 * \brief \<some brief description of Error Amplitude function\>
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
 *       <td>location parameter/td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b shape</td>
 *       <td>shape parameter k>0</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_error_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_error_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_error_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_error_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_error_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_error_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_error_amplitude ################ */

/* ################ start peak_error_area ################ */
/** 
 * \defgroup peak_error_area Error Area
 * \ingroup ff_peak_area
 *
 * \brief \<some brief description of Error Area function\>
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
 *       <td>location parameter/td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b shape</td>
 *       <td>shape parameter k>0</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_error_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_error_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_error_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_error_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_error_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_error_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_error_area ################ */


/* ################ start peak_exponentially_modified_gaussian_amplitude ################ */
/** 
 * \defgroup peak_exponentially_modified_gaussian_amplitude Exponentially Modified Gaussian Amplitude
 * \ingroup ff_peak_amplitudes
 *
 * \brief \<some brief description of Exponentially Modified Gaussian Amplitude function\>
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
 *       <td>location parameter/td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b distortion</td>
 *       <td>distortion parameter </td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_exponentially_modified_gaussian_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_exponentially_modified_gaussian_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_exponentially_modified_gaussian_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_exponentially_modified_gaussian_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_exponentially_modified_gaussian_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_exponentially_modified_gaussian_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_exponentially_modified_gaussian_amplitude ################ */

/* ################ start peak_exponentially_modified_gaussian_area ################ */
/** 
 * \defgroup peak_exponentially_modified_gaussian_area Exponentially Modified Gaussian Area
 * \ingroup ff_peak_area
 *
 * \brief \<some brief description of Exponentially Modified Gaussian Area function\>
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
 *       <td>location parameter/td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b distortion</td>
 *       <td>distortion parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_exponentially_modified_gaussian_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_exponentially_modified_gaussian_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_exponentially_modified_gaussian_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_exponentially_modified_gaussian_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_exponentially_modified_gaussian_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_exponentially_modified_gaussian_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_exponentially_modified_gaussian_area ################ */

/* ################ start peak_extreme_value_amplitude ################ */
/** 
 * \defgroup peak_extreme_value_amplitude Extreme Value Amplitude
 * \ingroup ff_peak_amplitudes
 *
 * \brief \<some brief description of Extreme Value Amplitude function\>
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
 *       <td>location parameter/td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_extreme_value_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_extreme_value_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_extreme_value_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_extreme_value_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_extreme_value_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_extreme_value_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_extreme_value_amplitude ################ */

/* ################ start peak_extreme_value_area ################ */
/** 
 * \defgroup peak_extreme_value_area Extreme Value Area
 * \ingroup ff_peak_area
 *
 * \brief \<some brief description of Extreme Value Area function\>
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
 *       <td>location parameter/td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_extreme_value_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_extreme_value_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_extreme_value_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_extreme_value_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_extreme_value_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_extreme_value_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_extreme_value_area ################ */

/* ################ start peak_fatique_life_amplitude ################ */
/** 
 * \defgroup peak_fatique_life_amplitude Fatique Life Amplitude
 * \ingroup ff_peak_amplitudes
 *
 * \brief \<some brief description of Fatique Life Amplitude function\>
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
 *       <td>\b X0</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_fatique_life_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_fatique_life_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_fatique_life_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_fatique_life_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_fatique_life_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_fatique_life_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_fatique_life_amplitude ################ */

/* ################ start peak_fatique_life_area ################ */
/** 
 * \defgroup peak_fatique_life_area Fatique Life Area Area
 * \ingroup ff_peak_area
 *
 * \brief \<some brief description of Fatique Life Area function\>
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
 *       <td>\b X0</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_fatique_life_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_fatique_life_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_fatique_life_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_fatique_life_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_fatique_life_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_fatique_life_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_fatique_life_area ################ */

/* ################ start peak_f_variance_amplitude ################ */
/** 
 * \defgroup peak_f_variance_amplitude F-variance Amplitude
 * \ingroup ff_peak_amplitudes
 *
 * \brief \<some brief description of F-variance Amplitude function\>
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
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b wifth</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b shape1</td>
 *       <td>1st shape parameter</td>
 *      </tr><tr>
 *       <td>\b shape2</td>
 *       <td>2nd shape parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_f_variance_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_f_variance_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_f_variance_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_f_variance_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_f_variance_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_f_variance_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_f_variance_amplitude ################ */

/* ################ start peak_f_variance_area ################ */
/** 
 * \defgroup peak_f_variance_area F-variance Area Area
 * \ingroup ff_peak_area
 *
 * \brief \<some brief description of F-variance Area function\>
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
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b wifth</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b shape1</td>
 *       <td>1st shape parameter</td>
 *      </tr><tr>
 *       <td>\b shape2</td>
 *       <td>2nd shape parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_f_variance_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_f_variance_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_f_variance_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_f_variance_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_f_variance_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_f_variance_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_f_variance_area ################ */

/* ################ start peak_gamma_amplitude ################ */
/** 
 * \defgroup peak_gamma_amplitude Gamma Amplitude
 * \ingroup ff_peak_amplitudes
 *
 * \brief \<some brief description of Gamma Amplitude function\>
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
 *       <td>center of distribution</td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b shape</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_gamma_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gamma_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gamma_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gamma_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gamma_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gamma_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_gamma_amplitude ################ */

/* ################ start peak_gamma_area ################ */
/** 
 * \defgroup peak_gamma_area Gamma Area
 * \ingroup ff_peak_area
 *
 * \brief \<some brief description of Gamma Area function\>
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
 *       <td>center of distribution</td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b shape</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_gamma_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gamma_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gamma_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gamma_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gamma_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gamma_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_gamma_area ################ */

/* ################ start peak_gaussian_amplitude ################ */
/** 
 * \defgroup peak_gaussian_amplitude Gaussian Amplitude
 * \ingroup ff_peak_amplitudes
 *
 * \brief \<some brief description of Gaussian Amplitude function\>
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
 *       <td>center of distribution</td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_gaussian_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gaussian_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gaussian_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_gaussian_amplitude ################ */

/* ################ start peak_gaussian_area ################ */
/** 
 * \defgroup peak_gaussian_area Gaussian Area
 * \ingroup ff_peak_area
 *
 * \brief \<some brief description of Gaussian Area function\>
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
 *       <td>center of distribution</td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_gaussian_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gaussian_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gaussian_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_gaussian_area ################ */

/* ################ start peak_gaussian_lorentzian_cross_product_amplitude ################ */
/** 
 * \defgroup peak_gaussian_lorentzian_cross_product_amplitude Gaussian-Lorentzian Cross-Product Amplitude
 * \ingroup ff_peak_amplitudes
 *
 * \brief \<some brief description of Gaussian-Lorentzian Cross-Product Amplitude function\>
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
 *       <td>center of distribution</td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b shape</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_gaussian_lorentzian_cross_product_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_lorentzian_cross_product_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gaussian_lorentzian_cross_product_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_lorentzian_cross_product_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gaussian_lorentzian_cross_product_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_lorentzian_cross_product_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_gaussian_lorentzian_cross_product_amplitude ################ */

/* ################ start peak_gaussian_lorentzian_cross_product_area ################ */
/** 
 * \defgroup peak_gaussian_lorentzian_cross_product_area Gaussian-Lorentzian Cross-Product Area
 * \ingroup ff_peak_area
 *
 * \brief \<some brief description of Gaussian-Lorentzian Cross-Product Area function\>
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
 *       <td>center of distribution</td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b shape</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_gaussian_lorentzian_cross_product_area
 *
 * \sa sasfit_peak.h, peak_gaussian_lorentzian_cross_product_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_lorentzian_cross_product_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gaussian_lorentzian_cross_product_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_lorentzian_cross_product_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gaussian_lorentzian_cross_product_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_lorentzian_cross_product_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_gaussian_lorentzian_cross_product_area ################ */

/* ################ start peak_gaussian_lorentzian_sum_amplitude ################ */
/** 
 * \defgroup peak_gaussian_lorentzian_sum_amplitude Gaussian-Lorentzian Sum Amplitude
 * \ingroup ff_peak_amplitudes
 *
 * \brief \<some brief description of Gaussian-Lorentzian Sum Amplitude function\>
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
 *       <td>center of distribution</td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b shape</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_gaussian_lorentzian_sum_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_lorentzian_sum_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gaussian_lorentzian_sum_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_lorentzian_sum_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gaussian_lorentzian_sum_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_lorentzian_sum_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_gaussian_lorentzian_sum_amplitude ################ */

/* ################ start peak_gaussian_lorentzian_sum_area ################ */
/** 
 * \defgroup peak_gaussian_lorentzian_sum_area Gaussian-Lorentzian Sum Area
 * \ingroup ff_peak_area
 *
 * \brief \<some brief description of Gaussian-Lorentzian Sum Area function\>
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
 *       <td>center of distribution</td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b shape</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 */

/**
 * \ingroup peak_gaussian_lorentzian_sum_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_lorentzian_sum_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gaussian_lorentzian_sum_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_lorentzian_sum_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gaussian_lorentzian_sum_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_lorentzian_sum_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_gaussian_lorentzian_sum_area ################ */

/* ################ start peak_generalized_gaussian1_amplitude ################ */
/** 
 * \defgroup peak_generalized_gaussian1_amplitude generalized Gaussian1 Amplitude
 * \ingroup ff_peak_amplitudes
 *
 * \brief \<some brief description of generalized Gaussian1 Amplitude function\>
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
 *       <td>\b location</td>
 *       <td>location parameter of distribution</td>
 *      </tr><tr>
 *       <td>\b scale</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b shape</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_generalized_gaussian1_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_gaussian1_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_generalized_gaussian1_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_gaussian1_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_generalized_gaussian1_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_gaussian1_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_generalized_gaussian1_amplitude ################ */

/* ################ start peak_generalized_gaussian1_area ################ */
/** 
 * \defgroup peak_generalized_gaussian1_area generalized Gaussian1 Area
 * \ingroup ff_peak_area
 *
 * \brief \<some brief description of generalized Gaussian1 Area function\>
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
 *       <td>\b location</td>
 *       <td>location parameter of distribution</td>
 *      </tr><tr>
 *       <td>\b scale</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b shape</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_generalized_gaussian1_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_gaussian1_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_generalized_gaussian1_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_gaussian1_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_generalized_gaussian1_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_gaussian1_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_generalized_gaussian1_area ################ */

/* ################ start peak_generalized_gaussian2_area ################ */
/** 
 * \defgroup peak_generalized_gaussian2_area generalized Gaussian2 Area
 * \ingroup ff_peak_area
 *
 * \brief \<some brief description of generalized Gaussian2 Area function\>
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
 *       <td>\b location</td>
 *       <td>location parameter of distribution</td>
 *      </tr><tr>
 *       <td>\b scale</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b shape</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_generalized_gaussian2_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_gaussian2_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_generalized_gaussian2_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_gaussian2_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_generalized_gaussian2_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_gaussian2_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_generalized_gaussian2_are ################ */

/* ################ start peak_generalized_gaussian2_amplitude ################ */
/** 
 * \defgroup peak_generalized_gaussian2_amplitude generalized Gaussian2 Amplitude
 * \ingroup ff_peak_amplitudes
 *
 * \brief \<some brief description of generalized Gaussian2 Amplitude function\>
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
 *       <td>\b location</td>
 *       <td>location parameter of distribution</td>
 *      </tr><tr>
 *       <td>\b scale</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b shape</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_generalized_gaussian2_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_gaussian2_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_generalized_gaussian2_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_gaussian2_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_generalized_gaussian2_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_gaussian2_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_generalized_gaussian2_amplitude ################ */

/* ################ start peak_generalized_maxwell_area ################ */
/** 
 * \defgroup peak_generalized_maxwell_area generalized Maxwell Area
 * \ingroup ff_peak_area
 *
 * \brief \<some brief description of generalized Maxwell Area function\>
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
 *       <td>\b offset</td>
 *       <td>offset parameter of distribution</td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b n</td>
 *       <td>first shape parameter</td>
 *      </tr><tr>
 *       <td>\b m</td>
 *       <td>second shape parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_generalized_maxwell_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_maxwell_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_generalized_gaussian2_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_maxwell_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_generalized_gaussian2_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_maxwell_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sasfit_peak_generalized_maxwell_area ################ */

/* ################ start peak_generalized_maxwell_amplitude ################ */
/** 
 * \defgroup peak_generalized_maxwell_amplitude Maxwell Amplitude
 * \ingroup ff_peak_amplitudes
 *
 * \brief \<some brief description of Maxwell Amplitude function\>
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
 *       <td>\b offset</td>
 *       <td>offset parameter of distribution</td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b n</td>
 *       <td>first shape parameter</td>
 *      </tr><tr>
 *       <td>\b m</td>
 *       <td>second shape parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_generalized_maxwell_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_maxwell_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_generalized_maxwell_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_maxwell_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_generalized_maxwell_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_maxwell_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_generalized_maxwell_amplitude ################ */

/* ################ start peak_gex_amplitude ################ */
/** 
 * \defgroup peak_gex_amplitude GEX Amplitude
 * \ingroup ff_peak_amplitudes
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
 *       <td>\b mode</td>
 *       <td>mode of distribution</td>
 *      </tr><tr>
 *       <td>\b X0</td>
 *       <td>shift parameter</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>first shape parameter</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>second shape parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_gex_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gex_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gex_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gex_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gex_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gex_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_gex_amplitude ################ */

/* ################ start peak_gex_area ################ */
/** 
 * \defgroup peak_gex_area GEX Area
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
 *       <td>\b mode</td>
 *       <td>mode of distribution</td>
 *      </tr><tr>
 *       <td>\b X0</td>
 *       <td>shift parameter</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>first shape parameter</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>second shape parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_gex_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gex_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gex_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gex_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gex_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gex_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_gex_area ################ */

/* ################ start peak_giddings_area ################ */
/** 
 * \defgroup peak_giddings_area Giddings Area
 * \ingroup ff_peak_area
 *
 * \brief \<some brief description of Giddings Area function\>
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
 *       <td>\b mode</td>
 *       <td>mode of distribution</td>
 *      </tr><tr>
 *       <td>\b X0</td>
 *       <td>shift parameter</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>first shape parameter</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>second shape parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_giddings_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_giddings_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_giddings_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_giddings_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_giddings_area
 *
 * \sa sasfit_peak.h, ff_peak_area
 */
sasfit_peak_DLLEXP scalar sasfit_peak_giddings_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_giddings_area ################ */

/* ################ start peak_giddings_amplitude ################ */
/** 
 * \defgroup peak_giddings_amplitude Giddings Amplitude
 * \ingroup ff_peak_amplitudes
 *
 * \brief \<some brief description of Giddings Amplitude function\>
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
 *       <td>\b mode</td>
 *       <td>mode of distribution</td>
 *      </tr><tr>
 *       <td>\b X0</td>
 *       <td>shift parameter</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>first shape parameter</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>second shape parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_giddings_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_giddings_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_giddings_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_giddings_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_giddings_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes
 */
sasfit_peak_DLLEXP scalar sasfit_peak_giddings_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_giddings_amplitude ################ */



#endif // this file

