/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_GENERALIZED_DISTRIBUTIONS_H
#define SASFIT_PLUGIN_GENERALIZED_DISTRIBUTIONS_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_generalized_distributions.h
 * Public available functions and descriptions of the generalized_distributions plugin.
 */

/**
 * \def sasfit_generalized_distributions_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_generalized_distributions_EXPORTS)
	#ifdef sasfit_generalized_distributions_DLLEXP
	#undef sasfit_generalized_distributions_DLLEXP
	#endif
	#define sasfit_generalized_distributions_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_generalized_distributions_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_generalized_distributions_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_generalized_hyperbolic__ampl_ ################ */
/**
 * \defgroup ff_generalized_hyperbolic__ampl_ generalized hyperbolic (Ampl)
 * \ingroup ff_peak_amplitudes_generalized
 *
 * \brief \<some brief description of generalized hyperbolic (Ampl) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_generalized_hyperbolic__ampl_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_amplitudes_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_generalized_hyperbolic__ampl_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_hyperbolic__ampl_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_amplitudes_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_generalized_hyperbolic__ampl__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_hyperbolic__ampl_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_amplitudes_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_generalized_hyperbolic__ampl__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_generalized_hyperbolic__ampl_ ################ */

/* ################ start ff_generalized_hyperbolic__area_ ################ */
/**
 * \defgroup ff_generalized_hyperbolic__area_ generalized hyperbolic (Area)
 * \ingroup ff_peak_area_generalized
 *
 * \brief \<some brief description of generalized hyperbolic (Area) function\>
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
 *       <td>\b lambda</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b y0</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_generalized_hyperbolic__area_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_area_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_generalized_hyperbolic__area_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_hyperbolic__area_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_area_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_generalized_hyperbolic__area__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_hyperbolic__area_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_area_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_generalized_hyperbolic__area__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_generalized_hyperbolic__area_ ################ */

/* ################ start ff_generalized_extreme_value__ampl_ ################ */
/**
 * \defgroup ff_generalized_extreme_value__ampl_ generalized extreme value (Ampl)
 * \ingroup ff_peak_amplitudes_generalized
 *
 * \brief \<some brief description of generalized extreme value (Ampl) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_generalized_extreme_value__ampl_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_amplitudes_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_generalized_extreme_value__ampl_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_extreme_value__ampl_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_amplitudes_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_generalized_extreme_value__ampl__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_extreme_value__ampl_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_amplitudes_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_generalized_extreme_value__ampl__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_generalized_extreme_value__ampl_ ################ */

/* ################ start ff_generalized_extreme_value__area_ ################ */
/**
 * \defgroup ff_generalized_extreme_value__area_ generalized extreme value (Area)
 * \ingroup ff_peak_area_generalized
 *
 * \brief \<some brief description of generalized extreme value (Area) function\>
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
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b y0</td>
 *       <td>background</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_generalized_extreme_value__area_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_area_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_generalized_extreme_value__area_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_extreme_value__area_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_area_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_generalized_extreme_value__area__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_extreme_value__area_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_area_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_generalized_extreme_value__area__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_generalized_extreme_value__area_ ################ */

/* ################ start ff_exponentiated_generalized_pareto__ampl_ ################ */
/**
 * \defgroup ff_exponentiated_generalized_pareto__ampl_ exponentiated generalized Pareto (Ampl)
 * \ingroup ff_peak_amplitudes_generalized
 *
 * \brief \<some brief description of exponentiated generalized Pareto (Ampl) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_exponentiated_generalized_pareto__ampl_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_amplitudes_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_exponentiated_generalized_pareto__ampl_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_exponentiated_generalized_pareto__ampl_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_amplitudes_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_exponentiated_generalized_pareto__ampl__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_exponentiated_generalized_pareto__ampl_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_amplitudes_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_exponentiated_generalized_pareto__ampl__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_exponentiated_generalized_pareto__ampl_ ################ */

/* ################ start ff_exponentiated_generalized_pareto__area_ ################ */
/**
 * \defgroup ff_exponentiated_generalized_pareto__area_ exponentiated generalized Pareto (Area)
 * \ingroup ff_peak_area_generalized
 *
 * \brief \<some brief description of exponentiated generalized Pareto (Area) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>log(xi) is location parameter</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b y0</td>
 *       <td>background</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_exponentiated_generalized_pareto__area_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_area_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_exponentiated_generalized_pareto__area_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_exponentiated_generalized_pareto__area_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_area_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_exponentiated_generalized_pareto__area__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_exponentiated_generalized_pareto__area_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_area_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_exponentiated_generalized_pareto__area__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_exponentiated_generalized_pareto__area_ ################ */

/* ################ start ff_generalized_normal__ampl_ ################ */
/**
 * \defgroup ff_generalized_normal__ampl_ generalized normal (Ampl)
 * \ingroup ff_peak_amplitudes_generalized
 *
 * \brief \<some brief description of generalized normal (Ampl) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_generalized_normal__ampl_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_amplitudes_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_generalized_normal__ampl_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_normal__ampl_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_amplitudes_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_generalized_normal__ampl__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_normal__ampl_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_amplitudes_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_generalized_normal__ampl__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_generalized_normal__ampl_ ################ */

/* ################ start ff_generalized_normal__area_ ################ */
/**
 * \defgroup ff_generalized_normal__area_ generalized normal (Area)
 * \ingroup ff_peak_area_generalized
 *
 * \brief \<some brief description of generalized normal (Area) function\>
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
 *       <td>scale parameter (positive)</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b y0</td>
 *       <td>background</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_generalized_normal__area_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_area_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_generalized_normal__area_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_normal__area_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_area_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_generalized_normal__area__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_normal__area_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_area_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_generalized_normal__area__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_generalized_normal__area_ ################ */

/* ################ start ff_generalized_inverse_gaussian__ampl_ ################ */
/**
 * \defgroup ff_generalized_inverse_gaussian__ampl_ generalized inverse Gaussian (Ampl)
 * \ingroup ff_peak_amplitudes_generalized
 *
 * \brief \<some brief description of generalized inverse Gaussian (Ampl) function\>
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
 *       <td>scale parameter (positive)</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b y0</td>
 *       <td>background</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_generalized_inverse_gaussian__ampl_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_amplitudes_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_generalized_inverse_gaussian__ampl_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_inverse_gaussian__ampl_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_amplitudes_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_generalized_inverse_gaussian__ampl__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_inverse_gaussian__ampl_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_amplitudes_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_generalized_inverse_gaussian__ampl__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_generalized_inverse_gaussian__ampl_ ################ */

/* ################ start ff_generalized_inverse_gaussian__area_ ################ */
/**
 * \defgroup ff_generalized_inverse_gaussian__area_ generalized inverse Gaussian (Area)
 * \ingroup ff_peak_area_generalized
 *
 * \brief \<some brief description of generalized inverse Gaussian (Area) function\>
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
 *       <td>\b theta</td>
 *       <td>concentration parameter (positive)</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scaling parameter (positive)</td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b y0</td>
 *       <td>background</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_generalized_inverse_gaussian__area_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_area_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_generalized_inverse_gaussian__area_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_inverse_gaussian__area_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_area_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_generalized_inverse_gaussian__area__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_inverse_gaussian__area_
 *
 * \sa sasfit_generalized_distributions.h, ff_peak_area_generalized
 */
sasfit_generalized_distributions_DLLEXP scalar sasfit_ff_generalized_inverse_gaussian__area__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_generalized_inverse_gaussian__area_ ################ */

#endif // this file

