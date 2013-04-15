/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_GENERALIZED_GUINIER_POROD_H
#define SASFIT_PLUGIN_GENERALIZED_GUINIER_POROD_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_generalized_guinier_porod.h
 * Public available functions and descriptions of the generalized_guinier_porod plugin.
 */

/**
 * \def sasfit_generalized_guinier_porod_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_generalized_guinier_porod_EXPORTS)
	#ifdef sasfit_generalized_guinier_porod_DLLEXP
	#undef sasfit_generalized_guinier_porod_DLLEXP
	#endif
	#define sasfit_generalized_guinier_porod_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_generalized_guinier_porod_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_generalized_guinier_porod_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_generalized_guinier_prod_law ################ */
/**
 * \defgroup ff_generalized_guinier_prod_law Generalized-Guinier-Prod-Law
 * \ingroup ff_plugins
 *
 * \brief \<some brief description of Generalized-Guinier-Prod-Law function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b G2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b s2</td>
 *       <td>s2: potential law for small q-region\ndimensionality parameter: (3-s2)</td>
 *      </tr><tr>
 *       <td>\b Rg2</td>
 *       <td>radius of gyration</td>
 *      </tr><tr>
 *       <td>\b s1</td>
 *       <td>s2: potential law in intermediate q-range\ndimensionality parameter (3-s1)</td>
 *      </tr><tr>
 *       <td>\b Rg1</td>
 *       <td>radius of gyration</td>
 *      </tr><tr>
 *       <td>\b m</td>
 *       <td>m: potential law at large q-values</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_generalized_guinier_prod_law
 *
 * \sa sasfit_generalized_guinier_porod.h, ff_plugins
 */
sasfit_generalized_guinier_porod_DLLEXP scalar sasfit_ff_generalized_guinier_prod_law(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_guinier_prod_law
 *
 * \sa sasfit_generalized_guinier_porod.h, ff_plugins
 */
sasfit_generalized_guinier_porod_DLLEXP scalar sasfit_ff_generalized_guinier_prod_law_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_guinier_prod_law
 *
 * \sa sasfit_generalized_guinier_porod.h, ff_plugins
 */
sasfit_generalized_guinier_porod_DLLEXP scalar sasfit_ff_generalized_guinier_prod_law_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_generalized_guinier_prod_law ################ */


#endif // this file

