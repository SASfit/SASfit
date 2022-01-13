/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_LOGNORM_FP_H
#define SASFIT_PLUGIN_LOGNORM_FP_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_lognorm_fp.h
 * Public available functions and descriptions of the lognorm_fp plugin.
 */

/**
 * \def sasfit_lognorm_fp_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_lognorm_fp_EXPORTS)
	#ifdef sasfit_lognorm_fp_DLLEXP
	#undef sasfit_lognorm_fp_DLLEXP
	#endif
	#define sasfit_lognorm_fp_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_lognorm_fp_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_lognorm_fp_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sd_std_lognorm ################ */
/** 
 * \defgroup sd_std_lognorm std LogNorm
 * \ingroup sd_plugins_quantile_semibounded
 *
 * \brief \<some brief description of std LogNorm function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>number density</td>
 *      </tr><tr>
 *       <td>\b s</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td>shape parameter  (p=1, standard value)</td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td>location parameter</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_std_lognorm
 *
 * \sa sasfit_lognorm_fp.h, sd_plugins_quantile_semibounded
 */
sasfit_lognorm_fp_DLLEXP scalar sasfit_sd_std_lognorm(scalar q, sasfit_param * p);

/**
 * \ingroup sd_std_lognorm
 *
 * \sa sasfit_lognorm_fp.h, sd_plugins_quantile_semibounded
 */
sasfit_lognorm_fp_DLLEXP scalar sasfit_sd_std_lognorm_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_std_lognorm
 *
 * \sa sasfit_lognorm_fp.h, sd_plugins_quantile_semibounded
 */
sasfit_lognorm_fp_DLLEXP scalar sasfit_sd_std_lognorm_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_std_lognorm ################ */

/* ################ start sd_shifted_lognorm ################ */
/** 
 * \defgroup sd_shifted_lognorm shifted LogNorm
 * \ingroup sd_plugins_quantile_semibounded
 *
 * \brief \<some brief description of shifted LogNorm function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>number density</td>
 *      </tr><tr>
 *       <td>\b s</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b x0</td>
 *       <td>shift of distrivbution</td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td>location parameter</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_shifted_lognorm
 *
 * \sa sasfit_lognorm_fp.h, sd_plugins_quantile_semibounded
 */
sasfit_lognorm_fp_DLLEXP scalar sasfit_sd_shifted_lognorm(scalar q, sasfit_param * p);

/**
 * \ingroup sd_shifted_lognorm
 *
 * \sa sasfit_lognorm_fp.h, sd_plugins_quantile_semibounded
 */
sasfit_lognorm_fp_DLLEXP scalar sasfit_sd_shifted_lognorm_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_shifted_lognorm
 *
 * \sa sasfit_lognorm_fp.h, sd_plugins_quantile_semibounded
 */
sasfit_lognorm_fp_DLLEXP scalar sasfit_sd_shifted_lognorm_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_shifted_lognorm ################ */

/* ################ start sd_lognorm_fp ################ */
/** 
 * \defgroup sd_lognorm_fp LogNorm_fp
 * \ingroup sd_plugins_quantile_semibounded
 *
 * \brief \<some brief description of LogNorm_fp function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b fp</td>
 *       <td>volume fraction</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width parameter</td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td>location parameter</td>
 *      </tr><tr>
 *       <td>\b length2</td>
 *       <td>second length parameter</td>
 *      </tr><tr>
 *       <td>\b length3</td>
 *       <td>third length parameter</td>
 *      </tr><tr>
 *       <td>\b shape</td>
 *       <td>shape parameter(1:sphere (dR), 2: cylinder (dR), 3: cylinder (dL), 4: sph. shell (dR), 5: sph. shell (dDR))</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_lognorm_fp
 *
 * \sa sasfit_lognorm_fp.h, sd_plugins_quantile_semibounded
 */
sasfit_lognorm_fp_DLLEXP scalar sasfit_sd_lognorm_fp(scalar q, sasfit_param * p);

/**
 * \ingroup sd_lognorm_fp
 *
 * \sa sasfit_lognorm_fp.h, sd_plugins_quantile_semibounded
 */
sasfit_lognorm_fp_DLLEXP scalar sasfit_sd_lognorm_fp_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_lognorm_fp
 *
 * \sa sasfit_lognorm_fp.h, sd_plugins_quantile_semibounded
 */
sasfit_lognorm_fp_DLLEXP scalar sasfit_sd_lognorm_fp_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_lognorm_fp ################ */

/* ################ start sd_bilognorm ################ */
/** 
 * \defgroup sd_bilognorm BiLogNorm
 * \ingroup sd_plugins_quantile_semibounded
 *
 * \brief \<some brief description of BiLogNorm function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N_1</td>
 *       <td>number density of first population</td>
 *      </tr><tr>
 *       <td>\b s_1</td>
 *       <td>first width parameter</td>
 *      </tr><tr>
 *       <td>\b p_1</td>
 *       <td>first shape parameter</td>
 *      </tr><tr>
 *       <td>\b mu_1</td>
 *       <td>first locatiopn parameter</td>
 *      </tr><tr>
 *       <td>\b N_2</td>
 *       <td>number density of second population</td>
 *      </tr><tr>
 *       <td>\b s_2</td>
 *       <td>second width parameter</td>
 *      </tr><tr>
 *       <td>\b p_2</td>
 *       <td>second shape parameter</td>
 *      </tr><tr>
 *       <td>\b mu_2</td>
 *       <td>second location parameter</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_bilognorm
 *
 * \sa sasfit_lognorm_fp.h, sd_plugins_quantile_semibounded
 */
sasfit_lognorm_fp_DLLEXP scalar sasfit_sd_bilognorm(scalar q, sasfit_param * p);

/**
 * \ingroup sd_bilognorm
 *
 * \sa sasfit_lognorm_fp.h, sd_plugins_quantile_semibounded
 */
sasfit_lognorm_fp_DLLEXP scalar sasfit_sd_bilognorm_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_bilognorm
 *
 * \sa sasfit_lognorm_fp.h, sd_plugins_quantile_semibounded
 */
sasfit_lognorm_fp_DLLEXP scalar sasfit_sd_bilognorm_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_bilognorm ################ */


#endif // this file

