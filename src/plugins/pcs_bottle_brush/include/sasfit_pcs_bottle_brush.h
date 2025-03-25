/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_PCS_BOTTLE_BRUSH_H
#define SASFIT_PLUGIN_PCS_BOTTLE_BRUSH_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_pcs_bottle_brush.h
 * Public available functions and descriptions of the pcs_bottle_brush plugin.
 */

/**
 * \def sasfit_pcs_bottle_brush_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_pcs_bottle_brush_EXPORTS)
	#ifdef sasfit_pcs_bottle_brush_DLLEXP
	#undef sasfit_pcs_bottle_brush_DLLEXP
	#endif
	#define sasfit_pcs_bottle_brush_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_pcs_bottle_brush_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_pcs_bottle_brush_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_bottlebrush_emp ################ */
/** 
 * \defgroup ff_bottlebrush_emp BottleBrush (emp)
 * \ingroup ff_plugins_fuzzy_cyl
 *
 * \brief http://dx.doi.org/10.1063/1.1860531
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b eta</td>
 *       <td>contrast</td>
 *      </tr><tr>
 *       <td>\b Rc</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b k</td>
 *       <td>power law decay (k>0)</td>
 *      </tr><tr>
 *       <td>\b Rs</td>
 *       <td>outer radius (Rs > Rc)</td>
 *      </tr><tr>
 *       <td>\b sigma_s</td>
 *       <td>transistion width</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_bottlebrush_emp
 *
 * \sa sasfit_pcs_bottle_brush.h, ff_plugins_fuzzy_cyl
 */
sasfit_pcs_bottle_brush_DLLEXP scalar sasfit_ff_bottlebrush_emp(scalar q, sasfit_param * p);

/**
 * \ingroup ff_bottlebrush_emp
 *
 * \sa sasfit_pcs_bottle_brush.h, ff_plugins_fuzzy_cyl
 */
sasfit_pcs_bottle_brush_DLLEXP scalar sasfit_ff_bottlebrush_emp_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_bottlebrush_emp
 *
 * \sa sasfit_pcs_bottle_brush.h, ff_plugins_fuzzy_cyl
 */
sasfit_pcs_bottle_brush_DLLEXP scalar sasfit_ff_bottlebrush_emp_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_bottlebrush_emp ################ */

/* ################ start ff_bottlebrush_profile_emp ################ */
/** 
 * \defgroup ff_bottlebrush_profile_emp BottleBrush profile (emp)
 * \ingroup ff_plugins_fuzzy_profiles
 *
 * \brief http://dx.doi.org/10.1063/1.1860531
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b eta</td>
 *       <td>contrast</td>
 *      </tr><tr>
 *       <td>\b Rc</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b k</td>
 *       <td>power law decay (k>0)</td>
 *      </tr><tr>
 *       <td>\b Rs</td>
 *       <td>outer radius (Rs > Rc)</td>
 *      </tr><tr>
 *       <td>\b sigma_s</td>
 *       <td>transistion width</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_bottlebrush_profile_emp
 *
 * \sa sasfit_pcs_bottle_brush.h, ff_plugins_fuzzy_profiles
 */
sasfit_pcs_bottle_brush_DLLEXP scalar sasfit_ff_bottlebrush_profile_emp(scalar q, sasfit_param * p);

/**
 * \ingroup ff_bottlebrush_profile_emp
 *
 * \sa sasfit_pcs_bottle_brush.h, ff_plugins_fuzzy_profiles
 */
sasfit_pcs_bottle_brush_DLLEXP scalar sasfit_ff_bottlebrush_profile_emp_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_bottlebrush_profile_emp
 *
 * \sa sasfit_pcs_bottle_brush.h, ff_plugins_fuzzy_profiles
 */
sasfit_pcs_bottle_brush_DLLEXP scalar sasfit_ff_bottlebrush_profile_emp_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_bottlebrush_profile_emp ################ */

/* ################ start ff_bottlebrush_sim ################ */
/** 
 * \defgroup ff_bottlebrush_sim BottleBrush (sim)
 * \ingroup ff_plugins_fuzzy_cyl
 *
 * \brief http://dx.doi.org/10.1002/mats.200700031
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b sigma_eta</td>
 *       <td>grafdting density x contrast</td>
 *      </tr><tr>
 *       <td>\b r1</td>
 *       <td>(r1 << r2)</td>
 *      </tr><tr>
 *       <td>\b x1</td>
 *       <td>(3 nu - 1) / (2 nu) = 0.5 for nu=0.5</td>
 *      </tr><tr>
 *       <td>\b r2</td>
 *       <td>(r1 << r2)</td>
 *      </tr><tr>
 *       <td>\b x2</td>
 *       <td>try 2 first</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_bottlebrush_sim
 *
 * \sa sasfit_pcs_bottle_brush.h, ff_plugins_fuzzy_cyl
 */
sasfit_pcs_bottle_brush_DLLEXP scalar sasfit_ff_bottlebrush_sim(scalar q, sasfit_param * p);

/**
 * \ingroup ff_bottlebrush_sim
 *
 * \sa sasfit_pcs_bottle_brush.h, ff_plugins_fuzzy_cyl
 */
sasfit_pcs_bottle_brush_DLLEXP scalar sasfit_ff_bottlebrush_sim_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_bottlebrush_sim
 *
 * \sa sasfit_pcs_bottle_brush.h, ff_plugins_fuzzy_cyl
 */
sasfit_pcs_bottle_brush_DLLEXP scalar sasfit_ff_bottlebrush_sim_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_bottlebrush__sim ################ */


/* ################ start ff_bottlebrush_profile_sim ################ */
/** 
 * \defgroup ff_bottlebrush_profile_sim BottleBrush profile (sim)
 * \ingroup ff_plugins_fuzzy_profiles
 *
 * \brief http://dx.doi.org/10.1002/mats.200700031
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b sigma_eta</td>
 *       <td>grafdting density x contrast</td>
 *      </tr><tr>
 *       <td>\b r1</td>
 *       <td>(r1 << r2)</td>
 *      </tr><tr>
 *       <td>\b x1</td>
 *       <td>(3 nu - 1) / (2 nu) = 0.5 for nu=0.5</td>
 *      </tr><tr>
 *       <td>\b r2</td>
 *       <td>(r1 << r2)</td>
 *      </tr><tr>
 *       <td>\b x2</td>
 *       <td>try 2 first</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_bottlebrush_profile_sim
 *
 * \sa sasfit_pcs_bottle_brush.h, ff_plugins_fuzzy_profiles
 */
sasfit_pcs_bottle_brush_DLLEXP scalar sasfit_ff_bottlebrush_profile_sim(scalar q, sasfit_param * p);

/**
 * \ingroup ff_bottlebrush_profile_sim
 *
 * \sa sasfit_pcs_bottle_brush.h, ff_plugins_fuzzy_profiles
 */
sasfit_pcs_bottle_brush_DLLEXP scalar sasfit_ff_bottlebrush_profile_sim_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_bottlebrush_profile_sim
 *
 * \sa sasfit_pcs_bottle_brush.h, ff_plugins_fuzzy_profiles
 */
sasfit_pcs_bottle_brush_DLLEXP scalar sasfit_ff_bottlebrush_profile_sim_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_bottlebrush_profile_sim ################ */



#endif // this file

