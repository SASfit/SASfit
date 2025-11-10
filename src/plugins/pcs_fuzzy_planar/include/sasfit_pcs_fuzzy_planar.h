/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_PCS_FUZZY_PLANAR_H
#define SASFIT_PLUGIN_PCS_FUZZY_PLANAR_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_pcs_fuzzy_planar.h
 * Public available functions and descriptions of the pcs_fuzzy_planar plugin.
 */

/**
 * \def sasfit_pcs_fuzzy_planar_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_pcs_fuzzy_planar_EXPORTS)
	#ifdef sasfit_pcs_fuzzy_planar_DLLEXP
	#undef sasfit_pcs_fuzzy_planar_DLLEXP
	#endif
	#define sasfit_pcs_fuzzy_planar_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_pcs_fuzzy_planar_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_pcs_fuzzy_planar_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_boucher_planar ################ */
/**
 * \defgroup ff_boucher_planar Pcs:BoucherPlanar
 * \ingroup ff_plugins_pcslocalplanar
 *
 * \brief see SASfit manual to read more about Pcs:BoucherPlanar
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b t</td>
 *       <td>thickness</td>
 *      </tr><tr>
 *       <td>\b sigma_t</td>
 *       <td>lognorm width of t</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>Boucher exponent</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Deta</td>
 *       <td>scattering contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_boucher_planar
 *
 * \sa sasfit_pcs_fuzzy_planar.h, ff_plugins_pcslocalplanar
 */
sasfit_pcs_fuzzy_planar_DLLEXP scalar sasfit_ff_boucher_planar(scalar q, sasfit_param * p);

/**
 * \ingroup ff_boucher_planar
 *
 * \sa sasfit_pcs_fuzzy_planar.h, ff_plugins_pcslocalplanar
 */
sasfit_pcs_fuzzy_planar_DLLEXP scalar sasfit_ff_boucher_planar_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_boucher_planar
 *
 * \sa sasfit_pcs_fuzzy_planar.h, ff_plugins_pcslocalplanar
 */
sasfit_pcs_fuzzy_planar_DLLEXP scalar sasfit_ff_boucher_planar_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_boucher_planar ################ */

/* ################ start ff_profile_boucher_planar ################ */
/**
 * \defgroup ff_profile_boucher_planar profile_boucher_planar
 * \ingroup ff_plugins_pcslocalplanar
 *
 * \brief \<some brief description of profile_boucher_planar function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b T</td>
 *       <td>thickness</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>Boucher exponent</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b Deta</td>
 *       <td>scattering contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_profile_boucher_planar
 *
 * \sa sasfit_pcs_fuzzy_planar.h, ff_plugins_pcslocalplanar
 */
sasfit_pcs_fuzzy_planar_DLLEXP scalar sasfit_ff_profile_boucher_planar(scalar q, sasfit_param * p);

/**
 * \ingroup ff_profile_boucher_planar
 *
 * \sa sasfit_pcs_fuzzy_planar.h, ff_plugins_pcslocalplanar
 */
sasfit_pcs_fuzzy_planar_DLLEXP scalar sasfit_ff_profile_boucher_planar_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_profile_boucher_planar
 *
 * \sa sasfit_pcs_fuzzy_planar.h, ff_plugins_pcslocalplanar
 */
sasfit_pcs_fuzzy_planar_DLLEXP scalar sasfit_ff_profile_boucher_planar_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_profile_boucher_planar ################ */


#endif // this file

