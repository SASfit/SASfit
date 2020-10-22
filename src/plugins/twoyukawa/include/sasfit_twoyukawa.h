/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_TWOYUKAWA_H
#define SASFIT_PLUGIN_TWOYUKAWA_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_twoyukawa.h
 * Public available functions and descriptions of the twoyukawa plugin.
 */

/**
 * \def sasfit_twoyukawa_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_twoyukawa_EXPORTS)
	#ifdef sasfit_twoyukawa_DLLEXP
	#undef sasfit_twoyukawa_DLLEXP
	#endif
	#define sasfit_twoyukawa_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_twoyukawa_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_twoyukawa_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq___yukawa ################ */
/** 
 * \defgroup sq___yukawa 2-Yukawa
 * \ingroup sq_plugins_devel
 *
 * \brief \<some brief description of 2-Yukawa function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius</td>
 *      </tr><tr>
 *       <td>\b K1</td>
 *       <td>Potential strength in units kT, K>1 attraction, K<1 repulsion</td>
 *      </tr><tr>
 *       <td>\b lambda1</td>
 *       <td>Screening length in nm. The inverse screening length is Z1=1/lambda1</td>
 *      </tr><tr>
 *       <td>\b K2</td>
 *       <td>Potential strength in units kT, K>1 attraction, K<1 repulsion</td>
 *      </tr><tr>
 *       <td>\b lambda2</td>
 *       <td>Screening length in nm. The inverse screening length is Z2=1/lambda2</td>
 *      </tr><tr>
 *       <td>\b phi</td>
 *       <td>volum fraction</td>
 *      </tr><tr>
 *       <td>\b molarity</td>
 *       <td>concentration in units mol/l. Overrides phi if lareger than 0</td>
 *      </tr></table>
 */

/**
 * \ingroup sq___yukawa
 *
 * \sa sasfit_twoyukawa.h, sq_plugins_devel
 */
sasfit_twoyukawa_DLLEXP scalar sasfit_sq___yukawa(scalar q, sasfit_param * p);

/**
 * \ingroup sq___yukawa
 *
 * \sa sasfit_twoyukawa.h, sq_plugins_devel
 */
sasfit_twoyukawa_DLLEXP scalar sasfit_sq___yukawa_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq___yukawa
 *
 * \sa sasfit_twoyukawa.h, sq_plugins_devel
 */
sasfit_twoyukawa_DLLEXP scalar sasfit_sq___yukawa_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq___yukawa ################ */


#endif // this file

