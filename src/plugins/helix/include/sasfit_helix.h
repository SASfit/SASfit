/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_HELIIX_H
#define SASFIT_PLUGIN_HELIIX_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_heliix.h
 * Public available functions and descriptions of the heliix plugin.
 */

/**
 * \def sasfit_heliix_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_heliix_EXPORTS)
	#ifdef sasfit_heliix_DLLEXP
	#undef sasfit_heliix_DLLEXP
	#endif
	#define sasfit_heliix_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_heliix_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_heliix_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_pringle_schmidt_helix ################ */
/** 
 * \defgroup ff_pringle_schmidt_helix Pringle-Schmidt helix
 * \ingroup ff_plugins_helix
 *
 * \brief \<some brief description of Pringle-Schmidt helix function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>external helix radius</td>
 *      </tr><tr>
 *       <td>\b a</td>
 *       <td>0<a<1, aR : inner helix radius</td>
 *      </tr><tr>
 *       <td>\b P</td>
 *       <td>helix period</td>
 *      </tr><tr>
 *       <td>\b H</td>
 *       <td>helix length</td>
 *      </tr><tr>
 *       <td>\b omega</td>
 *       <td>angular of the sector of material</td>
 *      </tr><tr>
 *       <td>\b phi</td>
 *       <td>angle between the two sectors of a double helix</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_pringle_schmidt_helix
 *
 * \sa sasfit_heliix.h, ff_plugins_helix
 */
sasfit_heliix_DLLEXP scalar sasfit_ff_pringle_schmidt_helix(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pringle_schmidt_helix
 *
 * \sa sasfit_heliix.h, ff_plugins_helix
 */
sasfit_heliix_DLLEXP scalar sasfit_ff_pringle_schmidt_helix_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pringle_schmidt_helix
 *
 * \sa sasfit_heliix.h, ff_plugins_helix
 */
sasfit_heliix_DLLEXP scalar sasfit_ff_pringle_schmidt_helix_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_pringle_schmidt_helix ################ */

/* ################ start ff_unnamed_helix ################ */
/** 
 * \defgroup ff_unnamed_helix unnamed helix
 * \ingroup form_fac
 *
 * \brief \<some brief description of unnamed helix function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_unnamed_helix
 *
 * \sa sasfit_heliix.h, form_fac
 */
sasfit_heliix_DLLEXP scalar sasfit_ff_unnamed_helix(scalar q, sasfit_param * p);

/**
 * \ingroup ff_unnamed_helix
 *
 * \sa sasfit_heliix.h, form_fac
 */
sasfit_heliix_DLLEXP scalar sasfit_ff_unnamed_helix_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_unnamed_helix
 *
 * \sa sasfit_heliix.h, form_fac
 */
sasfit_heliix_DLLEXP scalar sasfit_ff_unnamed_helix_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_unnamed_helix ################ */

/* ################ start ff_beads_helix ################ */
/** 
 * \defgroup ff_beads_helix beads helix
 * \ingroup ff_plugins_helix
 *
 * \brief \<some brief description of beads helix function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius of beads</td>
 *      </tr><tr>
 *       <td>\b H</td>
 *       <td>pitch of helix</td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td>diameter of helix</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_beads_helix
 *
 * \sa sasfit_heliix.h, ff_plugins_helix
 */
sasfit_heliix_DLLEXP scalar sasfit_ff_beads_helix(scalar q, sasfit_param * p);

/**
 * \ingroup ff_beads_helix
 *
 * \sa sasfit_heliix.h, ff_plugins_helix
 */
sasfit_heliix_DLLEXP scalar sasfit_ff_beads_helix_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_beads_helix
 *
 * \sa sasfit_heliix.h, ff_plugins_helix
 */
sasfit_heliix_DLLEXP scalar sasfit_ff_beads_helix_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_beads_helix ################ */


#endif // this file

