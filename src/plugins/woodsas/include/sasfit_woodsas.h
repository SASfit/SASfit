/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_WOODSAS_H
#define SASFIT_PLUGIN_WOODSAS_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_woodsas.h
 * Public available functions and descriptions of the woodsas plugin.
 */

/**
 * \def sasfit_woodsas_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_woodsas_EXPORTS)
	#ifdef sasfit_woodsas_DLLEXP
	#undef sasfit_woodsas_DLLEXP
	#endif
	#define sasfit_woodsas_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_woodsas_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_woodsas_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_woodsas ################ */
/**
 * \defgroup ff_woodsas woodsas
 * \ingroup ff_plugins_helix
 *
 * \brief \<some brief description of woodsas function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A_scale</td>
 *       <td>Scaling constant for cylinders</td>
 *      </tr><tr>
 *       <td>\b radius</td>
 *       <td>cylinder radius</td>
 *      </tr><tr>
 *       <td>\b dR_ratio</td>
 *       <td>dR/R</td>
 *      </tr><tr>
 *       <td>\b a</td>
 *       <td>lattice constant a</td>
 *      </tr><tr>
 *       <td>\b da_ratio</td>
 *       <td>da/a</td>
 *      </tr><tr>
 *       <td>\b B_scale</td>
 *       <td>Gaussian scaling constant</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>Gaussian width</td>
 *      </tr><tr>
 *       <td>\b C_scale</td>
 *       <td>power law scaling constant</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>power law exponent</td>
 *      </tr><tr>
 *       <td>\b bckgr</td>
 *       <td>constant background</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_woodsas
 *
 * \sa sasfit_woodsas.h, ff_plugins_helix
 */
sasfit_woodsas_DLLEXP scalar sasfit_ff_woodsas(scalar q, sasfit_param * p);

/**
 * \ingroup ff_woodsas
 *
 * \sa sasfit_woodsas.h, ff_plugins_helix
 */
sasfit_woodsas_DLLEXP scalar sasfit_ff_woodsas_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_woodsas
 *
 * \sa sasfit_woodsas.h, ff_plugins_helix
 */
sasfit_woodsas_DLLEXP scalar sasfit_ff_woodsas_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_woodsas ################ */


#endif // this file

