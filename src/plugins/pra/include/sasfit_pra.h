/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_PRA_H
#define SASFIT_PLUGIN_PRA_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_pra.h
 * Public available functions and descriptions of the pra plugin.
 */

/**
 * \def sasfit_pra_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_pra_EXPORTS)
	#ifdef sasfit_pra_DLLEXP
	#undef sasfit_pra_DLLEXP
	#endif
	#define sasfit_pra_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_pra_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_pra_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq_p_ra ################ */
/** 
 * \defgroup sq_p_ra P_RA
 * \ingroup sq_plugins_devel
 *
 * \brief \<some brief description of P_RA function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b b</td>
 *       <td>bond length of the
polycondensates</td>
 *      </tr><tr>
 *       <td>\b f</td>
 *       <td>functionality</td>
 *      </tr><tr>
 *       <td>\b nw</td>
 *       <td>weight-average number of the complex units per
the polyelectrolyte complex</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_p_ra
 *
 * \sa sasfit_pra.h, sq_plugins_devel
 */
sasfit_pra_DLLEXP scalar sasfit_sq_p_ra(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p_ra
 *
 * \sa sasfit_pra.h, sq_plugins_devel
 */
sasfit_pra_DLLEXP scalar sasfit_sq_p_ra_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p_ra
 *
 * \sa sasfit_pra.h, sq_plugins_devel
 */
sasfit_pra_DLLEXP scalar sasfit_sq_p_ra_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_p_ra ################ */


#endif // this file

