/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_SHEARFLOW_POLYMERS_LIN_H
#define SASFIT_PLUGIN_SHEARFLOW_POLYMERS_LIN_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_shearflow_polymers_lin.h
 * Public available functions and descriptions of the shearflow_polymers_lin plugin.
 */

/**
 * \def sasfit_shearflow_polymer_lin_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_shearflow_polymers_lin_EXPORTS)
	#ifdef sasfit_shearflow_polymers_lin_DLLEXP
	#undef sasfit_shearflow_polymers_lin_DLLEXP
	#endif
	#define sasfit_shearflow_polymers_lin_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_shearflow_polymers_lin_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_shearflow_polymers_lin_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_shearflow_lin ################ */
/**
 * \defgroup ff_shearflow_lin shearflow_lin
 * \ingroup ff_devel
 *
 * \brief \<some brief description of shearflow_lin function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b fp</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b A</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b B</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b C</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b phi (deg)</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b theta (deg)</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b psi (deg)</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b varphi (deg)</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy(</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td></td>
 *      </tr></table>

 */

/**
 * \ingroup ff_shearflow_lin
 *
 * \sa sasfit_shearflow_polymers_lin.h, ff_devel
 */
sasfit_shearflow_polymers_lin_DLLEXP scalar sasfit_ff_shearflow_lin(scalar q, sasfit_param * p);

/**
 * \ingroup ff_shearflow_lin
 *
 * \sa sasfit_shearflow_lin.h, ff_devel
 */
sasfit_shearflow_polymers_lin_DLLEXP scalar sasfit_ff_shearflow_lin_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_shearflow_polymers_lin
 *
 * \sa sasfit_shearflow_polymers_lin.h, ff_devel
 */
sasfit_shearflow_polymers_lin_DLLEXP scalar sasfit_ff_shearflow_lin_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_shearflow_lin ################ */

/* ################ start ff_shearflow_lin_g1 ################ */
/**
 * \defgroup ff_shearflow_lin_g1 shearflow_lin_g1
 * \ingroup ff_devel
 *
 * \brief \<some brief description of shearflow_lin_g1 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b fp</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b A</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b B</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b C</td>
 *       <td></td>
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
 *       <td>\b varphi (deg)</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b phi_0 (deg)</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b Delta_phi (deg)</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_shearflow_lin_g1
 *
 * \sa sasfit_shearflow_polymers_lin.h, ff_devel
 */
sasfit_shearflow_polymers_lin_DLLEXP scalar sasfit_ff_shearflow_lin_g1(scalar q, sasfit_param * p);

/**
 * \ingroup ff_shearflow_lin_g1
 *
 * \sa sasfit_shearflow_polymers_lin.h, ff_devel
 */
sasfit_shearflow_polymers_lin_DLLEXP scalar sasfit_ff_shearflow_lin_g1_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_shearflow_lin_g1
 *
 * \sa sasfit_shearflow_polymers_lin.h, ff_devel
 */
sasfit_shearflow_polymers_lin_DLLEXP scalar sasfit_ff_shearflow_lin_g1_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_shearflow_lin_g1 ################ */

/* ################ start ff_shearflow_lin_g2 ################ */
/**
 * \defgroup ff_shearflow_lin_g2 shearflow_lin_g2
 * \ingroup ff_devel
 *
 * \brief \<some brief description of shearflow_lin_g2 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b fp</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b A</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b B</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b C</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b psi_0 (deg)</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b P2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b P4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b varphi (deg)</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b theta_0 (deg)</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_shearflow_lin_g2
 *
 * \sa sasfit_shearflow_polymers_lin.h, ff_devel
 */
sasfit_shearflow_polymers_lin_DLLEXP scalar sasfit_ff_shearflow_lin_g2(scalar q, sasfit_param * p);

/**
 * \ingroup ff_shearflow_lin_g2
 *
 * \sa sasfit_shearflow_polymers_lin.h, ff_devel
 */
sasfit_shearflow_polymers_lin_DLLEXP scalar sasfit_ff_shearflow_lin_g2_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_shearflow_lin_g2
 *
 * \sa sasfit_shearflow_polymers_lin.h, ff_devel
 */
sasfit_shearflow_polymers_lin_DLLEXP scalar sasfit_ff_shearflow_lin_g2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_shearflow_lin_g2 ################ */


#endif // this file

