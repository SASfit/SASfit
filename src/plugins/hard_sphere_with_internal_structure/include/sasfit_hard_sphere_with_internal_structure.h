/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_HARD_SPHERE_WITH_INTERNAL_STRUCTURE_H
#define SASFIT_PLUGIN_HARD_SPHERE_WITH_INTERNAL_STRUCTURE_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_hard_sphere_with_internal_structure.h
 * Public available functions and descriptions of the hard_sphere_with_internal_structure plugin.
 */

/**
 * \def sasfit_hard_sphere_with_internal_structure_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_hard_sphere_with_internal_structure_EXPORTS)
	#ifdef sasfit_hard_sphere_with_internal_structure_DLLEXP
	#undef sasfit_hard_sphere_with_internal_structure_DLLEXP
	#endif
	#define sasfit_hard_sphere_with_internal_structure_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_hard_sphere_with_internal_structure_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_hard_sphere_with_internal_structure_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq_hard_sphere___py_3d_ ################ */
/** 
 * \defgroup sq_hard_sphere___py_3d_ Hard Sphere  (PY 3D)
 * \ingroup sq_plugins_hard_sphere
 *
 * \brief \<some brief description of Hard Sphere  (PY 3D) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>volume fraction (for R+DeltaR)</td>
 *      </tr><tr>
 *       <td>\b Delta_R</td>
 *       <td>shell thickness (hard sphere radius is total radius)</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_hard_sphere___py_3d_
 *
 * \sa sasfit_hard_sphere_with_internal_structure.h, sq_plugins_hard_sphere
 */
sasfit_hard_sphere_with_internal_structure_DLLEXP scalar sasfit_sq_hard_sphere___py_3d_(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hard_sphere___py_3d_
 *
 * \sa sasfit_hard_sphere_with_internal_structure.h, sq_plugins_hard_sphere
 */
sasfit_hard_sphere_with_internal_structure_DLLEXP scalar sasfit_sq_hard_sphere___py_3d__f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hard_sphere___py_3d_
 *
 * \sa sasfit_hard_sphere_with_internal_structure.h, sq_plugins_hard_sphere
 */
sasfit_hard_sphere_with_internal_structure_DLLEXP scalar sasfit_sq_hard_sphere___py_3d__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_hard_sphere___py_3d_ ################ */


#endif // this file

