/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_CAPPED_CYLINDER_H
#define SASFIT_PLUGIN_CAPPED_CYLINDER_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_capped_cylinder.h
 * Public available functions and descriptions of the capped_cylinder plugin.
 */

/**
 * \def sasfit_capped_cylinder_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_capped_cylinder_EXPORTS)
	#ifdef sasfit_capped_cylinder_DLLEXP
	#undef sasfit_capped_cylinder_DLLEXP
	#endif
	#define sasfit_capped_cylinder_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_capped_cylinder_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_capped_cylinder_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_capped_cylinder ################ */
/**
 * \defgroup ff_capped_cylinder capped_cylinder
 * \ingroup ff_plugins_cylindrical_obj
 *
 * \brief \<some brief description of capped_cylinder function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b r</td>
 *       <td>cylinder radius</td>
 *      </tr><tr>
 *       <td>\b h</td>
 *       <td>center offset of end cap from surface, (h<0 flat end cap, h>0 barbell end cap)</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>length of cylinder</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_capped_cylinder
 *
 * \sa sasfit_capped_cylinder.h, ff_plugins_cylindrical_obj
 */
sasfit_capped_cylinder_DLLEXP scalar sasfit_ff_capped_cylinder(scalar q, sasfit_param * p);

/**
 * \ingroup ff_capped_cylinder
 *
 * \sa sasfit_capped_cylinder.h, ff_plugins_cylindrical_obj
 */
sasfit_capped_cylinder_DLLEXP scalar sasfit_ff_capped_cylinder_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_capped_cylinder
 *
 * \sa sasfit_capped_cylinder.h, ff_plugins_cylindrical_obj
 */
sasfit_capped_cylinder_DLLEXP scalar sasfit_ff_capped_cylinder_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_capped_cylinder ################ */


#endif // this file

