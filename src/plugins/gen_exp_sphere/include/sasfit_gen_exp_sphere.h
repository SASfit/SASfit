/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_GEN_EXP_SPHERE_H
#define SASFIT_PLUGIN_GEN_EXP_SPHERE_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_gen_exp_sphere.h
 * Public available functions and descriptions of the gen_exp_sphere plugin.
 */

/**
 * \def sasfit_gen_exp_sphere_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_gen_exp_sphere_EXPORTS)
	#ifdef sasfit_gen_exp_sphere_DLLEXP
	#undef sasfit_gen_exp_sphere_DLLEXP
	#endif
	#define sasfit_gen_exp_sphere_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_gen_exp_sphere_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_gen_exp_sphere_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_profile__gen__exp__sphere ################ */
/**
 * \defgroup ff_profile__gen__exp__sphere profile: Gen. Exp. Sphere
 * \ingroup ff_plugins_fuzzy_profiles
 *
 * \brief \<some brief description of profile: Gen. Exp. Sphere function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>shape parameter of radial profile</td>
 *      </tr><tr>
 *       <td>\b Delta eta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_profile__gen__exp__sphere
 *
 * \sa sasfit_gen_exp_sphere.h, ff_plugins_fuzzy_profiles
 */
sasfit_gen_exp_sphere_DLLEXP scalar sasfit_ff_profile__gen__exp__sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_profile__gen__exp__sphere
 *
 * \sa sasfit_gen_exp_sphere.h, ff_plugins_fuzzy_profiles
 */
sasfit_gen_exp_sphere_DLLEXP scalar sasfit_ff_profile__gen__exp__sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_profile__gen__exp__sphere
 *
 * \sa sasfit_gen_exp_sphere.h, ff_plugins_fuzzy_profiles
 */
sasfit_gen_exp_sphere_DLLEXP scalar sasfit_ff_profile__gen__exp__sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_profile__gen__exp__sphere ################ */

/* ################ start ff_gen__exp__sphere ################ */
/**
 * \defgroup ff_gen__exp__sphere Gen. Exp. Sphere
 * \ingroup ff_plugins_fuzzy_sph
 *
 * \brief \<some brief description of Gen. Exp. Sphere function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>shape parameter of radial profile</td>
 *      </tr><tr>
 *       <td>\b Delta eta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_gen__exp__sphere
 *
 * \sa sasfit_gen_exp_sphere.h, ff_plugins_fuzzy_sph
 */
sasfit_gen_exp_sphere_DLLEXP scalar sasfit_ff_gen__exp__sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gen__exp__sphere
 *
 * \sa sasfit_gen_exp_sphere.h, ff_plugins_fuzzy_sph
 */
sasfit_gen_exp_sphere_DLLEXP scalar sasfit_ff_gen__exp__sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gen__exp__sphere
 *
 * \sa sasfit_gen_exp_sphere.h, ff_plugins_fuzzy_sph
 */
sasfit_gen_exp_sphere_DLLEXP scalar sasfit_ff_gen__exp__sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_gen__exp__sphere ################ */


/* ################ start ff_profile__gen__gauss__sphere ################ */
/**
 * \defgroup ff_profile__gen__gauss__sphere profile: Gen. Gauss Sphere
 * \ingroup ff_plugins_fuzzy_profiles
 *
 * \brief \<some brief description of profile: Gen. Gauss Sphere function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b Delta eta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_profile__gen__gauss__sphere
 *
 * \sa sasfit_gen_exp_sphere.h, ff_plugins_fuzzy_profiles
 */
sasfit_gen_exp_sphere_DLLEXP scalar sasfit_ff_profile__gen__gauss__sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_profile__gen__gauss__sphere
 *
 * \sa sasfit_gen_exp_sphere.h, ff_plugins_fuzzy_profiles
 */
sasfit_gen_exp_sphere_DLLEXP scalar sasfit_ff_profile__gen__gauss__sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_profile__gen__gauss__sphere
 *
 * \sa sasfit_gen_exp_sphere.h, ff_plugins_fuzzy_profiles
 */
sasfit_gen_exp_sphere_DLLEXP scalar sasfit_ff_profile__gen__gauss__sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_profile__gen__gauss__sphere ################ */

/* ################ start ff_gen__gauss__sphere ################ */
/**
 * \defgroup ff_gen__gauss__sphere Gen. Gauss Sphere
 * \ingroup ff_plugins_fuzzy_sph
 *
 * \brief \<some brief description of Gen. Gauss Sphere function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>shape parameter</td>
 *      </tr><tr>
 *       <td>\b Delta eta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_gen__gauss__sphere
 *
 * \sa sasfit_gen_exp_sphere.h, ff_plugins_fuzzy_sph
 */
sasfit_gen_exp_sphere_DLLEXP scalar sasfit_ff_gen__gauss__sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gen__gauss__sphere
 *
 * \sa sasfit_gen_exp_sphere.h, ff_plugins_fuzzy_sph
 */
sasfit_gen_exp_sphere_DLLEXP scalar sasfit_ff_gen__gauss__sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gen__gauss__sphere
 *
 * \sa sasfit_gen_exp_sphere.h, ff_plugins_fuzzy_sph
 */
sasfit_gen_exp_sphere_DLLEXP scalar sasfit_ff_gen__gauss__sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_gen__gauss__sphere ################ */


#endif // this file

