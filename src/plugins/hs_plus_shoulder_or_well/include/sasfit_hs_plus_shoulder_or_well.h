/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_HS_PLUS_SHOULDER_OR_WELL_H
#define SASFIT_PLUGIN_HS_PLUS_SHOULDER_OR_WELL_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_hs_plus_shoulder_or_well.h
 * Public available functions and descriptions of the hs_plus_shoulder_or_well plugin.
 */

/**
 * \def sasfit_hs_plus_shoulder_or_well_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_hs_plus_shoulder_or_well_EXPORTS)
	#ifdef sasfit_hs_plus_shoulder_or_well_DLLEXP
	#undef sasfit_hs_plus_shoulder_or_well_DLLEXP
	#endif
	#define sasfit_hs_plus_shoulder_or_well_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_hs_plus_shoulder_or_well_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_hs_plus_shoulder_or_well_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq_sticky_hard_sphere ################ */
/**
 * \defgroup sq_sticky_hard_sphere Sticky Hard Sphere
 * \ingroup sq_plugins_sticky_hard_sphere
 *
 * \brief \<some brief description of Sticky Hard Sphere function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b RHS</td>
 *       <td>hard sphere repulsion radius</td>
 *      </tr><tr>
 *       <td>\b fp</td>
 *       <td>volume fraction</td>
 *      </tr><tr>
 *       <td>\b tau</td>
 *       <td>stickiness (tau >~ 0.09)</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_sticky_hard_sphere
 *
 * \sa sasfit_hs_plus_shoulder_or_well.h, sq_plugins_sticky_hard_sphere
 */
sasfit_hs_plus_shoulder_or_well_DLLEXP scalar sasfit_sq_sticky_hard_sphere(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sticky_hard_sphere
 *
 * \sa sasfit_hs_plus_shoulder_or_well.h, sq_plugins_sticky_hard_sphere
 */
sasfit_hs_plus_shoulder_or_well_DLLEXP scalar sasfit_sq_sticky_hard_sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sticky_hard_sphere
 *
 * \sa sasfit_hs_plus_shoulder_or_well.h, sq_plugins_sticky_hard_sphere
 */
sasfit_hs_plus_shoulder_or_well_DLLEXP scalar sasfit_sq_sticky_hard_sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_sticky_hard_sphere ################ */

/* ################ start sq_sticky_hard_sphere_2 ################ */
/**
 * \defgroup sq_sticky_hard_sphere_2 Sticky Hard Sphere 2
 * \ingroup sq_plugins_sticky_hard_sphere
 *
 * \brief \<some brief description of Sticky Hard Sphere 2 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b RHS</td>
 *       <td>hard sphere repulsive radius</td>
 *      </tr><tr>
 *       <td>\b fp</td>
 *       <td>volume fraction</td>
 *      </tr><tr>
 *       <td>\b tau</td>
 *       <td>stickiness  (tau >~ 0.09)</td>
 *      </tr><tr>
 *       <td>\b Delta</td>
 *       <td>width of stickiness potential</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_sticky_hard_sphere_2
 *
 * \sa sasfit_hs_plus_shoulder_or_well.h, sq_plugins_sticky_hard_sphere
 */
sasfit_hs_plus_shoulder_or_well_DLLEXP scalar sasfit_sq_sticky_hard_sphere_2(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sticky_hard_sphere_2
 *
 * \sa sasfit_hs_plus_shoulder_or_well.h, sq_plugins_sticky_hard_sphere
 */
sasfit_hs_plus_shoulder_or_well_DLLEXP scalar sasfit_sq_sticky_hard_sphere_2_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sticky_hard_sphere_2
 *
 * \sa sasfit_hs_plus_shoulder_or_well.h, sq_plugins_sticky_hard_sphere
 */
sasfit_hs_plus_shoulder_or_well_DLLEXP scalar sasfit_sq_sticky_hard_sphere_2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_sticky_hard_sphere_2 ################ */

/* ################ start sq_square_well_potential ################ */
/**
 * \defgroup sq_square_well_potential Square Well Potential
 * \ingroup sq_plugins_sticky_hard_sphere
 *
 * \brief \<some brief description of Square Well Potential function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b RHS</td>
 *       <td>hard sphere repulsive radius</td>
 *      </tr><tr>
 *       <td>\b fp</td>
 *       <td>volume fraction</td>
 *      </tr><tr>
 *       <td>\b epsi/kT</td>
 *       <td>square well depth in units of kT</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>relative square well width, lambda >1\ntotal width Delta=2 RHS (lambda-1)</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_square_well_potential
 *
 * \sa sasfit_hs_plus_shoulder_or_well.h, sq_plugins_sticky_hard_sphere
 */
sasfit_hs_plus_shoulder_or_well_DLLEXP scalar sasfit_sq_square_well_potential(scalar q, sasfit_param * p);

/**
 * \ingroup sq_square_well_potential
 *
 * \sa sasfit_hs_plus_shoulder_or_well.h, sq_plugins_sticky_hard_sphere
 */
sasfit_hs_plus_shoulder_or_well_DLLEXP scalar sasfit_sq_square_well_potential_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_square_well_potential
 *
 * \sa sasfit_hs_plus_shoulder_or_well.h, sq_plugins_sticky_hard_sphere
 */
sasfit_hs_plus_shoulder_or_well_DLLEXP scalar sasfit_sq_square_well_potential_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_square_well_potential ################ */

/* ################ start sq_square_well_potential_2 ################ */
/**
 * \defgroup sq_square_well_potential_2 Square Well Potential 2
 * \ingroup sq_plugins_sticky_hard_sphere
 *
 * \brief \<some brief description of Square Well Potential 2 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b RHS</td>
 *       <td>hard sphere repulsive radius</td>
 *      </tr><tr>
 *       <td>\b fp</td>
 *       <td>volume fraction</td>
 *      </tr><tr>
 *       <td>\b epsi/kT</td>
 *       <td>square well depth in units of kT</td>
 *      </tr><tr>
 *       <td>\b Delta</td>
 *       <td>square well width</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_square_well_potential_2
 *
 * \sa sasfit_hs_plus_shoulder_or_well.h, sq_plugins_sticky_hard_sphere
 */
sasfit_hs_plus_shoulder_or_well_DLLEXP scalar sasfit_sq_square_well_potential_2(scalar q, sasfit_param * p);

/**
 * \ingroup sq_square_well_potential_2
 *
 * \sa sasfit_hs_plus_shoulder_or_well.h, sq_plugins_sticky_hard_sphere
 */
sasfit_hs_plus_shoulder_or_well_DLLEXP scalar sasfit_sq_square_well_potential_2_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_square_well_potential_2
 *
 * \sa sasfit_hs_plus_shoulder_or_well.h, sq_plugins_sticky_hard_sphere
 */
sasfit_hs_plus_shoulder_or_well_DLLEXP scalar sasfit_sq_square_well_potential_2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_square_well_potential_2 ################ */

/* ################ start sq_thin_square_well_potential ################ */
/**
 * \defgroup sq_thin_square_well_potential Thin Square Well Potential
 * \ingroup sq_plugins_sticky_hard_sphere
 *
 * \brief \<some brief description of Thin Square Well Potential function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b RHS</td>
 *       <td>hard sphere repulsive radius</td>
 *      </tr><tr>
 *       <td>\b U/kT</td>
 *       <td>square well depth in terms of kT</td>
 *      </tr><tr>
 *       <td>\b fp</td>
 *       <td>volume fraction</td>
 *      </tr><tr>
 *       <td>\b epsilon</td>
 *       <td>relative square well depth, value between 0 and 0.05\nepsilon=DeltaR/(2RHS+DeltaR)</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_thin_square_well_potential
 *
 * \sa sasfit_hs_plus_shoulder_or_well.h, sq_plugins_sticky_hard_sphere
 */
sasfit_hs_plus_shoulder_or_well_DLLEXP scalar sasfit_sq_thin_square_well_potential(scalar q, sasfit_param * p);

/**
 * \ingroup sq_thin_square_well_potential
 *
 * \sa sasfit_hs_plus_shoulder_or_well.h, sq_plugins_sticky_hard_sphere
 */
sasfit_hs_plus_shoulder_or_well_DLLEXP scalar sasfit_sq_thin_square_well_potential_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_thin_square_well_potential
 *
 * \sa sasfit_hs_plus_shoulder_or_well.h, sq_plugins_sticky_hard_sphere
 */
sasfit_hs_plus_shoulder_or_well_DLLEXP scalar sasfit_sq_thin_square_well_potential_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_thin_square_well_potential ################ */


/* ################ start sq_two_piecewise_constant ################ */
/**
 * \defgroup sq_two_piecewise_constant two piecewise constant
 * \ingroup sq_plugins_sticky_hard_sphere
 *
 * \brief \<some brief description of square shoulder function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b RHS</td>
 *       <td>hard sphere repulsion radius</td>
 *      </tr><tr>
 *       <td>\b DUMMY</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>volume fraction</td>
 *      </tr><tr>
 *       <td>\b lambda1</td>
 *       <td>relative square shoulder width, lambda>0, total width Delta=2RHS(lambda-1)</td>
 *      </tr><tr>
 *       <td>\b E1/kBT</td>
 *       <td>square well depth in units of kBT</td>
 *      </tr><tr>
 *       <td>\b lambda2</td>
 *       <td>relative square shoulder width, lambda>0, total width Delta=2RHS(lambda-1)</td>
 *      </tr><tr>
 *       <td>\b E2/kBT</td>
 *       <td>square well depth in units of kBT</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_two_piecewise_constant
 *
 * \sa sasfit_hs_plus_shoulder_or_well.h, sq_plugins_sticky_hard_sphere
 */
sasfit_hs_plus_shoulder_or_well_DLLEXP scalar sasfit_sq_two_piecewise_constant(scalar q, sasfit_param * p);

/**
 * \ingroup sq_two_piecewise_constant
 *
 * \sa sasfit_hs_plus_shoulder_or_well.h, sq_plugins_sticky_hard_sphere
 */
sasfit_hs_plus_shoulder_or_well_DLLEXP scalar sasfit_sq_two_piecewise_constant_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_two_piecewise_constant
 *
 * \sa sasfit_hs_plus_shoulder_or_well.h, sq_plugins_sticky_hard_sphere
 */
sasfit_hs_plus_shoulder_or_well_DLLEXP scalar sasfit_sq_two_piecewise_constant_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_two_piecewise_constant ################ */

#endif // this file

