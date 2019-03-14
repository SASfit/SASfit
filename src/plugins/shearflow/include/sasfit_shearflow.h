/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_SHEARFLOW_H
#define SASFIT_PLUGIN_SHEARFLOW_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_shearflow.h
 * Public available functions and descriptions of the shearflow plugin.
 */

/**
 * \def sasfit_shearflow_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_shearflow_EXPORTS)
	#ifdef sasfit_shearflow_DLLEXP
	#undef sasfit_shearflow_DLLEXP
	#endif
	#define sasfit_shearflow_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_shearflow_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_shearflow_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_polymer_under_shearflow ################ */
/**
 * \defgroup ff_polymer_under_shearflow Polymer under ShearFlow
 * \ingroup ff_devel
 *
 * \brief \<some brief description of Polymer under ShearFlow function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td>Radius of gyration</td>
 *      </tr><tr>
 *       <td>\b Wi</td>
 *       <td>Weissenberg number</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between q and shear direction</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_polymer_under_shearflow
 *
 * \sa sasfit_shearflow.h, ff_devel
 */
sasfit_shearflow_DLLEXP scalar sasfit_ff_polymer_under_shearflow(scalar q, sasfit_param * p);

/**
 * \ingroup ff_polymer_under_shearflow
 *
 * \sa sasfit_shearflow.h, ff_devel
 */
sasfit_shearflow_DLLEXP scalar sasfit_ff_polymer_under_shearflow_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_polymer_under_shearflow
 *
 * \sa sasfit_shearflow.h, ff_devel
 */
sasfit_shearflow_DLLEXP scalar sasfit_ff_polymer_under_shearflow_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_polymer_under_shearflow ################ */


/* ################ start ff_polymer_under_shearflow1 ################ */
/**
 * \defgroup ff_polymer_under_shearflow1 Polymer under ShearFlow1
 * \ingroup ff_devel
 *
 * \brief \<some brief description of Polymer under ShearFlow1 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td>Radius of gyration</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between q and shear direction</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>exponent (between 1-2)</td>
 *      </tr><tr>
 *       <td>\b Bx</td>
 *       <td>x-direction</td>
 *      </tr><tr>
 *       <td>\b Bz</td>
 *       <td>z-direction</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_polymer_under_shearflow1
 *
 * \sa sasfit_shearflow.h, ff_devel
 */
sasfit_shearflow_DLLEXP scalar sasfit_ff_polymer_under_shearflow1(scalar q, sasfit_param * p);

/**
 * \ingroup ff_polymer_under_shearflow1
 *
 * \sa sasfit_shearflow.h, ff_devel
 */
sasfit_shearflow_DLLEXP scalar sasfit_ff_polymer_under_shearflow1_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_polymer_under_shearflow1
 *
 * \sa sasfit_shearflow.h, ff_devel
 */
sasfit_shearflow_DLLEXP scalar sasfit_ff_polymer_under_shearflow1_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_polymer_under_shearflow1 ################ */

/* ################ start ff_polymer_under_shearflow2 ################ */
/**
 * \defgroup ff_polymer_under_shearflow2 Polymer under ShearFlow2
 * \ingroup ff_devel
 *
 * \brief \<some brief description of Polymer under ShearFlow2 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td>Radius of gyration</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between q and shear direction</td>
 *      </tr><tr>
 *       <td>\b nu</td>
 *       <td>slope change (between 0-1)</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>x-direction</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>z-direction</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>x-direction</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>z-direction</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_polymer_under_shearflow2
 *
 * \sa sasfit_shearflow.h, ff_devel
 */
sasfit_shearflow_DLLEXP scalar sasfit_ff_polymer_under_shearflow2(scalar q, sasfit_param * p);

/**
 * \ingroup ff_polymer_under_shearflow2
 *
 * \sa sasfit_shearflow.h, ff_devel
 */
sasfit_shearflow_DLLEXP scalar sasfit_ff_polymer_under_shearflow2_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_polymer_under_shearflow2
 *
 * \sa sasfit_shearflow.h, ff_devel
 */
sasfit_shearflow_DLLEXP scalar sasfit_ff_polymer_under_shearflow2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_polymer_under_shearflow2 ################ */


/* ################ start ff_ring_under_shearflow ################ */
/**
 * \defgroup ff_ring_under_shearflow Ring under ShearFlow
 * \ingroup ff_devel
 *
 * \brief \<some brief description of Ring under ShearFlow function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td>Radius of gyration</td>
 *      </tr><tr>
 *       <td>\b Wi</td>
 *       <td>Weissenberg number</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between q and shear direction</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ring_under_shearflow
 *
 * \sa sasfit_shearflow.h, ff_devel
 */
sasfit_shearflow_DLLEXP scalar sasfit_ff_ring_under_shearflow(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ring_under_shearflow
 *
 * \sa sasfit_shearflow.h, ff_devel
 */
sasfit_shearflow_DLLEXP scalar sasfit_ff_ring_under_shearflow_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ring_under_shearflow
 *
 * \sa sasfit_shearflow.h, ff_devel
 */
sasfit_shearflow_DLLEXP scalar sasfit_ff_ring_under_shearflow_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ring_under_shearflow ################ */


/* ################ start ff_ring_under_shearflow1 ################ */
/**
 * \defgroup ff_ring_under_shearflow1 Ring under ShearFlow1
 * \ingroup ff_devel
 *
 * \brief \<some brief description of Ring under ShearFlow1 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td>Radius of gyration</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between q and shear direction</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>exponent (between 1-2)</td>
 *      </tr><tr>
 *       <td>\b Bx</td>
 *       <td>x-direction</td>
 *      </tr><tr>
 *       <td>\b Bz</td>
 *       <td>z-direction</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ring_under_shearflow1
 *
 * \sa sasfit_shearflow.h, ff_devel
 */
sasfit_shearflow_DLLEXP scalar sasfit_ff_ring_under_shearflow1(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ring_under_shearflow1
 *
 * \sa sasfit_shearflow.h, ff_devel
 */
sasfit_shearflow_DLLEXP scalar sasfit_ff_ring_under_shearflow1_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ring_under_shearflow1
 *
 * \sa sasfit_shearflow.h, ff_devel
 */
sasfit_shearflow_DLLEXP scalar sasfit_ff_ring_under_shearflow1_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ring_under_shearflow1 ################ */

/* ################ start ff_ring_under_shearflow2 ################ */
/**
 * \defgroup ff_ring_under_shearflow2 Ring under ShearFlow2
 * \ingroup ff_devel
 *
 * \brief \<some brief description of Ring under ShearFlow2 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td>Radius of gyration</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between q and shear direction</td>
 *      </tr><tr>
 *       <td>\b nu</td>
 *       <td>slope change (between 0-1)</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>x-direction</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>z-direction</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>x-direction</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>z-direction</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ring_under_shearflow2
 *
 * \sa sasfit_shearflow.h, ff_devel
 */
sasfit_shearflow_DLLEXP scalar sasfit_ff_ring_under_shearflow2(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ring_under_shearflow2
 *
 * \sa sasfit_shearflow.h, ff_devel
 */
sasfit_shearflow_DLLEXP scalar sasfit_ff_ring_under_shearflow2_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ring_under_shearflow2
 *
 * \sa sasfit_shearflow.h, ff_devel
 */
sasfit_shearflow_DLLEXP scalar sasfit_ff_ring_under_shearflow2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ring_under_shearflow2 ################ */

#endif // this file

