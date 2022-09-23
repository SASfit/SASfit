/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_KAPPA_GENERALIZED_DISTRIBUTION_H
#define SASFIT_PLUGIN_KAPPA_GENERALIZED_DISTRIBUTION_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_kappa_generalized_distribution.h
 * Public available functions and descriptions of the kappa_generalized_distribution plugin.
 */

/**
 * \def sasfit_kappa_generalized_distribution_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_kappa_generalized_distribution_EXPORTS)
	#ifdef sasfit_kappa_generalized_distribution_DLLEXP
	#undef sasfit_kappa_generalized_distribution_DLLEXP
	#endif
	#define sasfit_kappa_generalized_distribution_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_kappa_generalized_distribution_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_kappa_generalized_distribution_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff___generalized_gamma_distribution__area_ ################ */
/**
 * \defgroup ff___generalized_gamma_distribution__area_ kappa-generalized gamma distribution (area)
 * \ingroup ff_peak_area_generalized
 *
 * \brief \<some brief description of kappa-generalized gamma distribution (area) function\>
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
 *       <td>\b sigma</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>shift parameter</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>kappa: 0 <= kappa < 1</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>alpha: shape parameter: alpha >0</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>beta: rate parameter: beta>0</td>
 *      </tr><tr>
 *       <td>\b nu</td>
 *       <td>nu: 0 < nu < 1/kappa</td>
 *      </tr></table>
 */

/**
 * \ingroup ff___generalized_gamma_distribution__area_
 *
 * \sa sasfit_kappa_generalized_distribution.h, ff_peak_area_generalized
 */
sasfit_kappa_generalized_distribution_DLLEXP scalar sasfit_ff___generalized_gamma_distribution__area_(scalar q, sasfit_param * p);

/**
 * \ingroup ff___generalized_gamma_distribution__area_
 *
 * \sa sasfit_kappa_generalized_distribution.h, ff_peak_area_generalized
 */
sasfit_kappa_generalized_distribution_DLLEXP scalar sasfit_ff___generalized_gamma_distribution__area__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff___generalized_gamma_distribution__area_
 *
 * \sa sasfit_kappa_generalized_distribution.h, ff_peak_area_generalized
 */
sasfit_kappa_generalized_distribution_DLLEXP scalar sasfit_ff___generalized_gamma_distribution__area__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff___generalized_gamma_distribution__area_ ################ */

/* ################ start ff___generalized_gamma_distribution__ampl_ ################ */
/**
 * \defgroup ff___generalized_gamma_distribution__ampl_ kappa-generalized gamma distribution (ampl)
 * \ingroup ff_peak_amplitudes_generalized
 *
 * \brief \<some brief description of kappa-generalized gamma distribution (ampl) function\>
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
 *       <td>\b sigma</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>shift parameter</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>kappa: 0 <= kappa < 1</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>alpha: shape parameter: alpha > 0</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>beta: rate parameter: beta>0</td>
 *      </tr><tr>
 *       <td>\b nu</td>
 *       <td>nu: 0 < nu < 1/kappa </td>
 *      </tr></table>
 */

/**
 * \ingroup ff___generalized_gamma_distribution__ampl_
 *
 * \sa sasfit_kappa_generalized_distribution.h, ff_peak_amplitudes_generalized
 */
sasfit_kappa_generalized_distribution_DLLEXP scalar sasfit_ff___generalized_gamma_distribution__ampl_(scalar q, sasfit_param * p);

/**
 * \ingroup ff___generalized_gamma_distribution__ampl_
 *
 * \sa sasfit_kappa_generalized_distribution.h, ff_peak_amplitudes_generalized
 */
sasfit_kappa_generalized_distribution_DLLEXP scalar sasfit_ff___generalized_gamma_distribution__ampl__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff___generalized_gamma_distribution__ampl_
 *
 * \sa sasfit_kappa_generalized_distribution.h, ff_peak_amplitudes_generalized
 */
sasfit_kappa_generalized_distribution_DLLEXP scalar sasfit_ff___generalized_gamma_distribution__ampl__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff___generalized_gamma_distribution__ampl_ ################ */


#endif // this file

