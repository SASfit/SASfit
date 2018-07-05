/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_TWO_DIM_FLUID_H
#define SASFIT_PLUGIN_TWO_DIM_FLUID_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_two_dim_fluid.h
 * Public available functions and descriptions of the two_dim_fluid plugin.
 */

/**
 * \def sasfit_two_dim_fluid_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_two_dim_fluid_EXPORTS)
	#ifdef sasfit_two_dim_fluid_DLLEXP
	#undef sasfit_two_dim_fluid_DLLEXP
	#endif
	#define sasfit_two_dim_fluid_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_two_dim_fluid_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_two_dim_fluid_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq__d_hard_disks__rosenfeld_ ################ */
/**
 * \defgroup sq__d_hard_disks__rosenfeld_ 2D hard disks (Rosenfeld)
 * \ingroup sq_plugins_hard_sphere
 *
 * \brief \<some brief description of 2D hard disks (Rosenfeld) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius of the discs</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>surface coverage</td>
 *      </tr></table>
 */

/**
 * \ingroup sq__d_hard_disks__rosenfeld_
 *
 * \sa sasfit_two_dim_fluid.h, sq_plugins_hard_sphere
 */
sasfit_two_dim_fluid_DLLEXP scalar sasfit_sq__d_hard_disks__rosenfeld_(scalar q, sasfit_param * p);

/**
 * \ingroup sq__d_hard_disks__rosenfeld_
 *
 * \sa sasfit_two_dim_fluid.h, sq_plugins_hard_sphere
 */
sasfit_two_dim_fluid_DLLEXP scalar sasfit_sq__d_hard_disks__rosenfeld__f(scalar q, sasfit_param * p);

/**
 * \ingroup sq__d_hard_disks__rosenfeld_
 *
 * \sa sasfit_two_dim_fluid.h, sq_plugins_hard_sphere
 */
sasfit_two_dim_fluid_DLLEXP scalar sasfit_sq__d_hard_disks__rosenfeld__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq__d_hard_disks__rosenfeld_ ################ */

/* ################ start sq__d_hard_disks__guo_ ################ */
/**
 * \defgroup sq__d_hard_disks__guo_ 2D hard disks (Guo)
 * \ingroup sq_plugins_hard_sphere
 *
 * \brief \<some brief description of 2D hard disks (Guo) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius of hard disks</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>surface coverage</td>
 *      </tr></table>
 */

/**
 * \ingroup sq__d_hard_disks__guo_
 *
 * \sa sasfit_two_dim_fluid.h, sq_plugins_hard_sphere
 */
sasfit_two_dim_fluid_DLLEXP scalar sasfit_sq__d_hard_disks__guo_(scalar q, sasfit_param * p);

/**
 * \ingroup sq__d_hard_disks__guo_
 *
 * \sa sasfit_two_dim_fluid.h, sq_plugins_hard_sphere
 */
sasfit_two_dim_fluid_DLLEXP scalar sasfit_sq__d_hard_disks__guo__f(scalar q, sasfit_param * p);

/**
 * \ingroup sq__d_hard_disks__guo_
 *
 * \sa sasfit_two_dim_fluid.h, sq_plugins_hard_sphere
 */
sasfit_two_dim_fluid_DLLEXP scalar sasfit_sq__d_hard_disks__guo__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq__d_hard_disks__guo_ ################ */


#endif // this file

