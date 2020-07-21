/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_DAB_EPSILON_H
#define SASFIT_PLUGIN_DAB_EPSILON_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_dab_epsilon.h
 * Public available functions and descriptions of the dab_epsilon plugin.
 */

/**
 * \def sasfit_dab_epsilon_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_dab_epsilon_EXPORTS)
	#ifdef sasfit_dab_epsilon_DLLEXP
	#undef sasfit_dab_epsilon_DLLEXP
	#endif
	#define sasfit_dab_epsilon_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_dab_epsilon_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_dab_epsilon_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_dab_epsilon ################ */
/**
 * \defgroup ff_dab_epsilon epsilonDAB
 * \ingroup ff_non-particulate
 *
 * \brief \<some brief description of epsilonDAB function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b xi</td>
 *       <td>correlation length</td>
 *      </tr><tr>
 *       <td>\b epsilon</td>
 *       <td>eccentricity</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_dab_epsilon
 *
 * \sa sasfit_dab_epsilon.h, ff_non-particulate
 */
sasfit_dab_epsilon_DLLEXP scalar sasfit_ff_dab_epsilon(scalar q, sasfit_param * p);

/**
 * \ingroup ff_dab_epsilon
 *
 * \sa sasfit_dab_epsilon.h, ff_non-particulate
 */
sasfit_dab_epsilon_DLLEXP scalar sasfit_ff_dab_epsilon_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_dab_epsilon
 *
 * \sa sasfit_dab_epsilon.h, ff_non-particulate
 */
sasfit_dab_epsilon_DLLEXP scalar sasfit_ff_dab_epsilon_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_dab_epsilon ################ */


#endif // this file

