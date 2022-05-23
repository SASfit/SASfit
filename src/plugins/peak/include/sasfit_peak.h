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
 * \ingroup ff_peak_amplitudes_a_m
 *
 * \brief \<some brief description of Beta Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>amplitude</td>
 *      </tr><tr>
 *       <td>\b x_min</td>
 *       <td>lower bound</td>
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
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_beta_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_beta_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_beta_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_beta_amplitude_a_m
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_beta_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_beta_amplitude ################ */

/* ################ start peak_beta_area ################ */
/**
 * \defgroup peak_beta_area Beta Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of Beta Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_beta_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_beta_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_beta_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_beta_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_beta_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_beta_area ################ */

/* ################ start peak_chi_squared_amplitude ################ */
/**
 * \defgroup peak_chi_squared_amplitude Chi-squared Amplitude
 * \ingroup ff_peak_amplitudes_a_m
 *
 * \brief \<some brief description of Chi-squared Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_chi_squared_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_chi_squared_amplitude_
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_chi_squared_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_chi_squared_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_chi_squared_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_chi_squared_amplitude ################ */

/* ################ start peak_chi_squared_area ################ */
/**
 * \defgroup peak_chi_squared_area Chi-squared Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of Chi-squared Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_chi_squared_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_chi_squared_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_chi_squared_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_chi_squared_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_chi_squared_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_chi_squared_area ################ */

/* ################ start peak_erfc_amplitude ################ */
/**
 * \defgroup peak_erfc_amplitude Erfc Amplitude
 * \ingroup ff_peak_amplitudes_a_m
 *
 * \brief \<some brief description of Erfc Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_erfc_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_erfc_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_erfc_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_erfc_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_erfc_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_erfc_amplitude ################ */

/* ################ start peak_erfc_area ################ */
/**
 * \defgroup peak_erfc_area Erfc Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of Erfc Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_erfc_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_erfc_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_erfc_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_erfc_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_erfc_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_erfc_area ################ */

/* ################ start peak_error_amplitude ################ */
/**
 * \defgroup peak_error_amplitude Error Amplitude
 * \ingroup ff_peak_amplitudes_a_m
 *
 * \brief \<some brief description of Error Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_error_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_error_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_error_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_error_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_error_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_error_amplitude ################ */

/* ################ start peak_error_area ################ */
/**
 * \defgroup peak_error_area Error Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of Error Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_error_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_error_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_error_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_error_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_error_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_error_area ################ */


/* ################ start peak_exponentially_modified_gaussian_amplitude ################ */
/**
 * \defgroup peak_exponentially_modified_gaussian_amplitude Exponentially Modified Gaussian Amplitude
 * \ingroup ff_peak_amplitudes_a_m
 *
 * \brief \<some brief description of Exponentially Modified Gaussian Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_exponentially_modified_gaussian_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_exponentially_modified_gaussian_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_exponentially_modified_gaussian_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_exponentially_modified_gaussian_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_exponentially_modified_gaussian_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_exponentially_modified_gaussian_amplitude ################ */

/* ################ start peak_exponentially_modified_gaussian_area ################ */
/**
 * \defgroup peak_exponentially_modified_gaussian_area Exponentially Modified Gaussian Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of Exponentially Modified Gaussian Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_exponentially_modified_gaussian_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_exponentially_modified_gaussian_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_exponentially_modified_gaussian_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_exponentially_modified_gaussian_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_exponentially_modified_gaussian_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_exponentially_modified_gaussian_area ################ */

/* ################ start peak_extreme_value_amplitude ################ */
/**
 * \defgroup peak_extreme_value_amplitude Extreme Value Amplitude
 * \ingroup ff_peak_amplitudes_a_m
 *
 * \brief \<some brief description of Extreme Value Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_extreme_value_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_extreme_value_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_extreme_value_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_extreme_value_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_extreme_value_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_extreme_value_amplitude ################ */

/* ################ start peak_extreme_value_area ################ */
/**
 * \defgroup peak_extreme_value_area Extreme Value Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of Extreme Value Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_extreme_value_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_extreme_value_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_extreme_value_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_extreme_value_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_extreme_value_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_extreme_value_area ################ */

/* ################ start peak_fatique_life_amplitude ################ */
/**
 * \defgroup peak_fatique_life_amplitude Fatique Life Amplitude
 * \ingroup ff_peak_amplitudes_a_m
 *
 * \brief \<some brief description of Fatique Life Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_fatique_life_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_fatique_life_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_fatique_life_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_fatique_life_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_fatique_life_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_fatique_life_amplitude ################ */

/* ################ start peak_fatique_life_area ################ */
/**
 * \defgroup peak_fatique_life_area Fatique Life Area Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of Fatique Life Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_fatique_life_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_fatique_life_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_fatique_life_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_fatique_life_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_fatique_life_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_fatique_life_area ################ */

/* ################ start peak_f_variance_amplitude ################ */
/**
 * \defgroup peak_f_variance_amplitude F-variance Amplitude
 * \ingroup ff_peak_amplitudes_a_m
 *
 * \brief \<some brief description of F-variance Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_f_variance_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_f_variance_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_f_variance_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_f_variance_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_f_variance_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_f_variance_amplitude ################ */

/* ################ start peak_f_variance_area ################ */
/**
 * \defgroup peak_f_variance_area F-variance Area Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of F-variance Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_f_variance_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_f_variance_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_f_variance_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_f_variance_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_f_variance_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_f_variance_area ################ */

/* ################ start peak_gamma_amplitude ################ */
/**
 * \defgroup peak_gamma_amplitude Gamma Amplitude
 * \ingroup ff_peak_amplitudes_a_m
 *
 * \brief \<some brief description of Gamma Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gamma_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gamma_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gamma_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gamma_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gamma_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_gamma_amplitude ################ */

/* ################ start peak_gamma_area ################ */
/**
 * \defgroup peak_gamma_area Gamma Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of Gamma Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gamma_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gamma_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gamma_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gamma_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gamma_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_gamma_area ################ */

/* ################ start peak_gaussian_amplitude ################ */
/**
 * \defgroup peak_gaussian_amplitude Gaussian Amplitude
 * \ingroup ff_peak_amplitudes_a_m
 *
 * \brief \<some brief description of Gaussian Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gaussian_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gaussian_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_gaussian_amplitude ################ */

/* ################ start peak_gaussian_area ################ */
/**
 * \defgroup peak_gaussian_area Gaussian Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of Gaussian Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gaussian_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gaussian_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_gaussian_area ################ */

/* ################ start peak_gaussian_lorentzian_cross_product_amplitude ################ */
/**
 * \defgroup peak_gaussian_lorentzian_cross_product_amplitude Gaussian-Lorentzian Cross-Product Amplitude
 * \ingroup ff_peak_amplitudes_a_m
 *
 * \brief \<some brief description of Gaussian-Lorentzian Cross-Product Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_lorentzian_cross_product_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gaussian_lorentzian_cross_product_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_lorentzian_cross_product_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gaussian_lorentzian_cross_product_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_lorentzian_cross_product_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_gaussian_lorentzian_cross_product_amplitude ################ */

/* ################ start peak_gaussian_lorentzian_cross_product_area ################ */
/**
 * \defgroup peak_gaussian_lorentzian_cross_product_area Gaussian-Lorentzian Cross-Product Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of Gaussian-Lorentzian Cross-Product Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_lorentzian_cross_product_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gaussian_lorentzian_cross_product_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_lorentzian_cross_product_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_gaussian_lorentzian_cross_product_area ################ */

/* ################ start peak_gaussian_lorentzian_sum_amplitude ################ */
/**
 * \defgroup peak_gaussian_lorentzian_sum_amplitude Gaussian-Lorentzian Sum Amplitude
 * \ingroup ff_peak_amplitudes_a_m
 *
 * \brief \<some brief description of Gaussian-Lorentzian Sum Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_lorentzian_sum_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gaussian_lorentzian_sum_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_lorentzian_sum_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gaussian_lorentzian_sum_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_lorentzian_sum_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_gaussian_lorentzian_sum_amplitude ################ */

/* ################ start peak_gaussian_lorentzian_sum_area ################ */
/**
 * \defgroup peak_gaussian_lorentzian_sum_area Gaussian-Lorentzian Sum Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of Gaussian-Lorentzian Sum Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_lorentzian_sum_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gaussian_lorentzian_sum_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_lorentzian_sum_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gaussian_lorentzian_sum_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gaussian_lorentzian_sum_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_gaussian_lorentzian_sum_area ################ */

/* ################ start peak_generalized_gaussian1_amplitude ################ */
/**
 * \defgroup peak_generalized_gaussian1_amplitude generalized Gaussian1 Amplitude
 * \ingroup ff_peak_amplitudes_generalized
 *
 * \brief \<some brief description of generalized Gaussian1 Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_amplitudes_generalized
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_gaussian1_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_generalized_gaussian1_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_generalized
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_gaussian1_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_generalized_gaussian1_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_generalized
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_gaussian1_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_generalized_gaussian1_amplitude ################ */

/* ################ start peak_generalized_gaussian1_area ################ */
/**
 * \defgroup peak_generalized_gaussian1_area generalized Gaussian1 Area
 * \ingroup ff_peak_area_generalized
 *
 * \brief \<some brief description of generalized Gaussian1 Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_area_generalized
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_gaussian1_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_generalized_gaussian1_area
 *
 * \sa sasfit_peak.h, ff_peak_area_generalized
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_gaussian1_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_generalized_gaussian1_area
 *
 * \sa sasfit_peak.h, ff_peak_area_generalized
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_gaussian1_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_generalized_gaussian1_area ################ */

/* ################ start peak_generalized_gaussian2_area ################ */
/**
 * \defgroup peak_generalized_gaussian2_area generalized Gaussian2 Area
 * \ingroup ff_peak_area_generalized
 *
 * \brief \<some brief description of generalized Gaussian2 Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_area_generalized
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_gaussian2_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_generalized_gaussian2_area
 *
 * \sa sasfit_peak.h, ff_peak_area_generalized
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_gaussian2_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_generalized_gaussian2_area
 *
 * \sa sasfit_peak.h, ff_peak_area_generalized
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_gaussian2_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_generalized_gaussian2_are ################ */

/* ################ start peak_generalized_gaussian2_amplitude ################ */
/**
 * \defgroup peak_generalized_gaussian2_amplitude generalized Gaussian2 Amplitude
 * \ingroup ff_peak_amplitudes_generalized
 *
 * \brief \<some brief description of generalized Gaussian2 Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_amplitudes_generalized
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_gaussian2_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_generalized_gaussian2_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_generalized
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_gaussian2_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_generalized_gaussian2_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_generalized
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_gaussian2_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_generalized_gaussian2_amplitude ################ */

/* ################ start peak_generalized_maxwell_area ################ */
/**
 * \defgroup peak_generalized_maxwell_area generalized Maxwell Area
 * \ingroup ff_peak_area_generalized
 *
 * \brief \<some brief description of generalized Maxwell Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_area_generalized
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_maxwell_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_generalized_gaussian2_area
 *
 * \sa sasfit_peak.h, ff_peak_area_generalized
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_maxwell_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_generalized_gaussian2_area
 *
 * \sa sasfit_peak.h, ff_peak_area_generalized
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_maxwell_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sasfit_peak_generalized_maxwell_area ################ */

/* ################ start peak_generalized_maxwell_amplitude ################ */
/**
 * \defgroup peak_generalized_maxwell_amplitude generalized Maxwell Amplitude
 * \ingroup ff_peak_amplitudes_generalized
 *
 * \brief \<some brief description of generalized Maxwell Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_amplitudes_generalized
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_maxwell_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_generalized_maxwell_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_generalized
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_maxwell_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_generalized_maxwell_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_generalized
 */
sasfit_peak_DLLEXP scalar sasfit_peak_generalized_maxwell_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_generalized_maxwell_amplitude ################ */

/* ################ start peak_gex_amplitude ################ */
/**
 * \defgroup peak_gex_amplitude GEX Amplitude
 * \ingroup ff_peak_amplitudes_generalized
 *
 * \brief \<some brief description of GEX Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_amplitudes_generalized
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gex_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gex_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_generalized
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gex_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gex_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_generalized
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gex_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_gex_amplitude ################ */

/* ################ start peak_gex_area ################ */
/**
 * \defgroup peak_gex_area GEX Area
 * \ingroup ff_peak_area_generalized
 *
 * \brief \<some brief description of GEX Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_area_generalized
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gex_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gex_area
 *
 * \sa sasfit_peak.h, ff_peak_area_generalized
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gex_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_gex_area
 *
 * \sa sasfit_peak.h, ff_peak_area_generalized
 */
sasfit_peak_DLLEXP scalar sasfit_peak_gex_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_gex_area ################ */

/* ################ start peak_giddings_area ################ */
/**
 * \defgroup peak_giddings_area Giddings Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of Giddings Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_giddings_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_giddings_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_giddings_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_giddings_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_giddings_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_giddings_area ################ */

/* ################ start peak_giddings_amplitude ################ */
/**
 * \defgroup peak_giddings_amplitude Giddings Amplitude
 * \ingroup ff_peak_amplitudes_a_m
 *
 * \brief \<some brief description of Giddings Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_giddings_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_giddings_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_giddings_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_giddings_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_giddings_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_giddings_amplitude ################ */

/* ################ start peak_haarhoff_vander_linde_area ################ */
/**
 * \defgroup peak_haarhoff_vander_linde_area Haarhoff Van der Linde Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of Haarhoff Van der Linde Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>center parameter of distribution</td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b distortion</td>
 *       <td>distortion parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_haarhoff_vander_linde_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_haarhoff_vander_linde_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_haarhoff_vander_linde_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_haarhoff_vander_linde_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_haarhoff_vander_linde_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_haarhoff_vander_linde_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_haarhoff_vander_linde_area ################ */

/* ################ start peak_half_gaussian_modified_gaussian_area ################ */
/**
 * \defgroup peak_half_gaussian_modified_gaussian_area half Gaussian modified Gaussian Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of half Gaussian modified Gaussian Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>center parameter of distribution</td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b distortion</td>
 *       <td>distortion parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_half_gaussian_modified_gaussian_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_half_gaussian_modified_gaussian_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_half_gaussian_modified_gaussian_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_half_gaussian_modified_gaussian_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_half_gaussian_modified_gaussian_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_half_gaussian_modified_gaussian_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_half_gaussian_modified_gaussian_area ################ */

/* ################ start peak_inverted_gamma_area ################ */
/**
 * \defgroup peak_inverted_gamma_area Inverted Gamma Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of Inverted Gamma Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>center parameter of distribution</td>
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
 * \ingroup peak_inverted_gamma_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_inverted_gamma_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_inverted_gamma_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_inverted_gamma_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_inverted_gamma_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_inverted_gamma_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_inverted_gamma_area ################ */

/* ################ start peak_inverted_gamma_amplitude ################ */
/**
 * \defgroup peak_inverted_gamma_amplitude Inverted Gamma Amplitude
 * \ingroup ff_peak_amplitudes_a_m
 *
 * \brief \<some brief description of Inverted Gamma Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>amplitude</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>center parameter of distribution</td>
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
 * \ingroup peak_inverted_gamma_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_inverted_gamma_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_inverted_gamma_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_inverted_gamma_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_inverted_gamma_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_inverted_gamma_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_inverted_gamma_amplitude ################ */

/* ################ start peak_kumaraswamy_area ################ */
/**
 * \defgroup peak_kumaraswamy_area Kumaraswamy Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of Kumaraswamy Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b xmin</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b xmax</td>
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
 * \ingroup peak_kumaraswamy_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_kumaraswamy_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_kumaraswamy_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_kumaraswamy_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_kumaraswamy_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_kumaraswamy_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_kumaraswamy_area ################ */

/* ################ start peak_kumaraswamy_amplitude ################ */
/**
 * \defgroup peak_kumaraswamy_amplitude Kumaraswamy Amplitude
 * \ingroup ff_peak_amplitudes_a_m
 *
 * \brief \<some brief description of Kumaraswamy Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>amplitude</td>
 *      </tr><tr>
 *       <td>\b xmin</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b xmax</td>
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
 * \ingroup peak_kumaraswamy_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_kumaraswamy_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_kumaraswamy_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_kumaraswamy_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_kumaraswamy_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_kumaraswamy_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_kumaraswamy_amplitude ################ */

/* ################ start peak_laplace_area ################ */
/**
 * \defgroup peak_laplace_area Laplace Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of Laplace Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>location parameter</td>
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
 * \ingroup peak_laplace_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_laplace_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_laplace_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_laplace_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_laplace_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_laplace_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_laplace_area ################ */

/* ################ start peak_laplace_amplitude ################ */
/**
 * \defgroup peak_laplace_amplitude Laplace Amplitude
 * \ingroup ff_peak_amplitudes_a_m
 *
 * \brief \<some brief description of Laplace Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>amplitude</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>location parameter</td>
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
 * \ingroup peak_laplace_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_laplace_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_laplace_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_laplace_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_laplace_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_laplace_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_laplace_amplitude ################ */

/* ################ start peak_logistic_area ################ */
/**
 * \defgroup peak_logistic_area Logistic Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of Logistic Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>location parameter</td>
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
 * \ingroup peak_logistic_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_logistic_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_logistic_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_logistic_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_logistic_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_logistic_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_logistic_area ################ */

/* ################ start peak_logistic_amplitude ################ */
/**
 * \defgroup peak_logistic_amplitude Logistic Amplitude
 * \ingroup ff_peak_amplitudes_a_m
 *
 * \brief \<some brief description of Logistic Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>amplitude</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>location parameter</td>
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
 * \ingroup peak_logistic_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_logistic_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_logistic_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_logistic_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_logistic_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_logistic_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_logistic_amplitude ################ */

/* ################ start peak_logit_amplitude ################ */
/**
 * \defgroup peak_logit_amplitude LogitNormal Amplitude
 * \ingroup ff_peak_amplitudes_a_m
 *
 * \brief \<some brief description of LogitNormal Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>amplitude</td>
 *      </tr><tr>
 *       <td>\b x_min</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b x_max</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_logit_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_logit_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_logit_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_logit_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_logit_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_logit_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_logit_amplitude ################ */

/* ################ start peak_logit_area ################ */
/**
 * \defgroup peak_logit_area LogitNormal Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of LogitNormal Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
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
 *       <td>\b mu</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_logit_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_logit_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_logit_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_logit_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_logit_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_logit_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_logit_area ################ */


/* ################ start peak_loglogistic_area ################ */
/**
 * \defgroup peak_loglogistic_area LogLogistic Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of LogLogistic Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b x0</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
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
 * \ingroup peak_loglogistic_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_loglogistic_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_loglogistic_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_loglogistic_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_loglogistic_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_loglogistic_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_loglogistic_area ################ */

/* ################ start peak_loglogistic_amplitude ################ */
/**
 * \defgroup peak_loglogistic_amplitude LogLogistic Amplitude
 * \ingroup ff_peak_amplitudes_a_m
 *
 * \brief \<some brief description of LogLogistic Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>amplitude</td>
 *      </tr><tr>
 *       <td>\b x0</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
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
 * \ingroup peak_loglogistic_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_loglogistic_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_loglogistic_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_loglogistic_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_loglogistic_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_loglogistic_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_loglogistic_amplitude ################ */

/* ################ start peak_lognormal_area ################ */
/**
 * \defgroup peak_lognormal_area LogNormal Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of LogNormal Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b x0</td>
 *       <td>shift parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_lognormal_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_lognormal_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_lognormal_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_lognormal_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_lognormal_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_lognormal_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_lognormal_area ################ */

/* ################ start peak_lognormal_amplitude ################ */
/**
 * \defgroup peak_lognormal_amplitude LogNormal Amplitude
 * \ingroup ff_peak_amplitudes_a_m
 *
 * \brief \<some brief description of LogNormal Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>amplitude</td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b x0</td>
 *       <td>shift parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_lognormal_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_lognormal_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_lognormal_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_lognormal_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_lognormal_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_lognormal_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_lognormal_amplitude ################ */

/* ################ start peak_lognormal4parameter_area ################ */
/**
 * \defgroup peak_lognormal4parameter_area LogNormal4Parameter Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of LogNorma4Parameterl Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>location parameter</td>
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
 * \ingroup peak_lognormal4parameter_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_lognormal4parameter_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_lognormal4parameter_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_lognormal4parameter_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_lognormal4parameter_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_lognormal4parameter_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_lognormal4parameter_area ################ */

/* ################ start peak_lognormal4parameter_amplitude ################ */
/**
 * \defgroup peak_lognormal4parameter_amplitude LogNormal4Parameter Amplitude
 * \ingroup ff_peak_amplitudes_a_m
 *
 * \brief \<some brief description of LogNormal4Parameter Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>amplitude</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>location parameter</td>
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
 * \ingroup peak_lognormal4parameter_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_lognormal4parameter_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_lognormal4parameter_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_lognormal4parameter_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_lognormal4parameter_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_lognormal4parameter_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_lognormal4parameter_amplitude ################ */

/* ################ start peak_lorentzian_area ################ */
/**
 * \defgroup peak_lorentzian_area Lorentzian Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of Lorentzian Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>location parameter</td>
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
 * \ingroup peak_lorentzian_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_lorentzian_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_lorentzian_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_lorentzian_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_lorentzian_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_lorentzian_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_lorentzian_area ################ */

/* ################ start peak_lorentzian_amplitude ################ */
/**
 * \defgroup peak_lorentzian_amplitude Lorentzian Amplitude
 * \ingroup ff_peak_amplitudes_a_m
 *
 * \brief \<some brief description of Lorentzian Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>amplitude</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>location parameter</td>
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
 * \ingroup peak_lorentzian_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_lorentzian_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_lorentzian_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_lorentzian_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_lorentzian_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_lorentzian_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_lorentzian_amplitude ################ */

/* ################ start peak_maxwell_area ################ */
/**
 * \defgroup peak_maxwell_area Maxwell Area
 * \ingroup ff_peak_area_a_m
 *
 * \brief \<some brief description of Maxwell Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b x0</td>
 *       <td>offset parameter</td>
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
 * \ingroup peak_maxwell_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_maxwell_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_maxwell_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_maxwell_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_maxwell_area
 *
 * \sa sasfit_peak.h, ff_peak_area_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_maxwell_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_maxwell_area ################ */

/* ################ start peak_maxwell_amplitude ################ */
/**
 * \defgroup peak_maxwell_amplitude Maxwell Amplitude
 * \ingroup ff_peak_amplitudes_a_m
 *
 * \brief \<some brief description of Maxwell Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>amplitude</td>
 *      </tr><tr>
 *       <td>\b x0</td>
 *       <td>offset parameter</td>
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
 * \ingroup peak_maxwell_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_maxwell_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_maxwell_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_maxwell_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_maxwell_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_a_m
 */
sasfit_peak_DLLEXP scalar sasfit_peak_maxwell_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_maxwell_amplitude ################ */

/* ################ start peak_pearsonIV_area ################ */
/**
 * \defgroup peak_pearsonIV_area PearsonIV Area
 * \ingroup ff_peak_area_n_z
 *
 * \brief \<some brief description of PearsonIV Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>location parameter </td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b shape1</td>
 *       <td>first shape parameter </td>
 *      </tr><tr>
 *       <td>\b shape2</td>
 *       <td>second shape parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_pearsonIV_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pearsonIV_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_pearsonIV_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pearsonIV_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_pearsonIV_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pearsonIV_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_pearsonIV_area ################ */

/* ################ start peak_pearsonIV_amplitude ################ */
/**
 * \defgroup peak_pearsonIV_amplitude PearsonIV Amplitude
 * \ingroup ff_peak_amplitudes_n_z
 *
 * \brief \<some brief description of PearsonIV Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>amplitude</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>location parameter </td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b shape1</td>
 *       <td>first shape parameter </td>
 *      </tr><tr>
 *       <td>\b shape2</td>
 *       <td>second shape parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_pearsonIV_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pearsonIV_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_pearsonIV_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pearsonIV_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_pearsonIV_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pearsonIV_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_pearsonIV_amplitude ################ */

/* ################ start peak_pearsonVII_area ################ */
/**
 * \defgroup peak_pearsonVII_area PearsonVII Area
 * \ingroup ff_peak_area_n_z
 *
 * \brief \<some brief description of PearsonVII Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>location parameter </td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b shape</td>
 *       <td>shape parameter </td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_pearsonVII_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pearsonVII_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_pearsonVII_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pearsonVII_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_pearsonVII_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pearsonVII_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_pearsonVII_area ################ */

/* ################ start peak_pearsonVII_amplitude ################ */
/**
 * \defgroup peak_pearsonVII_amplitude PearsonVII Amplitude
 * \ingroup ff_peak_amplitudes_n_z
 *
 * \brief \<some brief description of PearsonVII Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>amplitude</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>location parameter </td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b shape</td>
 *       <td>shape parameter </td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_pearsonVII_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pearsonVII_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_pearsonVII_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pearsonVII_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_pearsonVII_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pearsonVII_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_pearsonVII_amplitude ################ */

/* ################ start peak_powerlognormal_area ################ */
/**
 * \defgroup peak_powerlognormal_area power LogNormal Area
 * \ingroup ff_peak_area_n_z
 *
 * \brief \<some brief description of power LogNormal Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b location</td>
 *       <td>location parameter </td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b shape</td>
 *       <td>first shape parameter </td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td>second shape parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_powerlognormal_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_powerlognormal_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_powerlognormal_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_powerlognormal_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_powerlognormal_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_powerlognormal_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_powerlognormal_area ################ */

/* ################ start peak_powernormal_area ################ */
/**
 * \defgroup peak_powernormal_area power Normal Area
 * \ingroup ff_peak_area_n_z
 *
 * \brief \<some brief description of power Normal Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b location</td>
 *       <td>location parameter </td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b shape</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_powernormal_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_powernormal_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_powernormal_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_powernormal_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_powernormal_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_powernormal_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_powernormal_area ################ */

/* ################ start peak_pulse_area ################ */
/**
 * \defgroup peak_pulse_area Pulse Area
 * \ingroup ff_peak_area_n_z
 *
 * \brief \<some brief description of Pulse Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>location parameter </td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter </td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_pulse_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pulse_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_pulse_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pulse_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_pulse_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pulse_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_pulse_area ################ */

/* ################ start peak_pulse_amplitude ################ */
/**
 * \defgroup peak_pulse_amplitude Pulse Amplitude
 * \ingroup ff_peak_amplitudes_n_z
 *
 * \brief \<some brief description of Pulse Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>amplitude</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>location parameter </td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter </td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_pulse_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pulse_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_pulse_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pulse_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_pulse_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pulse_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_pulse_amplitude ################ */

/* ################ start peak_pulse_with_2nd_width_area ################ */
/**
 * \defgroup peak_pulse_with_2nd_width_area Pulse with 2nd width term Area
 * \ingroup ff_peak_area_n_z
 *
 * \brief \<some brief description of Pulse with 2nd width term Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>location parameter </td>
 *      </tr><tr>
 *       <td>\b width1</td>
 *       <td>first width parameter</td>
 *      </tr><tr>
 *       <td>\b width2</td>
 *       <td>second width parameter </td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_pulse_with_2nd_width_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pulse_with_2nd_width_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_pulse_with_2nd_width_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pulse_with_2nd_width_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_pulse_with_2nd_width_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pulse_with_2nd_width_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_pulse_with_2nd_width_area ################ */

/* ################ start peak_pulse_with_2nd_width_amplitude ################ */
/**
 * \defgroup peak_pulse_with_2nd_width_amplitude Pulse with 2nd width term Amplitude
 * \ingroup ff_peak_amplitudes_n_z
 *
 * \brief \<some brief description of Pulse with 2nd width term Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>amplitude</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>location parameter </td>
 *      </tr><tr>
 *       <td>\b width1</td>
 *       <td>first width parameter</td>
 *      </tr><tr>
 *       <td>\b width2</td>
 *       <td>second width parameter </td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_pulse_with_2nd_width_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pulse_with_2nd_width_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_pulse_with_2nd_width_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pulse_with_2nd_width_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_pulse_with_2nd_width_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pulse_with_2nd_width_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_pulse_with_2nd_width_amplitude ################ */

/* ################ start peak_pulse_with_power_term_area ################ */
/**
 * \defgroup peak_pulse_with_power_term_area Pulse with power term Area
 * \ingroup ff_peak_area_n_z
 *
 * \brief \<some brief description of Pulse with power term Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>location parameter </td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b shape</td>
 *       <td>shape parameter </td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_pulse_with_power_term_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pulse_with_power_term_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_pulse_with_power_term_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pulse_with_power_term_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_pulse_with_power_term_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pulse_with_power_term_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_pulse_with_power_term_area ################ */

/* ################ start peak_pulse_with_power_term_amplitude ################ */
/**
 * \defgroup peak_pulse_with_power_term_amplitude Pulse with power term Amplitude
 * \ingroup ff_peak_amplitudes_n_z
 *
 * \brief \<some brief description of Pulse with power term Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>amplitude</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>location parameter </td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b shaper</td>
 *       <td>shape parameter </td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_pulse_with_power_term_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pulse_with_power_term_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_pulse_with_power_term_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pulse_with_power_term_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_pulse_with_power_term_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_pulse_with_power_term_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_pulse_with_power_term_amplitude ################ */

/* ################ start peak_student_t_area ################ */
/**
 * \defgroup peak_student_t_area Student-t Area
 * \ingroup ff_peak_area_n_z
 *
 * \brief \<some brief description of Student-t Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>location parameter </td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b shape</td>
 *       <td>shape parameter </td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_student_t_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_student_t_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_student_t_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_student_t_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_student_t_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_student_t_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_student_t_area ################ */

/* ################ start peak_student_t_amplitude ################ */
/**
 * \defgroup peak_student_t_amplitude Student-t Amplitude
 * \ingroup ff_peak_amplitudes_n_z
 *
 * \brief \<some brief description of Student-t Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>amplitude</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>location parameter </td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b shape</td>
 *       <td>shape parameter </td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_student_t_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_student_t_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_student_t_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_student_t_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_student_t_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_student_t_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_student_t_amplitude ################ */

/* ################ start peak_voigt_area ################ */
/**
 * \defgroup peak_voigt_area Voigt Area
 * \ingroup ff_peak_area_n_z
 *
 * \brief \<some brief description of Voigt Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>location parameter </td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>Gaussian width parameter</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>Lorentzian width parameter </td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 */

/**
 * \ingroup peak_voigt_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_voigt_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_voigt_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_voigt_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_voigt_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_voigt_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_voigt_area ################ */

/* ################ start peak_voigt_amplitude ################ */
/**
 * \defgroup peak_voigt_amplitude Voigt Amplitude
 * \ingroup ff_peak_amplitudes_n_z
 *
 * \brief \<some brief description of Voigt Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>amplitude</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>location parameter </td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>Gaussian width parameter</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>Lorentzian width parameter </td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_voigt_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_voigt_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_voigt_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_voigt_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_voigt_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_voigt_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_voigt_amplitude ################ */

/* ################ start peak_weibull_area ################ */
/**
 * \defgroup peak_weibull_area Weibull Area
 * \ingroup ff_peak_area_n_z
 *
 * \brief \<some brief description of Weibull Area function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>area</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>location parameter </td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter </td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_weibull_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_weibull_area(scalar q, sasfit_param * p);

/**
 * \ingroup peak_weibull_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_weibull_area_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_weibull_area
 *
 * \sa sasfit_peak.h, ff_peak_area_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_weibull_area_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_weibull_area ################ */

/* ################ start peak_weibull_amplitude ################ */
/**
 * \defgroup peak_weibull_amplitude Weibull Amplitude
 * \ingroup ff_peak_amplitudes_n_z
 *
 * \brief \<some brief description of Weibull Amplitude function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>amplitude</td>
 *      </tr><tr>
 *       <td>\b center</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b width</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b shape</td>
 *       <td>shape parameter </td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy parameter</td>
 *      </tr><tr>
 *       <td>\b backgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup peak_weibull_amplitudes
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_weibull_amplitude(scalar q, sasfit_param * p);

/**
 * \ingroup peak_weibull_amplitudes
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_weibull_amplitude_f(scalar q, sasfit_param * p);

/**
 * \ingroup peak_weibull_amplitude
 *
 * \sa sasfit_peak.h, ff_peak_amplitudes_n_z
 */
sasfit_peak_DLLEXP scalar sasfit_peak_weibull_amplitude_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peak_weibull_amplitude ################ */

#endif // this file

