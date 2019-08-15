/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_GENERALIZED_FORMFACTOR_H
#define SASFIT_PLUGIN_GENERALIZED_FORMFACTOR_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_generalized_formfactor.h
 * Public available functions and descriptions of the generalized_formfactor plugin.
 */

/**
 * \def sasfit_generalized_formfactor_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_generalized_formfactor_EXPORTS)
	#ifdef sasfit_generalized_formfactor_DLLEXP
	#undef sasfit_generalized_formfactor_DLLEXP
	#endif
	#define sasfit_generalized_formfactor_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_generalized_formfactor_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_generalized_formfactor_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_generalized_form_factor ################ */
/**
 * \defgroup ff_generalized_form_factor generalized form factor
 * \ingroup ff_devel
 *
 * \brief \<some brief description of generalized form factor function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>size parameter</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>width of the size distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td> dimensionality parameter alpha related to a</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>decay at large scattering vectors, q^p, with p=gamma+beta+1</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>additional potential law q^gamma</td>
 *      </tr><tr>
 *       <td>\b nu</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_generalized_form_factor
 *
 * \sa sasfit_generalized_formfactor.h, ff_devel
 */
sasfit_generalized_formfactor_DLLEXP scalar sasfit_ff_generalized_form_factor(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_form_factor
 *
 * \sa sasfit_generalized_formfactor.h, ff_devel
 */
sasfit_generalized_formfactor_DLLEXP scalar sasfit_ff_generalized_form_factor_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_form_factor
 *
 * \sa sasfit_generalized_formfactor.h, ff_devel
 */
sasfit_generalized_formfactor_DLLEXP scalar sasfit_ff_generalized_form_factor_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_generalized_form_factor ################ */

/* ################ start ff_lesser_generalized_form_factor ################ */
/**
 * \defgroup ff_lesser_generalized_form_factor lesser generalized form factor 1
 * \ingroup ff_devel
 *
 * \brief \<some brief description of lesser generalized form factor 1 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>size parameter</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>width of the size distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dim</td>
 *       <td>overall dimension of the particle (dim<=3)</td>
 *      </tr><tr>
 *       <td>\b s</td>
 *       <td>potential law at small q-values</td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td>potential law at large q-values</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_lesser_generalized_form_factor
 *
 * \sa sasfit_generalized_formfactor.h, ff_devel
 */
sasfit_generalized_formfactor_DLLEXP scalar sasfit_ff_lesser_generalized_form_factor(scalar q, sasfit_param * p);

/**
 * \ingroup ff_lesser_generalized_form_factor
 *
 * \sa sasfit_generalized_formfactor.h, ff_devel
 */
sasfit_generalized_formfactor_DLLEXP scalar sasfit_ff_lesser_generalized_form_factor_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_lesser_generalized_form_factor
 *
 * \sa sasfit_generalized_formfactor.h, ff_devel
 */
sasfit_generalized_formfactor_DLLEXP scalar sasfit_ff_lesser_generalized_form_factor_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_lesser_generalized_form_factor ################ */


/* ################ start ff_lesser_generalized_form_factor2 ################ */
/**
 * \defgroup ff_lesser_generalized_form_factor2 lesser generalized form factor 2
 * \ingroup ff_devel
 *
 * \brief \<some brief description of lesser generalized form factor 2 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>size parameter</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>width of the size distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td> dimensionality parameter alpha related to a</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b s</td>
 *       <td>potential law at small q-values</td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td>potential law at large q-values</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_lesser_generalized_form_factor2
 *
 * \sa sasfit_generalized_formfactor.h, ff_devel
 */
sasfit_generalized_formfactor_DLLEXP scalar sasfit_ff_lesser_generalized_form_factor2(scalar q, sasfit_param * p);

/**
 * \ingroup ff_lesser_generalized_form_factor2
 *
 * \sa sasfit_generalized_formfactor.h, ff_devel
 */
sasfit_generalized_formfactor_DLLEXP scalar sasfit_ff_lesser_generalized_form_factor2_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_lesser_generalized_form_factor2
 *
 * \sa sasfit_generalized_formfactor.h, ff_devel
 */
sasfit_generalized_formfactor_DLLEXP scalar sasfit_ff_lesser_generalized_form_factor2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_lesser_generalized_form_factor2 ################ */
#endif // this file

