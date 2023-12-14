/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_RIBBON_H
#define SASFIT_PLUGIN_RIBBON_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_ribbon.h
 * Public available functions and descriptions of the ribbon plugin.
 */

/**
 * \def sasfit_ribbon_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_ribbon_EXPORTS)
	#ifdef sasfit_ribbon_DLLEXP
	#undef sasfit_ribbon_DLLEXP
	#endif
	#define sasfit_ribbon_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_ribbon_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_ribbon_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start peaks_ribbon_w_sd ################ */
/** 
 * \defgroup peaks_ribbon_w_sd ribbon_w_sd
 * \ingroup ff_plugins_polyhedra
 *
 * \brief \<some brief description of ribbon_w_sd function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b B</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b sigma_B</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b C</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup peaks_ribbon_w_sd
 *
 * \sa sasfit_ribbon.h, ff_plugins_polyhedra
 */
sasfit_ribbon_DLLEXP scalar sasfit_peaks_ribbon_w_sd(scalar q, sasfit_param * p);

/**
 * \ingroup peaks_ribbon_w_sd
 *
 * \sa sasfit_ribbon.h, ff_plugins_polyhedra
 */
sasfit_ribbon_DLLEXP scalar sasfit_peaks_ribbon_w_sd_f(scalar q, sasfit_param * p);

/**
 * \ingroup peaks_ribbon_w_sd
 *
 * \sa sasfit_ribbon.h, ff_plugins_polyhedra
 */
sasfit_ribbon_DLLEXP scalar sasfit_peaks_ribbon_w_sd_v(scalar q, sasfit_param * p, int dist);
/* ################ stop peaks_ribbon_w_sd ################ */

/* ################ start ff_ribbon_mono ################ */
/** 
 * \defgroup ff_ribbon_mono ribbon_mono
 * \ingroup ff_plugins_polyhedra
 *
 * \brief \<some brief description of ribbon_mono function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b B</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b C</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ribbon_mono
 *
 * \sa sasfit_ribbon.h, ff_plugins_polyhedra
 */
sasfit_ribbon_DLLEXP scalar sasfit_ff_ribbon_mono(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ribbon_mono
 *
 * \sa sasfit_ribbon.h, ff_plugins_polyhedra
 */
sasfit_ribbon_DLLEXP scalar sasfit_ff_ribbon_mono_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ribbon_mono
 *
 * \sa sasfit_ribbon.h, ff_plugins_polyhedra
 */
sasfit_ribbon_DLLEXP scalar sasfit_ff_ribbon_mono_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ribbon_mono ################ */


#endif // this file

