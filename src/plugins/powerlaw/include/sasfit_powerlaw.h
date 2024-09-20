/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_POWERLAW_H
#define SASFIT_PLUGIN_POWERLAW_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_powerlaw.h
 * Public available functions and descriptions of the powerlaw plugin.
 */

/**
 * \def sasfit_powerlaw_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_powerlaw_EXPORTS)
	#ifdef sasfit_powerlaw_DLLEXP
	#undef sasfit_powerlaw_DLLEXP
	#endif
	#define sasfit_powerlaw_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_powerlaw_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_powerlaw_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_profile__power_law ################ */
/**
 * \defgroup ff_profile__power_law profile: power law Sphere
 * \ingroup ff_plugins_fuzzy_profiles
 *
 * \brief \<some brief description of profile: power law function\>
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
 *       <td>power law in radial profile</td>
 *      </tr><tr>
 *       <td>\b Deta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_profile__power_law
 *
 * \sa sasfit_powerlaw.h, ff_plugins_fuzzy_sph_profiles
 */
sasfit_powerlaw_DLLEXP scalar sasfit_ff_profile__power_law(scalar q, sasfit_param * p);

/**
 * \ingroup ff_profile__power_law
 *
 * \sa sasfit_powerlaw.h, ff_plugins_fuzzy_sph_profiles
 */
sasfit_powerlaw_DLLEXP scalar sasfit_ff_profile__power_law_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_profile__power_law
 *
 * \sa sasfit_powerlaw.h, ff_plugins_fuzzy_sph_profiles
 */
sasfit_powerlaw_DLLEXP scalar sasfit_ff_profile__power_law_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_profile__power_law ################ */

/* ################ start ff_power_law ################ */
/**
 * \defgroup ff_power_law power law Sphere
 * \ingroup ff_plugins_fuzzy_sph
 *
 * \brief \<some brief description of power law Sphere function\>
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
 *       <td>power law in radial profile</td>
 *      </tr><tr>
 *       <td>\b Deta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_power_law
 *
 * \sa sasfit_powerlaw.h, ff_plugins_fuzzy_sph
 */
sasfit_powerlaw_DLLEXP scalar sasfit_ff_power_law(scalar q, sasfit_param * p);

/**
 * \ingroup ff_power_law
 *
 * \sa sasfit_powerlaw.h, ff_plugins_fuzzy_sph
 */
sasfit_powerlaw_DLLEXP scalar sasfit_ff_power_law_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_power_law
 *
 * \sa sasfit_powerlaw.h, ff_plugins_fuzzy_sph
 */
sasfit_powerlaw_DLLEXP scalar sasfit_ff_power_law_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_power_law ################ */


#endif // this file

