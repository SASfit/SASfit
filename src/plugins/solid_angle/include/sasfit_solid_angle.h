/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_SOLID_ANGLE_H
#define SASFIT_PLUGIN_SOLID_ANGLE_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_solid_angle.h
 * Public available functions and descriptions of the solid_angle plugin.
 */

/**
 * \def sasfit_solid_angle_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_solid_angle_EXPORTS)
	#ifdef sasfit_solid_angle_DLLEXP
	#undef sasfit_solid_angle_DLLEXP
	#endif
	#define sasfit_solid_angle_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_solid_angle_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_solid_angle_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq_solid_angle_correction ################ */
/**
 * \defgroup sq_solid_angle_correction solid angle correction
 * \ingroup sq_plugins_devel
 *
 * \brief \<some brief description of solid angle correction function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b lambda</td>
 *       <td>wave length</td>
 *      </tr><tr>
 *       <td>\b T0</td>
 *       <td>transmission</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_solid_angle_correction
 *
 * \sa sasfit_solid_angle.h, sq_plugins_devel
 */
sasfit_solid_angle_DLLEXP scalar sasfit_sq_solid_angle_correction(scalar q, sasfit_param * p);

/**
 * \ingroup sq_solid_angle_correction
 *
 * \sa sasfit_solid_angle.h, sq_plugins_devel
 */
sasfit_solid_angle_DLLEXP scalar sasfit_sq_solid_angle_correction_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_solid_angle_correction
 *
 * \sa sasfit_solid_angle.h, sq_plugins_devel
 */
sasfit_solid_angle_DLLEXP scalar sasfit_sq_solid_angle_correction_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_solid_angle_correction ################ */

/* ################ start sq_flat_panel_correction ################ */
/**
 * \defgroup sq_flat_panel_correction flat panel correction
 * \ingroup sq_plugins_devel
 *
 * \brief \<some brief description of flat panel correction function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b lambda</td>
 *       <td>wavelength</td>
 *      </tr><tr>
 *       <td>\b T0sample</td>
 *       <td>transmission of sample</td>
 *      </tr><tr>
 *       <td>\b T0flat</td>
 *       <td>transmission of flat scatterer</td>
 *      </tr><tr>
 *       <td>\b D_sample</td>
 *       <td>sample thickness</td>
 *      </tr><tr>
 *       <td>\b D_flat</td>
 *       <td>thickness of flat scatterer</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_flat_panel_correction
 *
 * \sa sasfit_solid_angle.h, sq_plugins_devel
 */
sasfit_solid_angle_DLLEXP scalar sasfit_sq_flat_panel_correction(scalar q, sasfit_param * p);

/**
 * \ingroup sq_flat_panel_correction
 *
 * \sa sasfit_solid_angle.h, sq_plugins_devel
 */
sasfit_solid_angle_DLLEXP scalar sasfit_sq_flat_panel_correction_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_flat_panel_correction
 *
 * \sa sasfit_solid_angle.h, sq_plugins_devel
 */
sasfit_solid_angle_DLLEXP scalar sasfit_sq_flat_panel_correction_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_flat_panel_correction ################ */


#endif // this file

