/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_BOUCHERPROFILE_H
#define SASFIT_PLUGIN_BOUCHERPROFILE_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_boucherprofile.h
 * Public available functions and descriptions of the boucherprofile plugin.
 */

/**
 * \def sasfit_boucherprofile_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_boucherprofile_EXPORTS)
	#ifdef sasfit_boucherprofile_DLLEXP
	#undef sasfit_boucherprofile_DLLEXP
	#endif
	#define sasfit_boucherprofile_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_boucherprofile_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_boucherprofile_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_boucher_sphere ################ */
/** 
 * \defgroup ff_boucher_sphere Boucher Sphere
 * \ingroup ff_plugins_spheres_shells
 *
 * \brief \<some brief description of Boucher Sphere function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>Porod potential decay</td>
 *      </tr><tr>
 *       <td>\b Delta_eta</td>
 *       <td>scattering contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_boucher_sphere
 *
 * \sa sasfit_boucherprofile.h, ff_plugins_spheres_shells
 */
sasfit_boucherprofile_DLLEXP scalar sasfit_ff_boucher_sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_boucher_sphere
 *
 * \sa sasfit_boucherprofile.h, ff_plugins_spheres_shells
 */
sasfit_boucherprofile_DLLEXP scalar sasfit_ff_boucher_sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_boucher_sphere
 *
 * \sa sasfit_boucherprofile.h, ff_plugins_spheres_shells
 */
sasfit_boucherprofile_DLLEXP scalar sasfit_ff_boucher_sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_boucher_sphere ################ */

/* ################ start ff_boucher_profiles ################ */
/** 
 * \defgroup ff_boucher_profiles Boucher profiles
 * \ingroup ff_plugins_spheres_shells
 *
 * \brief \<some brief description of Boucher profiles function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>Porod potential decay</td>
 *      </tr><tr>
 *       <td>\b Delta_eta</td>
 *       <td>scattering contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_boucher_profiles
 *
 * \sa sasfit_boucherprofile.h, ff_plugins_spheres_shells
 */
sasfit_boucherprofile_DLLEXP scalar sasfit_ff_boucher_profiles(scalar q, sasfit_param * p);

/**
 * \ingroup ff_boucher_profiles
 *
 * \sa sasfit_boucherprofile.h, ff_plugins_spheres_shells
 */
sasfit_boucherprofile_DLLEXP scalar sasfit_ff_boucher_profiles_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_boucher_profiles
 *
 * \sa sasfit_boucherprofile.h, ff_plugins_spheres_shells
 */
sasfit_boucherprofile_DLLEXP scalar sasfit_ff_boucher_profiles_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_boucher_profiles ################ */


/* ################ start ff_boucher_sphere2 ################ */
/** 
 * \defgroup ff_boucher_sphere2 Boucher Sphere2
 * \ingroup ff_plugins_spheres_shells
 *
 * \brief \<some brief description of Boucher Sphere2 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>Porod potential decay</td>
 *      </tr><tr>
 *       <td>\b Delta_eta</td>
 *       <td>average scattering contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_boucher_sphere2
 *
 * \sa sasfit_boucherprofile.h, ff_plugins_spheres_shells
 */
sasfit_boucherprofile_DLLEXP scalar sasfit_ff_boucher_sphere2(scalar q, sasfit_param * p);

/**
 * \ingroup ff_boucher_sphere2
 *
 * \sa sasfit_boucherprofile.h, ff_plugins_spheres_shells
 */
sasfit_boucherprofile_DLLEXP scalar sasfit_ff_boucher_sphere2_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_boucher_sphere2
 *
 * \sa sasfit_boucherprofile.h, ff_plugins_spheres_shells
 */
sasfit_boucherprofile_DLLEXP scalar sasfit_ff_boucher_sphere2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_boucher_sphere ################ */

/* ################ start ff_boucher_profiles2 ################ */
/** 
 * \defgroup ff_boucher_profiles2 Boucher profiles2
 * \ingroup ff_plugins_spheres_shells
 *
 * \brief \<some brief description of Boucher profiles2 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>Porod potential decay</td>
 *      </tr><tr>
 *       <td>\b Delta_eta</td>
 *       <td>average scattering contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_boucher_profiles2
 *
 * \sa sasfit_boucherprofile.h, ff_plugins_spheres_shells
 */
sasfit_boucherprofile_DLLEXP scalar sasfit_ff_boucher_profiles2(scalar q, sasfit_param * p);

/**
 * \ingroup ff_boucher_profiles2
 *
 * \sa sasfit_boucherprofile.h, ff_plugins_spheres_shells
 */
sasfit_boucherprofile_DLLEXP scalar sasfit_ff_boucher_profiles2_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_boucher_profiles2
 *
 * \sa sasfit_boucherprofile.h, ff_plugins_spheres_shells
 */
sasfit_boucherprofile_DLLEXP scalar sasfit_ff_boucher_profiles2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_boucher_profiles2 ################ */

#endif // this file

