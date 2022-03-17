/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_GENERALIZED_JOHNSON_DISTRIBUTION_H
#define SASFIT_PLUGIN_GENERALIZED_JOHNSON_DISTRIBUTION_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_generalized_johnson_distribution.h
 * Public available functions and descriptions of the generalized_johnson_distribution plugin.
 */

/**
 * \def sasfit_generalized_johnson_distribution_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_generalized_johnson_distribution_EXPORTS)
	#ifdef sasfit_generalized_johnson_distribution_DLLEXP
	#undef sasfit_generalized_johnson_distribution_DLLEXP
	#endif
	#define sasfit_generalized_johnson_distribution_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_generalized_johnson_distribution_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_generalized_johnson_distribution_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sd_g_n_pdf ################ */
/**
 * \defgroup sd_g_n_pdf G_N J_PDF
 * \ingroup sd_plugins_quantile_unbounded
 *
 * \brief \<some brief description of G_N J_PDF function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>number density</td>
 *      </tr><tr>
 *       <td>\b l</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b u</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>shape parameter</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_g_n_pdf
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_unbounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_n_pdf(scalar q, sasfit_param * p);

/**
 * \ingroup sd_g_n_pdf
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_unbounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_n_pdf_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_g_n_pdf
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_unbounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_n_pdf_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_g_n_pdf ################ */

/* ################ start sd_g_l_pdf ################ */
/**
 * \defgroup sd_g_l_pdf G_L J_PDF
 * \ingroup sd_plugins_quantile_unbounded
 *
 * \brief \<some brief description of G_L J_PDF function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>number density</td>
 *      </tr><tr>
 *       <td>\b l</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b u</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>shape parameter</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_g_l_pdf
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_unbounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_l_pdf(scalar q, sasfit_param * p);

/**
 * \ingroup sd_g_l_pdf
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_unbounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_l_pdf_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_g_l_pdf
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_unbounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_l_pdf_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_g_l_pdf ################ */

/* ################ start sd_g_c_pdf ################ */
/**
 * \defgroup sd_g_c_pdf G_C J_PDF
 * \ingroup sd_plugins_quantile_unbounded
 *
 * \brief \<some brief description of G_C J_PDF function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>number densitynumber density</td>
 *      </tr><tr>
 *       <td>\b l</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b u</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>shape parameter</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_g_c_pdf
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_unbounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_c_pdf(scalar q, sasfit_param * p);

/**
 * \ingroup sd_g_c_pdf
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_unbounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_c_pdf_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_g_c_pdf
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_unbounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_c_pdf_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_g_c_pdf ################ */

/* ################ start sd_g_n_pdf_s ################ */
/**
 * \defgroup sd_g_n_pdf_s G_N J_PDF_S
 * \ingroup sd_plugins_quantile_semibounded
 *
 * \brief \<some brief description of G_N J_PDF_S function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>number density</td>
 *      </tr><tr>
 *       <td>\b l</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b u</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>shape parameter</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_g_n_pdf_s
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_semibounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_n_pdf_s(scalar q, sasfit_param * p);

/**
 * \ingroup sd_g_n_pdf_s
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_semibounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_n_pdf_s_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_g_n_pdf_s
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_semibounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_n_pdf_s_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_g_n_pdf_s ################ */

/* ################ start sd_g_l_pdf_s ################ */
/**
 * \defgroup sd_g_l_pdf_s G_L J_PDF_S
 * \ingroup sd_plugins_quantile_semibounded
 *
 * \brief \<some brief description of G_L J_PDF_S function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>number density</td>
 *      </tr><tr>
 *       <td>\b l</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b u</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>shape parameter</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_g_l_pdf_s
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_semibounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_l_pdf_s(scalar q, sasfit_param * p);

/**
 * \ingroup sd_g_l_pdf_s
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_semibounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_l_pdf_s_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_g_l_pdf_s
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_semibounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_l_pdf_s_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_g_l_pdf_s ################ */

/* ################ start sd_g_c_pdf_s ################ */
/**
 * \defgroup sd_g_c_pdf_s G_C PDF_S
 * \ingroup sd_plugins_quantile_semibounded
 *
 * \brief \<some brief description of G_C PDF_S function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>number density</td>
 *      </tr><tr>
 *       <td>\b l</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b u</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>shape parameter</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_g_c_pdf_s
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_semibounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_c_pdf_s(scalar q, sasfit_param * p);

/**
 * \ingroup sd_g_c_pdf_s
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_semibounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_c_pdf_s_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_g_c_pdf_s
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_semibounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_c_pdf_s_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_g_c_pdf_s ################ */

/* ################ start sd_g_n_pdf_b ################ */
/**
 * \defgroup sd_g_n_pdf_b G_N PDF_B
 * \ingroup sd_plugins_quantile_bounded
 *
 * \brief \<some brief description of G_N PDF_B function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>number density</td>
 *      </tr><tr>
 *       <td>\b l</td>
 *       <td>lower boound</td>
 *      </tr><tr>
 *       <td>\b u</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>shape parameter</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_g_n_pdf_b
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_bounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_n_pdf_b(scalar q, sasfit_param * p);

/**
 * \ingroup sd_g_n_pdf_b
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_bounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_n_pdf_b_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_g_n_pdf_b
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_bounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_n_pdf_b_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_g_n_pdf_b ################ */

/* ################ start sd_g_l_pdf_b ################ */
/**
 * \defgroup sd_g_l_pdf_b G_L PDF_B
 * \ingroup sd_plugins_quantile_bounded
 *
 * \brief \<some brief description of G_L PDF_B function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>number density</td>
 *      </tr><tr>
 *       <td>\b l</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b u</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>shape parameter</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_g_l_pdf_b
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_bounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_l_pdf_b(scalar q, sasfit_param * p);

/**
 * \ingroup sd_g_l_pdf_b
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_bounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_l_pdf_b_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_g_l_pdf_b
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_bounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_l_pdf_b_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_g_l_pdf_b ################ */

/* ################ start sd_g_c_pdf_b ################ */
/**
 * \defgroup sd_g_c_pdf_b G_C PDF_B
 * \ingroup sd_plugins_quantile_bounded
 *
 * \brief \<some brief description of G_C PDF_B function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>number density</td>
 *      </tr><tr>
 *       <td>\b l</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b u</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>shape parameter</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_g_c_pdf_b
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_bounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_c_pdf_b(scalar q, sasfit_param * p);

/**
 * \ingroup sd_g_c_pdf_b
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_bounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_c_pdf_b_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_g_c_pdf_b
 *
 * \sa sasfit_generalized_johnson_distribution.h, sd_plugins_quantile_bounded
 */
sasfit_generalized_johnson_distribution_DLLEXP scalar sasfit_sd_g_c_pdf_b_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_g_c_pdf_b ################ */


#endif // this file

