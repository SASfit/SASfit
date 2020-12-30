/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_RANDOM_FLIGHT_H
#define SASFIT_PLUGIN_RANDOM_FLIGHT_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_random_flight.h
 * Public available functions and descriptions of the random_flight plugin.
 */

/**
 * \def sasfit_random_flight_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_random_flight_EXPORTS)
	#ifdef sasfit_random_flight_DLLEXP
	#undef sasfit_random_flight_DLLEXP
	#endif
	#define sasfit_random_flight_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_random_flight_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_random_flight_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq_random_flight ################ */
/**
 * \defgroup sq_random_flight random flight
 * \ingroup sq_plugins_devel
 *
 * \brief \<some brief description of random flight function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b D</td>
 *       <td>step width</td>
 *      </tr><tr>
 *       <td>\b n</td>
 *       <td>number of random flight steps</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_random_flight
 *
 * \sa sasfit_random_flight.h, sq_plugins_devel
 */
sasfit_random_flight_DLLEXP scalar sasfit_sq_random_flight(scalar q, sasfit_param * p);

/**
 * \ingroup sq_random_flight
 *
 * \sa sasfit_random_flight.h, sq_plugins_devel
 */
sasfit_random_flight_DLLEXP scalar sasfit_sq_random_flight_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_random_flight
 *
 * \sa sasfit_random_flight.h, sq_plugins_devel
 */
sasfit_random_flight_DLLEXP scalar sasfit_sq_random_flight_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_random_flight ################ */


/* ################ start sq_pc_random_flight ################ */
/**
 * \defgroup sq_pc_random_flight PC random flight
 * \ingroup sq_plugins_devel
 *
 * \brief \<some brief description of PC random flight function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b D</td>
 *       <td>step width</td>
 *      </tr><tr>
 *       <td>\b n</td>
 *       <td>number of random flight steps</td>
 *      </tr><tr>
 *       <td>\b Delta</td>
 *       <td>Gaussian uncertainty in flight steps width</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_pc_random_flight
 *
 * \sa sasfit_random_flight.h, sq_plugins_devel
 */
sasfit_random_flight_DLLEXP scalar sasfit_sq_pc_random_flight(scalar q, sasfit_param * p);

/**
 * \ingroup sq_pc_random_flight
 *
 * \sa sasfit_random_flight.h, sq_plugins_devel
 */
sasfit_random_flight_DLLEXP scalar sasfit_sq_pc_random_flight_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_pc_random_flight
 *
 * \sa sasfit_random_flight.h, sq_plugins_devel
 */
sasfit_random_flight_DLLEXP scalar sasfit_sq_pc_random_flight_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_random_flight ################ */


#endif // this file

