#ifndef HEADER_769512F2DA484E52
#define HEADER_769512F2DA484E52

/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_METALOG_H
#define SASFIT_PLUGIN_METALOG_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_metalog.h
 * Public available functions and descriptions of the metalog plugin.
 */

/**
 * \def sasfit_metalog_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_metalog_EXPORTS)
	#ifdef sasfit_metalog_DLLEXP
	#undef sasfit_metalog_DLLEXP
	#endif
	#define sasfit_metalog_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_metalog_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_metalog_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_metalog_sphere ################ */
/**
 * \defgroup ff_metalog_sphere metalog (clipped) sd of sph
 * \ingroup ff_plugins_sd_spheres
 *
 * \brief \<some brief description of metalog Sphere function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b bl</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>number, volume, intensity distribution (alpha:0,3,6)</td>
 *      </tr><tr>
 *       <td>\b a1</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a5</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a6</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a7</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a8</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a9</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a10</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_metalog_sphere
 *
 * \sa sasfit_metalog.h, ff_plugins_sd_spheres
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_sphere
 *
 * \sa sasfit_metalog.h, ff_plugins_sd_spheres
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_sphere
 *
 * \sa sasfit_metalog.h, ff_plugins_sd_spheres
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_metalog_sphere ################ */

/* ################ start ff_metaloglog_sphere ################ */
/**
 * \defgroup ff_metaloglog_sphere metalogLog sd of sph
 * \ingroup ff_plugins_sd_spheres
 *
 * \brief \<some brief description of metalogLog Sphere function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b bl</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>number, volume, intensity distribution (alpha:0,3,6)</td>
 *      </tr><tr>
 *       <td>\b a1</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a5</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a6</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a7</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a8</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a9</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a10</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_metaloglog_sphere
 *
 * \sa sasfit_metalog.h, ff_plugins_sd_spheres
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metaloglog_sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metaloglog_sphere
 *
 * \sa sasfit_metalog.h, ff_plugins_sd_spheres
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metaloglog_sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metaloglog_sphere
 *
 * \sa sasfit_metalog.h, ff_plugins_sd_spheres
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metaloglog_sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_metaloglog_sphere ################ */


/* ################ start ff_metaloglogit_sphere ################ */
/**
 * \defgroup ff_metaloglogit_sphere metalogLogit sd of sph
 * \ingroup ff_plugins_sd_spheres
 *
 * \brief \<some brief description of metalogLogit Sphere function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b bl</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>number, volume, intensity distribution (alpha:0,3,6)</td>
 *      </tr><tr>
 *       <td>\b a1</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a5</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a6</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a7</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a8</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a9</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a10</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_metaloglogit_sphere
 *
 * \sa sasfit_metalog.h, ff_plugins_sd_spheres
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metaloglogit_sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metaloglogit_sphere
 *
 * \sa sasfit_metalog.h, ff_plugins_sd_spheres
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metaloglogit_sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metaloglogit_sphere
 *
 * \sa sasfit_metalog.h, ff_plugins_sd_spheres
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metaloglogit_sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_metaloglogit_sphere ################ */

/* ################ start sd_metalog_log_clipped ################ */
/**
 * \defgroup sd_metalog_log_clipped metalog_log_clipped
 * \ingroup sd_plugins_metalog
 *
 * \brief \<some brief description of metalog_log_clipped function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b bl</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>number, volume, intensity distribution (alpha:0,3,6)</td>
 *      </tr><tr>
 *       <td>\b a1</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a5</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a6</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a7</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a8</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a9</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a10</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sd_metalog_log_clipped
 *
 * \sa sasfit_metalog.h, sd_plugins_metalog
 */
sasfit_metalog_DLLEXP scalar sasfit_sd_metalog_log_clipped(scalar q, sasfit_param * p);

/**
 * \ingroup sd_metalog_log_clipped
 *
 * \sa sasfit_metalog.h, sd_plugins_metalog
 */
sasfit_metalog_DLLEXP scalar sasfit_sd_metalog_log_clipped_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_metalog_log_clipped
 *
 * \sa sasfit_metalog.h, sd_plugins_metalog
 */
sasfit_metalog_DLLEXP scalar sasfit_sd_metalog_log_clipped_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_metalog_log_clipped ################ */

/* ################ start sd_metalog_clipped ################ */
/**
 * \defgroup sd_metalog_clipped metalog_clipped
 * \ingroup sd_plugins_metalog
 *
 * \brief \<some brief description of metalog_clipped function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>scaling factor</td>
 *      </tr><tr>
 *       <td>\b bl</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>number, volume, intensity distribution (alpha:0,3,6)</td>
 *      </tr><tr>
 *       <td>\b a1</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a5</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a6</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a7</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a8</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a9</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a10</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sd_metalog_clipped
 *
 * \sa sasfit_metalog.h, sd_plugins_metalog
 */
sasfit_metalog_DLLEXP scalar sasfit_sd_metalog_clipped(scalar q, sasfit_param * p);

/**
 * \ingroup sd_metalog_clipped
 *
 * \sa sasfit_metalog.h, sd_plugins_metalog
 */
sasfit_metalog_DLLEXP scalar sasfit_sd_metalog_clipped_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_metalog_clipped
 *
 * \sa sasfit_metalog.h, sd_plugins_metalog
 */
sasfit_metalog_DLLEXP scalar sasfit_sd_metalog_clipped_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_metalog_clipped ################ */

/* ################ start sd_metalog_logit ################ */
/**
 * \defgroup sd_metalog_logit metalog_logit
 * \ingroup sd_plugins_metalog
 *
 * \brief \<some brief description of metalog_logit function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b bl</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>number, volume, intensity distribution (alpha:0,3,6)</td>
 *      </tr><tr>
 *       <td>\b a1</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a5</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a6</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a7</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a8</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a9</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a10</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sd_metalog_logit
 *
 * \sa sasfit_metalog.h, sd_plugins_metalog
 */
sasfit_metalog_DLLEXP scalar sasfit_sd_metalog_logit(scalar q, sasfit_param * p);

/**
 * \ingroup sd_metalog_logit
 *
 * \sa sasfit_metalog.h, sd_plugins_metalog
 */
sasfit_metalog_DLLEXP scalar sasfit_sd_metalog_logit_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_metalog_logit
 *
 * \sa sasfit_metalog.h, sd_plugins_metalog
 */
sasfit_metalog_DLLEXP scalar sasfit_sd_metalog_logit_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_metalog_logit ################ */

/* ################ start ff_metalog_clipped_profile_sphere ################ */
/**
 * \defgroup ff_metalog_clipped_profile_sphere metalog (clipped) sph-xs
 * \ingroup ff_plugins_fuzzy_sph
 *
 * \brief some brief description of metalog_clipped profile for Sphere function
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper limit</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b a1</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a5</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a6</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a7</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a8</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a9</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a10</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_metalog_clipped_profile_sphere
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_sph
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_clipped_profile_sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_clipped_profile_sphere
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_sph
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_clipped_profile_sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_clipped_profile_sphere
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_sph
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_clipped_profile_sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_metalog_clipped_profile_sphere ################ */

/* ################ start ff_metalog_clipped_profile_PcsCyl ################ */
/**
 * \defgroup ff_metalog_clipped_profile_PcsCyl Pcs:metalog (clipped) cyl-xs
 * \ingroup ff_plugins_fuzzy_cyl
 *
 * \brief some brief description of Pcs:metalog_clipped profile for cylindrical xs
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper limit</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b a1</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a5</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a6</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a7</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a8</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a9</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a10</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_metalog_clipped_profile_PcsCyl
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_cyl
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_clipped_profile_PcsCyl(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_clipped_profile_PcsCyl
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_cyl
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_clipped_profile_PcsCyl_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_clipped_profile_PcsCyl
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_cyl
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_clipped_profile_PcsCyl_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_metalog_clipped_profile_PcsCyl ################ */

/* ################ start ff_metalog_clipped_profile_PcsPlanar ################ */
/**
 * \defgroup ff_metalog_clipped_profile_PcsPlanar Pcs:metalog (clipped) planar-xs
 * \ingroup ff_plugins_fuzzy_planar
 *
 * \brief some brief description of Pcs:metalog_clipped profile for planar x-section
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper limit</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b a1</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a5</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a6</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a7</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a8</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a9</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a10</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_metalog_clipped_profile_PcsPlanar
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_planar
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_clipped_profile_PcsPlanar(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_clipped_profile_PcsPlanar
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_planar
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_clipped_profile_PcsPlanar_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_clipped_profile_PcsPlanar
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_planar
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_clipped_profile_PcsPlanar_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_metalog_clipped_profile_PcsPlanar ################ */


/* ################ start ff_metalog_logit_profile_sphere ################ */
/**
 * \defgroup ff_metalog_logit_profile_sphere metalog_logit sph-xs
 * \ingroup ff_plugins_fuzzy_sph
 *
 * \brief metalog_logit x-section profile for fuzzy spheres
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper limit</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b a1</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a5</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a6</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a7</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a8</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a9</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a10</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_metalog_logit_profile_sphere
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_sph
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_logit_profile_sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_logit_profile_sphere
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_sph
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_logit_profile_sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_logit_profile_sphere
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_sph
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_logit_profile_sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_metalog_logit_profile_sphere ################ */

/* ################ start ff_metalog_logit_profile_PcsCyl ################ */
/**
 * \defgroup ff_metalog_logit_profile_PcsCyl Pcs:metalog_logit cyl-xs
 * \ingroup ff_plugins_fuzzy_cyl
 *
 * \brief metalog_logit profile for cyl-xs
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper limit</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b a1</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a5</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a6</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a7</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a8</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a9</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a10</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_metalog_logit_profile_PcsCyl
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_cyl
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_logit_profile_PcsCyl(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_logit_profile_PcsCyl
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_cyl
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_logit_profile_PcsCyl_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_logit_profile_PcsCyl
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_cyl
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_logit_profile_PcsCyl_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_metalog_logit_profile_PcsCyl ################ */

/* ################ start ff_metalog_logit_profile_PcsPlanar################ */
/**
 * \defgroup ff_metalog_logit_profile_PcsPlanar Pcs:metalog_logit planar-xs
 * \ingroup ff_plugins_fuzzy_planar
 *
 * \brief metalog_logit PcsPlanar
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper limit</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b a1</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a5</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a6</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a7</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a8</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a9</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a10</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_metalog_logit_profile_PcsPlanar
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_planar
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_logit_profile_PcsPlanar(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_logit_profile_PcsPlanar
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_planar
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_logit_profile_PcsPlanar_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_logit_profile_PcsPlanar
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_planar
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_logit_profile_PcsPlanar_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_metalog_logit_profile_PcsPlanar ################ */


/* ################ start ff_metalog_log_profile_sphere ################ */
/**
 * \defgroup ff_metalog_log_profile_sphere metalog_log sph-xs
 * \ingroup ff_plugins_fuzzy_sph
 *
 * \brief some brief description of metalog_log profile for Sphere function
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper limit</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b a1</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a5</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a6</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a7</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a8</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a9</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a10</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_metalog_log_profile_sphere
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_sph
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_log_profile_sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_log_profile_sphere
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_sph
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_log_profile_sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_log_profile_sphere
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_sph
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_log_profile_sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_metalog_log_profile_sphere ################ */

/* ################ start ff_metalog_log_profile_PcsCyl ################ */
/**
 * \defgroup ff_metalog_log_profile_PcsCyl Pcs:metalog_log cyl-xs
 * \ingroup ff_plugins_fuzzy_cyl
 *
 * \brief some brief description of metalog_log profile for Sphere function
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper limit</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b a1</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a5</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a6</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a7</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a8</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a9</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a10</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_metalog_log_profile_PcsCyl
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_cyl
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_log_profile_PcsCyl(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_log_profile_PcsCyl
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_cyl
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_log_profile_PcsCyl_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_log_profile_PcsCyl
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_cyl
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_log_profile_PcsCyl_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_metalog_log_profile_PcsCyl ################ */

/* ################ start ff_metalog_log_profile_PcsPlanar ################ */
/**
 * \defgroup ff_metalog_log_profile_PcsPlanar Pcs:metalog_log Planar-xs
 * \ingroup ff_plugins_fuzzy_planar
 *
 * \brief some brief description of metalog_log profile for PcsPlanar function
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper limit</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b a1</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a5</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a6</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a7</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a8</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a9</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a10</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_metalog_log_profile_PcsPlanar
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_planar
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_log_profile_PcsPlanar(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_log_profile_PcsPlanar
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_planar
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_log_profile_PcsPlanar_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_log_profile_PcsPlanar
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_planar
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_log_profile_PcsPlanar_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_metalog_log_profile_PcsPlanar ################ */


/* ################ start ff_metalog_clipped_profile ################ */
/**
 * \defgroup ff_metalog_clipped_profile metalog_clipped SLD-profile
 * \ingroup ff_plugins_fuzzy_profiles
 *
 * \brief some brief description of metalog_clipped SLD-profile function
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper limit</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b a1</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a5</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a6</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a7</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a8</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a9</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a10</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_metalog_clipped_profile
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_profiles
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_clipped_profile(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_clipped_profile
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_profiles
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_clipped_profile_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_clipped_profile
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_profiles
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_clipped_profile_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_metalog_clipped_profile_sphere ################ */


/* ################ start ff_metalog_logit_profile ################ */
/**
 * \defgroup ff_metalog_logit_profile metalog_logit SLD-profile
 * \ingroup ff_plugins_fuzzy_profiles
 *
 * \brief some brief description of metalog_logit SLD-profile function
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper limit</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b a1</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a5</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a6</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a7</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a8</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a9</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a10</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_metalog_logit_profile
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_profiles
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_logit_profile(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_logit_profile
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_profiles
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_logit_profile_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_logit_profile
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_profiles
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_logit_profile_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_metalog_logit_profile ################ */


/* ################ start ff_metalog_log_profile ################ */
/**
 * \defgroup ff_metalog_log_profile metalog_log SLD-profile
 * \ingroup ff_plugins_fuzzy_profiles
 *
 * \brief some brief description of metalog_log SLD-profile function
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper limit</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b a1</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a5</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a6</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a7</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a8</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a9</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a10</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_metalog_log_profile
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_profiles
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_log_profile(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_log_profile
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_profiles
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_log_profile_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metalog_log_profile_
 *
 * \sa sasfit_metalog.h, ff_plugins_fuzzy_profiles
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metalog_log_profile_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_metalog_log_profile ################ */



#endif // this file

#endif // header guard

