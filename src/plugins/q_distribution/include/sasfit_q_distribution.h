/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_Q_DISTRIBUTION_H
#define SASFIT_PLUGIN_Q_DISTRIBUTION_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_q_distribution.h
 * Public available functions and descriptions of the q_distribution plugin.
 */

/**
 * \def sasfit_q_distribution_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_q_distribution_EXPORTS)
	#ifdef sasfit_q_distribution_DLLEXP
	#undef sasfit_q_distribution_DLLEXP
	#endif
	#define sasfit_q_distribution_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_q_distribution_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_q_distribution_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_q_weibull__area_ ################ */
/**
 * \defgroup ff_q_weibull__area_ q-Weibull (Area)
 * \ingroup ff_peak_area_n_z
 *
 * \brief \<some brief description of q-Weibull (Area) function\>
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
 *       <td>\b q</td>
 *       <td>shape q<2</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>rate lambda>0</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>shape kappa>0</td>
 *      </tr><tr>
 *       <td>\b shift</td>
 *       <td>shift</td>
 *      </tr><tr>
 *       <td>\b y0</td>
 *       <td>background</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_q_weibull__area_
 *
 * \sa sasfit_q_distribution.h, ff_peak_area_n_z
 */
sasfit_q_distribution_DLLEXP scalar sasfit_ff_q_weibull__area_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_q_weibull__area_
 *
 * \sa sasfit_q_distribution.h, ff_peak_area_n_z
 */
sasfit_q_distribution_DLLEXP scalar sasfit_ff_q_weibull__area__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_q_weibull__area_
 *
 * \sa sasfit_q_distribution.h, ff_peak_area_n_z
 */
sasfit_q_distribution_DLLEXP scalar sasfit_ff_q_weibull__area__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_q_weibull__area_ ################ */

/* ################ start ff_q_weibull__ampl_ ################ */
/**
 * \defgroup ff_q_weibull__ampl_ q-Weibull (Ampl)
 * \ingroup ff_peak_amplitudes_n_z
 *
 * \brief \<some brief description of q-Weibull (Ampl) function\>
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
 *       <td>\b q</td>
 *       <td>shape q<2</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>rate lambda>0</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>shape kappa>0</td>
 *      </tr><tr>
 *       <td>\b shift</td>
 *       <td>shift</td>
 *      </tr><tr>
 *       <td>\b y0</td>
 *       <td>background</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_q_weibull__ampl_
 *
 * \sa sasfit_q_distribution.h, ff_peak_amplitudes_n_z
 */
sasfit_q_distribution_DLLEXP scalar sasfit_ff_q_weibull__ampl_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_q_weibull__ampl_
 *
 * \sa sasfit_q_distribution.h, ff_peak_amplitudes_n_z
 */
sasfit_q_distribution_DLLEXP scalar sasfit_ff_q_weibull__ampl__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_q_weibull__ampl_
 *
 * \sa sasfit_q_distribution.h, ff_peak_amplitudes_n_z
 */
sasfit_q_distribution_DLLEXP scalar sasfit_ff_q_weibull__ampl__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_q_weibull__ampl_ ################ */

/* ################ start ff_q_gaussian__area_ ################ */
/**
 * \defgroup ff_q_gaussian__area_ q-Gaussian (Area)
 * \ingroup ff_peak_area_n_z
 *
 * \brief \<some brief description of q-Gaussian (Area) function\>
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
 *       <td>\b q</td>
 *       <td>shape q<3</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>beta>0</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b shift</td>
 *       <td>shift</td>
 *      </tr><tr>
 *       <td>\b y0</td>
 *       <td>background</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_q_gaussian__area_
 *
 * \sa sasfit_q_distribution.h, ff_peak_area_n_z
 */
sasfit_q_distribution_DLLEXP scalar sasfit_ff_q_gaussian__area_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_q_gaussian__area_
 *
 * \sa sasfit_q_distribution.h, ff_peak_area_n_z
 */
sasfit_q_distribution_DLLEXP scalar sasfit_ff_q_gaussian__area__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_q_gaussian__area_
 *
 * \sa sasfit_q_distribution.h, ff_peak_area_n_z
 */
sasfit_q_distribution_DLLEXP scalar sasfit_ff_q_gaussian__area__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_q_gaussian__area_ ################ */

/* ################ start ff_q_gaussian__ampl_ ################ */
/**
 * \defgroup ff_q_gaussian__ampl_ q-Gaussian (Ampl)
 * \ingroup ff_peak_amplitudes_n_z
 *
 * \brief \<some brief description of q-Gaussian (Ampl) function\>
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
 *       <td>\b q</td>
 *       <td>shape q<3</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>beta>0</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b shift</td>
 *       <td>shift</td>
 *      </tr><tr>
 *       <td>\b y0</td>
 *       <td>background</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_q_gaussian__ampl_
 *
 * \sa sasfit_q_distribution.h, ff_peak_amplitudes_n_z
 */
sasfit_q_distribution_DLLEXP scalar sasfit_ff_q_gaussian__ampl_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_q_gaussian__ampl_
 *
 * \sa sasfit_q_distribution.h, ff_peak_amplitudes_n_z
 */
sasfit_q_distribution_DLLEXP scalar sasfit_ff_q_gaussian__ampl__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_q_gaussian__ampl_
 *
 * \sa sasfit_q_distribution.h, ff_peak_amplitudes_n_z
 */
sasfit_q_distribution_DLLEXP scalar sasfit_ff_q_gaussian__ampl__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_q_gaussian__ampl_ ################ */

/* ################ start ff_q_exponential__area_ ################ */
/**
 * \defgroup ff_q_exponential__area_ q-exponential (Area)
 * \ingroup ff_peak_area_n_z
 *
 * \brief \<some brief description of q-exponential (Area) function\>
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
 *       <td>\b q</td>
 *       <td>shape q<2</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>lambda>0</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b shift</td>
 *       <td>shift</td>
 *      </tr><tr>
 *       <td>\b y0</td>
 *       <td>background</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_q_exponential__area_
 *
 * \sa sasfit_q_distribution.h, ff_peak_area_n_z
 */
sasfit_q_distribution_DLLEXP scalar sasfit_ff_q_exponential__area_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_q_exponential__area_
 *
 * \sa sasfit_q_distribution.h, ff_peak_area_n_z
 */
sasfit_q_distribution_DLLEXP scalar sasfit_ff_q_exponential__area__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_q_exponential__area_
 *
 * \sa sasfit_q_distribution.h, ff_peak_area_n_z
 */
sasfit_q_distribution_DLLEXP scalar sasfit_ff_q_exponential__area__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_q_exponential__area_ ################ */

/* ################ start ff_q_exponential__ampl_ ################ */
/**
 * \defgroup ff_q_exponential__ampl_ q-exponential (Ampl)
 * \ingroup ff_peak_amplitudes_n_z
 *
 * \brief \<some brief description of q-exponential (Ampl) function\>
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
 *       <td>\b q</td>
 *       <td>shape q<2</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>lambda>0</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b shift</td>
 *       <td>shift</td>
 *      </tr><tr>
 *       <td>\b y0</td>
 *       <td>background</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_q_exponential__ampl_
 *
 * \sa sasfit_q_distribution.h, ff_peak_amplitudes_n_z
 */
sasfit_q_distribution_DLLEXP scalar sasfit_ff_q_exponential__ampl_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_q_exponential__ampl_
 *
 * \sa sasfit_q_distribution.h, ff_peak_amplitudes_n_z
 */
sasfit_q_distribution_DLLEXP scalar sasfit_ff_q_exponential__ampl__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_q_exponential__ampl_
 *
 * \sa sasfit_q_distribution.h, ff_peak_amplitudes_n_z
 */
sasfit_q_distribution_DLLEXP scalar sasfit_ff_q_exponential__ampl__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_q_exponential__ampl_ ################ */


#endif // this file

