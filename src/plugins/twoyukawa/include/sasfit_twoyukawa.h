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

/* ################ start sq_two_yukawa ################ */
/**
 * \defgroup sq_two_yukawa 2-Yukawa
 * \ingroup sq_plugins_yukawa
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
 *       <td>concentration in units mol/l. Overrides phi if it larger than 0</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_two_yukawa
 *
 * \sa sasfit_twoyukawa.h, sq_plugins_yukawa
 */
sasfit_twoyukawa_DLLEXP scalar sasfit_sq_two_yukawa(scalar q, sasfit_param * p);

/**
 * \ingroup sq_two_yukawa
 *
 * \sa sasfit_twoyukawa.h, sq_plugins_yukawa
 */
sasfit_twoyukawa_DLLEXP scalar sasfit_sq_two_yukawa_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_two_yukawa
 *
 * \sa sasfit_twoyukawa.h, sq_plugins_yukawa
 */
sasfit_twoyukawa_DLLEXP scalar sasfit_sq_two_yukawa_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_two_yukawa ################ */


/* ################ start sq_two_yukawa_potential ################ */
/**
 * \defgroup sq_two_yukawa_potential 2-Yukawa potential
 * \ingroup sq_plugins_yukawa
 *
 * \brief \<some brief description of 2-Yukawa potential function\>
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
 *       <td>concentration in units mol/l. Overrides phi if it larger than 0</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_two_yukawa_potential
 *
 * \sa sasfit_twoyukawa.h, sq_plugins_yukawa
 */
sasfit_twoyukawa_DLLEXP scalar sasfit_sq_two_yukawa_potential(scalar q, sasfit_param * p);

/**
 * \ingroup sq_two_yukawa_potential
 *
 * \sa sasfit_twoyukawa.h, sq_plugins_yukawa
 */
sasfit_twoyukawa_DLLEXP scalar sasfit_sq_two_yukawa_potential_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_two_yukawa_potential
 *
 * \sa sasfit_twoyukawa.h, sq_plugins_yukawa
 */
sasfit_twoyukawa_DLLEXP scalar sasfit_sq_two_yukawa_potential_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_two_yukawa_potential ################ */

/* ################ start sq_two_yukawa_gr ################ */
/**
 * \defgroup sq_two_yukawa_gr 2-Yukawa g(r)
 * \ingroup sq_plugins_yukawa
 *
 * \brief \<some brief description of 2-Yukawa g(r) function\>
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
 *       <td>concentration in units mol/l. Overrides phi if it larger than 0</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_two_yukawa_gr
 *
 * \sa sasfit_twoyukawa.h, sq_plugins_yukawa
 */
sasfit_twoyukawa_DLLEXP scalar sasfit_sq_two_yukawa_gr(scalar q, sasfit_param * p);

/**
 * \ingroup sq_two_yukawa_gr
 *
 * \sa sasfit_twoyukawa.h, sq_plugins_yukawa
 */
sasfit_twoyukawa_DLLEXP scalar sasfit_sq_two_yukawa_gr_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_two_yukawa_gr
 *
 * \sa sasfit_twoyukawa.h, sq_plugins_yukawa
 */
sasfit_twoyukawa_DLLEXP scalar sasfit_sq_two_yukawa_gr_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_two_yukawa_gr ################ */

/* ################ start sq_one_yukawa ################ */
/**
 * \defgroup sq_one_yukawa 1-Yukawa
 * \ingroup sq_plugins_yukawa
 *
 * \brief \<some brief description of 1-Yukawa function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius</td>
 *      </tr><tr>
 *       <td>\b K</td>
 *       <td>Potential strength in units kT, K>1 attraction, K<1 repulsion</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>Screening length in nm. The inverse screening length is Z=1/lambda</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy/td>
 *      </tr><tr>
 *       <td>\b phi</td>
 *       <td>volum fraction</td>
 *      </tr><tr>
 *       <td>\b molarity</td>
 *       <td>concentration in units mol/l. Overrides phi if it larger than 0</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_one_yukawa
 *
 * \sa sasfit_twoyukawa.h, sq_plugins_yukawa
 */
sasfit_twoyukawa_DLLEXP scalar sasfit_sq_one_yukawa(scalar q, sasfit_param * p);

/**
 * \ingroup sq_one_yukawa
 *
 * \sa sasfit_twoyukawa.h, sq_plugins_yukawa
 */
sasfit_twoyukawa_DLLEXP scalar sasfit_sq_one_yukawa_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_one_yukawa
 *
 * \sa sasfit_twoyukawa.h, sq_plugins_yukawa
 */
sasfit_twoyukawa_DLLEXP scalar sasfit_sq_one_yukawa_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_one_yukawa ################ */

#endif // this file

