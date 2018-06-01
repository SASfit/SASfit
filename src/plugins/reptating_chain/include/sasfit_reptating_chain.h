/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_REPTATING_CHAIN_H
#define SASFIT_PLUGIN_REPTATING_CHAIN_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_reptating_chain.h
 * Public available functions and descriptions of the reptating_chain plugin.
 */

/**
 * \def sasfit_reptating_chain_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_reptating_chain_EXPORTS)
	#ifdef sasfit_reptating_chain_DLLEXP
	#undef sasfit_reptating_chain_DLLEXP
	#endif
	#define sasfit_reptating_chain_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_reptating_chain_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_reptating_chain_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_reptating_chain ################ */
/**
 * \defgroup ff_reptating_chain reptating chain
 * \ingroup ff_plugins_deformed_textured
 *
 * \brief \<some brief description of reptating chain function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of unstreched polymer</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>streching factor</td>
 *      </tr><tr>
 *       <td>\b t/tau</td>
 *       <td>ralaxation time in units of Rouse time</td>
 *      </tr><tr>
 *       <td>\b theta_0</td>
 *       <td>stretching direction in the detector plane</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>q-direction on the detector plane</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_reptating_chain
 *
 * \sa sasfit_reptating_chain.h, ff_plugins_deformed_textured
 */
sasfit_reptating_chain_DLLEXP scalar sasfit_ff_reptating_chain(scalar q, sasfit_param * p);

/**
 * \ingroup ff_reptating_chain
 *
 * \sa sasfit_reptating_chain.h, ff_plugins_deformed_textured
 */
sasfit_reptating_chain_DLLEXP scalar sasfit_ff_reptating_chain_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_reptating_chain
 *
 * \sa sasfit_reptating_chain.h, ff_plugins_deformed_textured
 */
sasfit_reptating_chain_DLLEXP scalar sasfit_ff_reptating_chain_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_reptating_chain ################ */


#endif // this file

