/*
 * Author(s) of this file:
 *   (see top-level README.md)
 */

#ifndef SASFIT_PLUGIN_RFA_H
#define SASFIT_PLUGIN_RFA_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_RFA.h
 * Public available functions and descriptions of the RFA plugin.
 */

/**
 * \def sasfit_RFA_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_RFA_EXPORTS)
	#ifdef sasfit_RFA_DLLEXP
	#undef sasfit_RFA_DLLEXP
	#endif
	#define sasfit_RFA_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_RFA_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_RFA_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the structure factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq_RFA1 ################ */
/**
 * \defgroup sq_RFA1 RFA1 (rational-function approximation, single step)
 * \ingroup sq_plugins_user1
 *
 * \brief Rational-Function Approximation (RFA1 variant) structure
 *        factor for a hard core plus a single square-well or
 *        square-shoulder potential step.
 *
 * Implements the RFA1 variant of Santos, Yuste & Lopez de Haro
 * (Condens. Matter Phys. 15, 23602 (2012)), generalising S. Bravo
 * Yuste & A. Santos (J. Chem. Phys. 101, 2355 (1994)): the coefficient
 * A_1 is fixed at its exact zero-density limit and only B_1 is solved
 * for, via continuity of the cavity function y(r) at the step. Reduces
 * to the exact Percus-Yevick solution in the hard-sphere and
 * sticky-hard-sphere limits.
 *
 * The underlying nonlinear solve (GSL's Powell hybrid method over the
 * rational-function ansatz's coefficients) is cached across repeated
 * calls with unchanged parameters, and warm-started from the previous
 * solution for small parameter perturbations -- both automatic,
 * requiring no action from the GUI or a fit script calling this
 * function repeatedly. A converged-but-unphysical result (g(r)<0
 * somewhere in the first shell -- see the main project's
 * documentation) is treated the same as a failed solve.
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b RADIUS</td>
 *       <td>hard-sphere radius, in Angstrom</td>
 *      </tr><tr>
 *       <td>\b LAMBDA</td>
 *       <td>reduced range parameter of the step, dimensionless, LAMBDA > 1</td>
 *      </tr><tr>
 *       <td>\b EPSILON_OVER_KT</td>
 *       <td>signed reduced step height beta*epsilon; negative = attractive well, positive = repulsive shoulder</td>
 *      </tr><tr>
 *       <td>\b VOLFRACTION</td>
 *       <td>packing fraction eta, 0 < eta < 1</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_RFA1
 *
 * \sa sasfit_RFA.h, sq_plugins_user1
 */
sasfit_RFA_DLLEXP scalar sasfit_sq_RFA1(scalar q, sasfit_param * p);

/**
 * \ingroup sq_RFA1
 *
 * \sa sasfit_RFA.h, sq_plugins_user1
 */
sasfit_RFA_DLLEXP scalar sasfit_sq_RFA1_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_RFA1
 *
 * \sa sasfit_RFA.h, sq_plugins_user1
 */
sasfit_RFA_DLLEXP scalar sasfit_sq_RFA1_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_RFA1 ################ */


/* ################ start sq_RFA2 ################ */
/**
 * \defgroup sq_RFA2 RFA2 (rational-function approximation, single step)
 * \ingroup sq_plugins_user1
 *
 * \brief Rational-Function Approximation (RFA2 variant) structure
 *        factor for a hard core plus a single square-well or
 *        square-shoulder potential step.
 *
 * As \ref sq_RFA1, but additionally imposes continuity of y'(r) (not
 * just y(r)) at the step, solving for both A_1 and B_1 jointly -- more
 * internally self-consistent than RFA1, but NOT uniformly more
 * accurate; the two variants can disagree by a factor of several in
 * g(1+) for square wells near the model's high-density/deep-well
 * breakdown boundary (see the main project's report.pdf, Section 7).
 * Fitting data with both \ref sq_RFA1 and \ref sq_RFA2 and comparing is
 * recommended over trusting either alone in that regime.
 *
 * Caching, warm-starting and the physicality guard: as \ref sq_RFA1.
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b RADIUS</td>
 *       <td>hard-sphere radius, in Angstrom</td>
 *      </tr><tr>
 *       <td>\b LAMBDA</td>
 *       <td>reduced range parameter of the step, dimensionless, LAMBDA > 1</td>
 *      </tr><tr>
 *       <td>\b EPSILON_OVER_KT</td>
 *       <td>signed reduced step height beta*epsilon; negative = attractive well, positive = repulsive shoulder</td>
 *      </tr><tr>
 *       <td>\b VOLFRACTION</td>
 *       <td>packing fraction eta, 0 < eta < 1</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_RFA2
 *
 * \sa sasfit_RFA.h, sq_plugins_user1
 */
sasfit_RFA_DLLEXP scalar sasfit_sq_RFA2(scalar q, sasfit_param * p);

/**
 * \ingroup sq_RFA2
 *
 * \sa sasfit_RFA.h, sq_plugins_user1
 */
sasfit_RFA_DLLEXP scalar sasfit_sq_RFA2_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_RFA2
 *
 * \sa sasfit_RFA.h, sq_plugins_user1
 */
sasfit_RFA_DLLEXP scalar sasfit_sq_RFA2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_RFA2 ################ */


/* ################ start sq_RFA1_3step ################ */
/**
 * \defgroup sq_RFA1_3step RFA1_3step (rational-function approximation, three steps)
 * \ingroup sq_plugins_user1
 *
 * \brief Rational-Function Approximation (RFA1 variant) structure
 *        factor for a hard core plus three independently-signed
 *        piece-wise constant potential steps.
 *
 * As \ref sq_RFA1, generalised to n=3 potential steps (the underlying
 * theory is defined for arbitrary n -- Santos, Yuste & Lopez de Haro,
 * Condens. Matter Phys. 15, 23602 (2012)). Each step's sign is
 * independent: e.g. a repulsive shoulder followed by an attractive
 * well followed by a second shoulder is EPS1_OVER_KT>0, EPS2_OVER_KT<0,
 * EPS3_OVER_KT>0.
 *
 * Caching, warm-starting and the physicality guard: as \ref sq_RFA1.
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b RADIUS</td>
 *       <td>hard-sphere radius, in Angstrom</td>
 *      </tr><tr>
 *       <td>\b LAMBDA1</td>
 *       <td>reduced outer edge of step 1, dimensionless, LAMBDA1 > 1</td>
 *      </tr><tr>
 *       <td>\b LAMBDA2</td>
 *       <td>reduced outer edge of step 2, LAMBDA2 > LAMBDA1</td>
 *      </tr><tr>
 *       <td>\b LAMBDA3</td>
 *       <td>reduced outer edge of step 3, LAMBDA3 > LAMBDA2</td>
 *      </tr><tr>
 *       <td>\b EPS1_OVER_KT</td>
 *       <td>signed reduced height of step 1</td>
 *      </tr><tr>
 *       <td>\b EPS2_OVER_KT</td>
 *       <td>signed reduced height of step 2</td>
 *      </tr><tr>
 *       <td>\b EPS3_OVER_KT</td>
 *       <td>signed reduced height of step 3</td>
 *      </tr><tr>
 *       <td>\b VOLFRACTION</td>
 *       <td>packing fraction eta, 0 < eta < 1</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_RFA1_3step
 *
 * \sa sasfit_RFA.h, sq_plugins_user1
 */
sasfit_RFA_DLLEXP scalar sasfit_sq_RFA1_3step(scalar q, sasfit_param * p);

/**
 * \ingroup sq_RFA1_3step
 *
 * \sa sasfit_RFA.h, sq_plugins_user1
 */
sasfit_RFA_DLLEXP scalar sasfit_sq_RFA1_3step_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_RFA1_3step
 *
 * \sa sasfit_RFA.h, sq_plugins_user1
 */
sasfit_RFA_DLLEXP scalar sasfit_sq_RFA1_3step_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_RFA1_3step ################ */


/* ################ start sq_RFA2_3step ################ */
/**
 * \defgroup sq_RFA2_3step RFA2_3step (rational-function approximation, three steps)
 * \ingroup sq_plugins_user1
 *
 * \brief Rational-Function Approximation (RFA2 variant) structure
 *        factor for a hard core plus three independently-signed
 *        piece-wise constant potential steps.
 *
 * As \ref sq_RFA1_3step, but using the RFA2 variant (see \ref sq_RFA2
 * for the RFA1/RFA2 distinction).
 *
 * Caching, warm-starting and the physicality guard: as \ref sq_RFA1.
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b RADIUS</td>
 *       <td>hard-sphere radius, in Angstrom</td>
 *      </tr><tr>
 *       <td>\b LAMBDA1</td>
 *       <td>reduced outer edge of step 1, dimensionless, LAMBDA1 > 1</td>
 *      </tr><tr>
 *       <td>\b LAMBDA2</td>
 *       <td>reduced outer edge of step 2, LAMBDA2 > LAMBDA1</td>
 *      </tr><tr>
 *       <td>\b LAMBDA3</td>
 *       <td>reduced outer edge of step 3, LAMBDA3 > LAMBDA2</td>
 *      </tr><tr>
 *       <td>\b EPS1_OVER_KT</td>
 *       <td>signed reduced height of step 1</td>
 *      </tr><tr>
 *       <td>\b EPS2_OVER_KT</td>
 *       <td>signed reduced height of step 2</td>
 *      </tr><tr>
 *       <td>\b EPS3_OVER_KT</td>
 *       <td>signed reduced height of step 3</td>
 *      </tr><tr>
 *       <td>\b VOLFRACTION</td>
 *       <td>packing fraction eta, 0 < eta < 1</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_RFA2_3step
 *
 * \sa sasfit_RFA.h, sq_plugins_user1
 */
sasfit_RFA_DLLEXP scalar sasfit_sq_RFA2_3step(scalar q, sasfit_param * p);

/**
 * \ingroup sq_RFA2_3step
 *
 * \sa sasfit_RFA.h, sq_plugins_user1
 */
sasfit_RFA_DLLEXP scalar sasfit_sq_RFA2_3step_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_RFA2_3step
 *
 * \sa sasfit_RFA.h, sq_plugins_user1
 */
sasfit_RFA_DLLEXP scalar sasfit_sq_RFA2_3step_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_RFA2_3step ################ */

#endif // this file
