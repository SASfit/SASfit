/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_ONEYUKAWAFRACTALWITH_NN_H
#define SASFIT_PLUGIN_ONEYUKAWAFRACTALWITH_NN_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_oneyukawafractalwith_nn.h
 * Public available functions and descriptions of the oneyukawafractalwith_nn plugin.
 */

/**
 * \def sasfit_oneyukawafractalwith_nn_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_oneyukawafractalwith_nn_EXPORTS)
	#ifdef sasfit_oneyukawafractalwith_nn_DLLEXP
	#undef sasfit_oneyukawafractalwith_nn_DLLEXP
	#endif
	#define sasfit_oneyukawafractalwith_nn_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_oneyukawafractalwith_nn_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_oneyukawafractalwith_nn_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq_oneyukawa_on_fractal_incl_nn ################ */
/** 
 * \defgroup sq_oneyukawa_on_fractal_incl_nn OneYukawa_on_fractal_incl_nn
 * \ingroup sq_plugins_devel
 *
 * \brief \<some brief description of OneYukawa_on_fractal_incl_nn function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rcluster</td>
 *       <td>radius of cluster, should be similar to xi</td>
 *      </tr><tr>
 *       <td>\b K</td>
 *       <td>potential strength in units of kT, K>0 attraction, K<0 repulsion</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>screening length in nm. The inverse sreening length is Z=1/lambda</td>
 *      </tr><tr>
 *       <td>\b phi</td>
 *       <td>volume fraction</td>
 *      </tr><tr>
 *       <td>\b molarity</td>
 *       <td>concentration in units mol/l. Overrides phi if it larger than 0</td>
 *      </tr><tr>
 *       <td>\b R</td>
 *       <td>radius of particles in fractal</td>
 *      </tr><tr>
 *       <td>\b z1</td>
 *       <td>number of particles in first shell</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>fractal correlation length</td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td>fractal dimension</td>
 *      </tr><tr>
 *       <td>\b N</td>
 *       <td>number of particles in the fractal aggregate</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_oneyukawa_on_fractal_incl_nn
 *
 * \sa sasfit_oneyukawafractalwith_nn.h, sq_plugins_devel
 */
sasfit_oneyukawafractalwith_nn_DLLEXP scalar sasfit_sq_oneyukawa_on_fractal_incl_nn(scalar q, sasfit_param * p);

/**
 * \ingroup sq_oneyukawa_on_fractal_incl_nn
 *
 * \sa sasfit_oneyukawafractalwith_nn.h, sq_plugins_devel
 */
sasfit_oneyukawafractalwith_nn_DLLEXP scalar sasfit_sq_oneyukawa_on_fractal_incl_nn_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_oneyukawa_on_fractal_incl_nn
 *
 * \sa sasfit_oneyukawafractalwith_nn.h, sq_plugins_devel
 */
sasfit_oneyukawafractalwith_nn_DLLEXP scalar sasfit_sq_oneyukawa_on_fractal_incl_nn_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_oneyukawa_on_fractal_incl_nn ################ */


#endif // this file

