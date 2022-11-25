/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_SQ_OF_OZ_SOLVER_H
#define SASFIT_PLUGIN_SQ_OF_OZ_SOLVER_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_sq_of_oz_solver.h
 * Public available functions and descriptions of the sq_of_oz_solver plugin.
 */

/**
 * \def sasfit_sq_of_oz_solver_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_sq_of_oz_solver_EXPORTS)
	#ifdef sasfit_sq_of_oz_solver_DLLEXP
	#undef sasfit_sq_of_oz_solver_DLLEXP
	#endif
	#define sasfit_sq_of_oz_solver_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_sq_of_oz_solver_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_sq_of_oz_solver_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq_sq_oz_1 ################ */
/**
 * \defgroup sq_sq_oz_1 SQ oz 1
 * \ingroup sq_plugins_oz_solver
 *
 * \brief \<some brief description of SQ oz 1 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>hard core radius</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_sq_oz_1
 *
 * \sa sasfit_sq_of_oz_solver.h, sq_plugins_oz_solver
 */
sasfit_sq_of_oz_solver_DLLEXP scalar sasfit_sq_sq_oz_1(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sq_oz_1
 *
 * \sa sasfit_sq_of_oz_solver.h, sq_plugins_oz_solver
 */
sasfit_sq_of_oz_solver_DLLEXP scalar sasfit_sq_sq_oz_1_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sq_oz_1
 *
 * \sa sasfit_sq_of_oz_solver.h, sq_plugins_oz_solver
 */
sasfit_sq_of_oz_solver_DLLEXP scalar sasfit_sq_sq_oz_1_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_sq_oz_1 ################ */

/* ################ start sq_sq_oz_2 ################ */
/**
 * \defgroup sq_sq_oz_2 SQ oz 2
 * \ingroup sq_plugins_oz_solver
 *
 * \brief \<some brief description of SQ oz 2 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>hard core radius</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_sq_oz_2
 *
 * \sa sasfit_sq_of_oz_solver.h, sq_plugins_oz_solver
 */
sasfit_sq_of_oz_solver_DLLEXP scalar sasfit_sq_sq_oz_2(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sq_oz_2
 *
 * \sa sasfit_sq_of_oz_solver.h, sq_plugins_oz_solver
 */
sasfit_sq_of_oz_solver_DLLEXP scalar sasfit_sq_sq_oz_2_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sq_oz_2
 *
 * \sa sasfit_sq_of_oz_solver.h, sq_plugins_oz_solver
 */
sasfit_sq_of_oz_solver_DLLEXP scalar sasfit_sq_sq_oz_2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_sq_oz_2 ################ */

/* ################ start sq_sq_oz_3 ################ */
/**
 * \defgroup sq_sq_oz_3 SQ oz 3
 * \ingroup sq_plugins_oz_solver
 *
 * \brief \<some brief description of SQ oz 3 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>hard core radius</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_sq_oz_3
 *
 * \sa sasfit_sq_of_oz_solver.h, sq_plugins_oz_solver
 */
sasfit_sq_of_oz_solver_DLLEXP scalar sasfit_sq_sq_oz_3(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sq_oz_3
 *
 * \sa sasfit_sq_of_oz_solver.h, sq_plugins_oz_solver
 */
sasfit_sq_of_oz_solver_DLLEXP scalar sasfit_sq_sq_oz_3_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sq_oz_3
 *
 * \sa sasfit_sq_of_oz_solver.h, sq_plugins_oz_solver
 */
sasfit_sq_of_oz_solver_DLLEXP scalar sasfit_sq_sq_oz_3_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_sq_oz_3 ################ */

/* ################ start sq_sq_oz_4 ################ */
/**
 * \defgroup sq_sq_oz_4 SQ oz 4
 * \ingroup sq_plugins_oz_solver
 *
 * \brief \<some brief description of SQ oz 4 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>hard core radius</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_sq_oz_4
 *
 * \sa sasfit_sq_of_oz_solver.h, sq_plugins_oz_solver
 */
sasfit_sq_of_oz_solver_DLLEXP scalar sasfit_sq_sq_oz_4(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sq_oz_4
 *
 * \sa sasfit_sq_of_oz_solver.h, sq_plugins_oz_solver
 */
sasfit_sq_of_oz_solver_DLLEXP scalar sasfit_sq_sq_oz_4_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sq_oz_4
 *
 * \sa sasfit_sq_of_oz_solver.h, sq_plugins_oz_solver
 */
sasfit_sq_of_oz_solver_DLLEXP scalar sasfit_sq_sq_oz_4_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_sq_oz_4 ################ */

/* ################ start sq_sq_oz_5 ################ */
/**
 * \defgroup sq_sq_oz_5 SQ oz 5
 * \ingroup sq_plugins_oz_solver
 *
 * \brief \<some brief description of SQ oz 5 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>hard core radius</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_sq_oz_5
 *
 * \sa sasfit_sq_of_oz_solver.h, sq_plugins_oz_solver
 */
sasfit_sq_of_oz_solver_DLLEXP scalar sasfit_sq_sq_oz_5(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sq_oz_5
 *
 * \sa sasfit_sq_of_oz_solver.h, sq_plugins_oz_solver
 */
sasfit_sq_of_oz_solver_DLLEXP scalar sasfit_sq_sq_oz_5_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sq_oz_5
 *
 * \sa sasfit_sq_of_oz_solver.h, sq_plugins_oz_solver
 */
sasfit_sq_of_oz_solver_DLLEXP scalar sasfit_sq_sq_oz_5_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_sq_oz_5 ################ */

/* ################ start sq_sq_oz_6 ################ */
/**
 * \defgroup sq_sq_oz_6 SQ oz 6
 * \ingroup sq_plugins_oz_solver
 *
 * \brief \<some brief description of SQ oz 6 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>hard core radius</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_sq_oz_6
 *
 * \sa sasfit_sq_of_oz_solver.h, sq_plugins_oz_solver
 */
sasfit_sq_of_oz_solver_DLLEXP scalar sasfit_sq_sq_oz_6(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sq_oz_6
 *
 * \sa sasfit_sq_of_oz_solver.h, sq_plugins_oz_solver
 */
sasfit_sq_of_oz_solver_DLLEXP scalar sasfit_sq_sq_oz_6_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sq_oz_6
 *
 * \sa sasfit_sq_of_oz_solver.h, sq_plugins_oz_solver
 */
sasfit_sq_of_oz_solver_DLLEXP scalar sasfit_sq_sq_oz_6_v(scalar q, sasfit_param * p, int dist);

/**
 * \ingroup sq_sq_oz_6
 *
 * \sa sasfit_sq_of_oz_solver.h, sq_plugins_oz_solver
 */
sasfit_sq_of_oz_solver_DLLEXP scalar sasfit_sq_sq_oz_6_init(scalar *q, scalar *Sq, int dim, sasfit_param * p);
/* ################ stop sq_sq_oz_6 ################ */


#endif // this file

