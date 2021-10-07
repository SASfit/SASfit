/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_POWERLAWSQ_H
#define SASFIT_PLUGIN_POWERLAWSQ_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_powerlawsq.h
 * Public available functions and descriptions of the powerlawsq plugin.
 */

/**
 * \def sasfit_powerlawsq_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_powerlawsq_EXPORTS)
	#ifdef sasfit_powerlawsq_DLLEXP
	#undef sasfit_powerlawsq_DLLEXP
	#endif
	#define sasfit_powerlawsq_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_powerlawsq_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_powerlawsq_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq_simple_sq_power_law ################ */
/** 
 * \defgroup sq_simple_sq_power_law simple SQ power-law
 * \ingroup sq_plugins_fractal_objects
 *
 * \brief \<some brief description of simple SQ power-law function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>scaling factor</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>power law q^-alpha</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_simple_sq_power_law
 *
 * \sa sasfit_powerlawsq.h, sq_plugins_fractal_objects
 */
sasfit_powerlawsq_DLLEXP scalar sasfit_sq_simple_sq_power_law(scalar q, sasfit_param * p);

/**
 * \ingroup sq_simple_sq_power_law
 *
 * \sa sasfit_powerlawsq.h, sq_plugins_fractal_objects
 */
sasfit_powerlawsq_DLLEXP scalar sasfit_sq_simple_sq_power_law_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_simple_sq_power_law
 *
 * \sa sasfit_powerlawsq.h, sq_plugins_fractal_objects
 */
sasfit_powerlawsq_DLLEXP scalar sasfit_sq_simple_sq_power_law_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_simple_sq_power_law ################ */


#endif // this file

