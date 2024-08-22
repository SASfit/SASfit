/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_POLYNOM_H
#define SASFIT_PLUGIN_POLYNOM_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_polynom.h
 * Public available functions and descriptions of the polynom plugin.
 */

/**
 * \def sasfit_polynom_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_polynom_EXPORTS)
	#ifdef sasfit_polynom_DLLEXP
	#undef sasfit_polynom_DLLEXP
	#endif
	#define sasfit_polynom_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_polynom_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_polynom_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_polynom ################ */
/** 
 * \defgroup ff_polynom polynom
 * \ingroup ff_sans
 *
 * \brief calculates a polynom of degree 4 (p0 + p1*x + p2*x^2 + p3*x^3 + p4*x^4)
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b p0</td>
 *       <td>p0  (offset)</td>
 *      </tr><tr>
 *       <td>\b p1</td>
 *       <td>p1 * q (linear)</td>
 *      </tr><tr>
 *       <td>\b p2</td>
 *       <td>p2 * q^2 (quadratic)</td>
 *      </tr><tr>
 *       <td>\b p3</td>
 *       <td>p3 * q^3 (cubic)</td>
 *      </tr><tr>
 *       <td>\b p4</td>
 *       <td>p4 * q^4 (4th order)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_polynom
 *
 * \sa sasfit_polynom.h, ff_sans
 */
sasfit_polynom_DLLEXP scalar sasfit_ff_polynom(scalar q, sasfit_param * p);

/**
 * \ingroup ff_polynom
 *
 * \sa sasfit_polynom.h, ff_sans
 */
sasfit_polynom_DLLEXP scalar sasfit_ff_polynom_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_polynom
 *
 * \sa sasfit_polynom.h, ff_sans
 */
sasfit_polynom_DLLEXP scalar sasfit_ff_polynom_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_polynom ################ */

/* ################ start ff_cv2 ################ */
/** 
 * \defgroup ff_cv2 CV2
 * \ingroup ff_sans
 *
 * \brief calculates forward scattering 
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I_bckgr</td>
 *       <td>offset</td>
 *      </tr><tr>
 *       <td>\b x_min</td>
 *       <td>match point</td>
 *      </tr><tr>
 *       <td>\b I_scaling</td>
 *       <td>intensity scaling</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_cv2
 *
 * \sa sasfit_polynom.h, ff_sans
 */
sasfit_polynom_DLLEXP scalar sasfit_ff_cv2(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cv2
 *
 * \sa sasfit_polynom.h, ff_sans
 */
sasfit_polynom_DLLEXP scalar sasfit_ff_cv2_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cv2
 *
 * \sa sasfit_polynom.h, ff_sans
 */
sasfit_polynom_DLLEXP scalar sasfit_ff_cv2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_cv2 ################ */


#endif // this file

