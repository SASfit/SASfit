/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_SESANS_H
#define SASFIT_PLUGIN_SESANS_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_sesans.h
 * Public available functions and descriptions of the sesans plugin.
 */

/**
 * \def sasfit_sesans_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_sesans_EXPORTS)
	#ifdef sasfit_sesans_DLLEXP
	#undef sasfit_sesans_DLLEXP
	#endif
	#define sasfit_sesans_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_sesans_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_sesans_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_gz_sphere ################ */
/**
 * \defgroup ff_gz_sphere G_Sphere(delta)
 * \ingroup ff_sesans
 *
 * \brief \< some brief description of G_Sphere(delta) function \>
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
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_gz_sphere
 *
 * \sa sasfit_sesans.h, ff_sesans
 */
sasfit_sesans_DLLEXP scalar sasfit_ff_gz_sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gz_sphere
 *
 * \sa sasfit_sesans.h, ff_sesans
 */
sasfit_sesans_DLLEXP scalar sasfit_ff_gz_sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gz_sphere
 *
 * \sa sasfit_sesans.h, ff_sesans
 */
sasfit_sesans_DLLEXP scalar sasfit_ff_gz_sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_gz_sphere ################ */

/* ################ start ff_gz_dab ################ */
/**
 * \defgroup ff_gz_dab G_DAB(delta)
 * \ingroup ff_sesans
 *
 * \brief \<some brief description of G_DAB(delta) function \>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b xi</td>
 *       <td>correlation length</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_gz_dab
 *
 * \sa sasfit_sesans.h, ff_sesans
 */
sasfit_sesans_DLLEXP scalar sasfit_ff_gz_dab(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gz_dab
 *
 * \sa sasfit_sesans.h, ff_sesans
 */
sasfit_sesans_DLLEXP scalar sasfit_ff_gz_dab_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gz_dab
 *
 * \sa sasfit_sesans.h, ff_sesans
 */
sasfit_sesans_DLLEXP scalar sasfit_ff_gz_dab_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_gz_dab ################ */

/* ################ start ff_gz_generalized_dab ################ */
/**
 * \defgroup ff_gz_generalized_dab G_gDAB(delta)
 * \ingroup ff_sesans
 *
 * \brief \<some brief description of G_gDAB(delta) \>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a </td>
 *       <td>correlation length</td>
 *      </tr><tr>
 *       <td>\b H</td>
 *       <td>Hurst exponent</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_gz_generalized_dab
 *
 * \sa sasfit_sesans.h, ff_sesans
 */
sasfit_sesans_DLLEXP scalar sasfit_ff_gz_generalized_dab(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gz_generalized_dab
 *
 * \sa sasfit_sesans.h, ff_sesans
 */
sasfit_sesans_DLLEXP scalar sasfit_ff_gz_generalized_dab_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gz_generalized_dab
 *
 * \sa sasfit_sesans.h, ff_sesans
 */
sasfit_sesans_DLLEXP scalar sasfit_ff_gz_generalized_dab_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_gz_generalized_dab ################ */

/* ################ start ff_gz_ggc ################ */
/**
 * \defgroup ff_gz_ggc G_gGc(delta)
 * \ingroup ff_sesans
 *
 * \brief \<some brief description of G_gGc(delta) (generalized Gaussian coil) \>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg </td>
 *       <td>radius of gyration</td>
 *      </tr><tr>
 *       <td>\b nu</td>
 *       <td>Flory exponent (should be between 0 and 1/2)</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b beta^2</td>
 *       <td>forward scattering or squared excess scattering length</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_gz_ggc
 *
 * \sa sasfit_sesans.h, ff_sesans
 */
sasfit_sesans_DLLEXP scalar sasfit_ff_gz_ggc(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gz_ggc
 *
 * \sa sasfit_sesans.h, ff_sesans
 */
sasfit_sesans_DLLEXP scalar sasfit_ff_gz_ggc_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gz_ggc
 *
 * \sa sasfit_sesans.h, ff_sesans
 */
sasfit_sesans_DLLEXP scalar sasfit_ff_gz_ggc_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_gz_ggc ################ */
#endif // this file

