#ifndef HEADER_644A515AEDB574B2
#define HEADER_644A515AEDB574B2

/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_T_R_Y_H
#define SASFIT_PLUGIN_T_R_Y_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_t_r_y.h
 * Public available functions and descriptions of the t_r_y plugin.
 */

/**
 * \def sasfit_t_r_y_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_t_r_y_EXPORTS)
	#ifdef sasfit_t_r_y_DLLEXP
	#undef sasfit_t_r_y_DLLEXP
	#endif
	#define sasfit_t_r_y_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_t_r_y_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_t_r_y_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sd_fgld_ ################ */
/**
 * \defgroup sd_fgld_ FGLD
 * \ingroup sd_plugins_try
 *
 * \brief doi:10.1007/s40745-021-00322-3
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
 *       <td>lower boundary</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper boundary</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sd_fgld_
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_fgld_(scalar q, sasfit_param * p);

/**
 * \ingroup sd_fgld_
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_fgld__f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_fgld_
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_fgld__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_fgld_ ################ */

/* ################ start ff_fgld_profile ################ */
/**
 * \defgroup ff_fgld_profile FGLD SLD-profile
 * \ingroup ff_plugins_fuzzy_profiles
 *
 * \brief doi:10.1007/s40745-021-00322-3
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
 *       <td>lower boundary</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper boundary</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_fgld_profile
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_profiles
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_fgld_profile(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fgld_profile
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_profiles
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_fgld_profile_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fgld_profile
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_profiles
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_fgld_profile_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_fgld_profile ################ */

/* ################ start ff_fgld_profile_sphere ################ */
/**
 * \defgroup ff_fgld_profile_sphere FGLD sph-xs
 * \ingroup ff_plugins_fuzzy_sph
 *
 * \brief doi:10.1007/s40745-021-00322-3
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
 *       <td>lower boundary</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper boundary</td>
 *      </tr><tr>
 *       <td>\b r</td>
 *       <td>size scaling factor</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_fgld_profile_sphere
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_sph
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_fgld_profile_sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fgld_profile_sphere
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_sph
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_fgld_profile_sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fgld_profile_sphere
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_sph
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_fgld_profile_sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_fgld_profile_sphere ################ */

/* ################ start ff_fgld_sphere ################ */
/**
 * \defgroup ff_fgld_sphere FGLD-sd of sph.
 * \ingroup ff_plugins_sd_spheres
 *
 * \brief FGLD-sd of spheres (doi:10.1007/s40745-021-00322-3)
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
 *       <td>lower boundary</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper boundary</td>
 *      </tr><tr>
 *       <td>\b dim</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_fgld_sphere
 *
 * \sa sasfit_t_r_y.h, ff_plugins_sd_spheres
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_fgld_sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fgld_sphere
 *
 * \sa sasfit_t_r_y.h, ff_plugins_sd_spheres
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_fgld_sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fgld_sphere
 *
 * \sa sasfit_t_r_y.h, ff_plugins_sd_spheres
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_fgld_sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_fgld_sphere ################ */

/* ################ start ff_fgld_profile_PcsCyl ################ */
/**
 * \defgroup ff_fgld_profile_PcsCyl Pcs:FGLD cyl-xs
 * \ingroup ff_plugins_fuzzy_cyl
 *
 * \brief FGLD profile for PcsCyl (doi:10.1007/s40745-021-00322-3)
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
 *       <td>lower boundary</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper boundary</td>
 *      </tr><tr>
 *       <td>\b r</td>
 *       <td>size scaling factor</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_fgld_profile_PcsCyl
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_cyl
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_fgld_profile_PcsCyl(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fgld_profile_PcsCyl
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_cyl
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_fgld_profile_PcsCyl_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fgld_profile_PcsCyl
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_cyl
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_fgld_profile_PcsCyl_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_fgld_profile_PcsCyl ################ */

/* ################ start ff_fgld_profile_PcsPlanar ################ */
/**
 * \defgroup ff_fgld_profile_PcsPlanar Pcs: FGLD planar-xs
 * \ingroup ff_plugins_fuzzy_planar
 *
 * \brief FGLD profile for PcsPlanar (doi:10.1007/s40745-021-00322-3)
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
 *       <td>lower boundary</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper boundary</td>
 *      </tr><tr>
 *       <td>\b r</td>
 *       <td>size scaling factor</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_fgld_profile_PcsPlanar
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_planar
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_fgld_profile_PcsPlanar(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fgld_profile_PcsPlanar
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_planar
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_fgld_profile_PcsPlanar_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fgld_profile_PcsPlanar
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_planar
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_fgld_profile_PcsPlanar_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_fgld_profile_PcsPlanar ################ */

/* ################ start sd_egu_w_l ################ */
/**
 * \defgroup sd_egu_w_l EGu-W{L}
 * \ingroup sd_plugins_try
 *
 * \brief exponentiated Gumbel-Weibull {logistic} distribution (doi:10.1007/s40745-022-00373-0)
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
 *       <td>lower boundary</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper boundary</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b k</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b c</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sd_egu_w_l
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_egu_w_l(scalar q, sasfit_param * p);

/**
 * \ingroup sd_egu_w_l
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_egu_w_l_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_egu_w_l
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_egu_w_l_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_egu_w_l ################ */

/* ################ start ff_egu_w_l_profile ################ */
/**
 * \defgroup ff_egu_w_l_profile EGu-W{L} SLD-profile
 * \ingroup ff_plugins_fuzzy_profiles
 *
 * \brief (doi:10.1007/s40745-022-00373-0)
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
 *       <td></td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper boundary</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b k</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b c</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_egu_w_l_profile
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_profiles
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_egu_w_l_profile(scalar q, sasfit_param * p);

/**
 * \ingroup ff_egu_w_l_profile
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_profiles
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_egu_w_l_profile_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fgld_profile
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_profiles
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_egu_w_l_profile_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_egu_w_l_profile ################ */

/* ################ start ff_egu_w_l_profile_sphere ################ */
/**
 * \defgroup ff_egu_w_l_profile_sphere  EGu-W{L} sph-xs
 * \ingroup ff_plugins_fuzzy_sph
 *
 * \brief (doi:10.1007/s40745-022-00373-0)
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
 *       <td></td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper boundary</td>
 *      </tr><tr>
 *       <td>\b r</td>
 *       <td>size scaling factor</td>
 *      </tr><tr>
 *       <td>\b k</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b c</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_egu_w_l_profile_sphere
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_sph
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_egu_w_l_profile_sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_egu_w_l_profile_sphere
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_sph
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_egu_w_l_profile_sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_egu_w_l_profile_sphere
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_sph
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_egu_w_l_profile_sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_egu_w_l_profile_sphere ################ */

/* ################ start ff_egu_w_l_profile_PcsCyl ################ */
/**
 * \defgroup ff_egu_w_l_profile_PcsCyl Pcs:EGu-W{L} cyl-xs
 * \ingroup ff_plugins_fuzzy_cyl
 *
 * \brief EGu-W{L} profile for PcsCyl (doi:10.1007/s40745-022-00373-0)
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
 *       <td></td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper boundary</td>
 *      </tr><tr>
 *       <td>\b r</td>
 *       <td>size scaling factor</td>
 *      </tr><tr>
 *       <td>\b k</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b c</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_egu_w_l_profile_PcsCyl
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_cyl
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_egu_w_l_profile_PcsCyl(scalar q, sasfit_param * p);

/**
 * \ingroup ff_egu_w_l_profile_PcsCyl
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_cyl
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_egu_w_l_profile_PcsCyl_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_egu_w_l_profile_PcsCyl
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_cyl
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_egu_w_l_profile_PcsCyl_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_egu_w_l_profile_PcsCyl ################ */

/* ################ start ff_egu_w_l_profile_PcsPlanar ################ */
/**
 * \defgroup ff_egu_w_l_profile_PcsPlanar Pcs:EGu-W{L} planar-xs
 * \ingroup ff_plugins_fuzzy_planar
 *
 * \brief EGu-W{L} profile for PcsPlanar (doi:10.1007/s40745-022-00373-0)
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
 *       <td></td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper boundary</td>
 *      </tr><tr>
 *       <td>\b r</td>
 *       <td>size scaling factor</td>
 *      </tr><tr>
 *       <td>\b k</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b c</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_egu_w_l_profile_PcsPlanar
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_planar
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_egu_w_l_profile_PcsPlanar(scalar q, sasfit_param * p);

/**
 * \ingroup ff_egu_w_l_profile_PcsPlanar
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_planar
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_egu_w_l_profile_PcsPlanar_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_egu_w_l_profile_PcsPlanar
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_planar
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_egu_w_l_profile_PcsPlanar_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_egu_w_l_profile_PcsPlanar ################ */

/* ################ start ff_egu_w_l_sphere ################ */
/**
 * \defgroup ff_egu_w_l_sphere EGu-W{L}-sd of sph
 * \ingroup ff_plugins_sd_spheres
 *
 * \brief (doi:10.1007/s40745-022-00373-0)
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
 *       <td></td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper boundary</td>
 *      </tr><tr>
 *       <td>\b dim</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b k</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b c</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_egu_w_l_sphere
 *
 * \sa sasfit_t_r_y.h, ff_plugins_sd_spheres
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_egu_w_l_sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_egu_w_l_sphere
 *
 * \sa sasfit_t_r_y.h, ff_plugins_sd_spheres
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_egu_w_l_sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_egu_w_l_sphere
 *
 * \sa sasfit_t_r_y.h, ff_plugins_sd_spheres
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_egu_w_l_sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_egu_w_l_sphere ################ */

/* ################ start sd_g_c_e__ ################ */
/**
 * \defgroup sd_g_c_e__ G-C{E}
 * \ingroup sd_plugins_try
 *
 * \brief (doi:10.1186/s40488-016-0050-3)
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
 *       <td>lower boundary</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper boundary</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sd_g_c_e__
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_g_c_e__(scalar q, sasfit_param * p);

/**
 * \ingroup sd_g_c_e__
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_g_c_e___f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_g_c_e__
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_g_c_e___v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_g_c_e__ ################ */

/* ################ start sd_ll_l_gl__ ################ */
/**
 * \defgroup sd_ll_l_gl__ LL-L{GL}
 * \ingroup sd_plugins_try
 *
 * \brief (doi:10.1186/s40488-017-0081-4)
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
 *       <td>lower boundary</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper boundary</td>
 *       <td></td>
 *       <td>\b dummy</td>
 *       <td></td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td>mu element R</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>sigma>0</td>
 *      </tr><tr>
 *       <td>\b c</td>
 *       <td>c>0</td>
 *      </tr><tr>
 *       <td>\b w</td>
 *       <td>w>0</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_ll_l_gl__
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_ll_l_gl__(scalar q, sasfit_param * p);

/**
 * \ingroup sd_ll_l_gl__
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_ll_l_gl___f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_ll_l_gl__
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_ll_l_gl___v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_ll_l_gl__ ################ */

/* ################ start sd_p_w_gl__ ################ */
/**
 * \defgroup sd_p_w_gl__ P-W{GL}
 * \ingroup sd_plugins_try
 *
 * \brief (doi:10.1186/s40488-017-0081-4)
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b s</td>
 *       <td>>0</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>>0</td>
 *      </tr><tr>
 *       <td>\b c</td>
 *       <td>>0</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>>0</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_p_w_gl__
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_p_w_gl__(scalar q, sasfit_param * p);

/**
 * \ingroup sd_p_w_gl__
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_p_w_gl___f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_p_w_gl__
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_p_w_gl___v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_p_w_gl__ ################ */

/* ################ start sd_n_u_gl__ ################ */
/**
 * \defgroup sd_n_u_gl__ N-U{GL}
 * \ingroup sd_plugins_try
 *
 * \brief (doi:10.1186/s40488-017-0081-4)
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b b</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b lambda2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b lambda3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b lambda4</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sd_n_u_gl__
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_n_u_gl__(scalar q, sasfit_param * p);

/**
 * \ingroup sd_n_u_gl__
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_n_u_gl___f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_n_u_gl__
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_n_u_gl___v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_n_u_gl__ ################ */

/* ################ start sd_u_e_gl__ ################ */
/**
 * \defgroup sd_u_e_gl__ U-E{GL}
 * \ingroup sd_plugins_try
 *
 * \brief (doi:10.1186/s40488-017-0081-4)
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b lambda3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b lambda4</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sd_u_e_gl__
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_u_e_gl__(scalar q, sasfit_param * p);

/**
 * \ingroup sd_u_e_gl__
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_u_e_gl___f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_u_e_gl__
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_u_e_gl___v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_u_e_gl__ ################ */

/* ################ start sd_w_l_e_ ################ */
/**
 * \defgroup sd_w_l_e_ W-L{E}
 * \ingroup sd_plugins_try
 *
 * \brief (doi:10.1186/s40488-021-00127-y)
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
 *       <td>lower boundary</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper boundary</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sd_w_l_e_
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_w_l_e_(scalar q, sasfit_param * p);

/**
 * \ingroup sd_w_l_e_
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_w_l_e__f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_w_l_e_
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_w_l_e__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_w_l_e_ ################ */

/* ################ start ff_w_l_e_profile ################ */
/**
 * \defgroup ff_w_l_e_profile W-L{E} SLD-profile
 * \ingroup ff_plugins_fuzzy_profiles
 *
 * \brief (doi:10.1186/s40488-021-00127-y)
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
 *       <td>lower boundary</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper boundary</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_w_l_e_profile
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_profiles
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_w_l_e_profile(scalar q, sasfit_param * p);

/**
 * \ingroup ff_w_l_e_profile
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_profiles
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_w_l_e_profile_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_w_l_e_profile
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_profiles
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_w_l_e_profile_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_w_l_e_profile ################ */

/* ################ start ff_w_l_e_profile_sphere ################ */
/**
 * \defgroup ff_w_l_e_profile_sphere W-L{E} sph-xs
 * \ingroup ff_plugins_fuzzy_sph
 *
 * \brief (doi:10.1186/s40488-021-00127-y)
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
 *       <td>lower boundary</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper boundary</td>
 *      </tr><tr>
 *       <td>\b r</td>
 *       <td>size scaling factor</td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_w_l_e_profile_sphere
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_sph
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_w_l_e_profile_sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_w_l_e_profile_sphere
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_sph
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_w_l_e_profile_sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_w_l_e_profile_sphere
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_sph
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_w_l_e_profile_sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_w_l_e_profile_sphere ################ */

/* ################ start ff_w_l_e_sphere ################ */
/**
 * \defgroup ff_w_l_e_sphere W-L{E}-sd of sph.
 * \ingroup ff_plugins_sd_spheres
 *
 * \brief W-L{E}-sd of spheres (doi:10.1186/s40488-021-00127-y)
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
 *       <td>lower boundary</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper boundary</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_w_l_e_sphere
 *
 * \sa sasfit_t_r_y.h, ff_plugins_w_l_e_spheres
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_w_l_e_sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_w_l_e_sphere
 *
 * \sa sasfit_t_r_y.h, ff_plugins_w_l_e_spheres
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_w_l_e_sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_w_l_e_sphere
 *
 * \sa sasfit_t_r_y.h, ff_plugins_sd_spheres
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_w_l_e_sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_w_l_e_sphere ################ */

/* ################ start ff_w_l_e_profile_PcsCyl ################ */
/**
 * \defgroup ff_w_l_e_profile_PcsCyl Pcs:W-L{E} cyl-xs
 * \ingroup ff_plugins_fuzzy_cyl
 *
 * \brief W-L{E} profile for PcsCyl (doi:10.1186/s40488-021-00127-y)
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
 *       <td>lower boundary</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper boundary</td>
 *      </tr><tr>
 *       <td>\b r</td>
 *       <td>size scaling factor</td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_w_l_e_profile_PcsCyl
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_cyl
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_w_l_e_profile_PcsCyl(scalar q, sasfit_param * p);

/**
 * \ingroup ff_w_l_e_profile_PcsCyl
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_cyl
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_w_l_e_profile_PcsCyl_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_w_l_e_profile_PcsCyl
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_cyl
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_w_l_e_profile_PcsCyl_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_w_l_e_profile_PcsCyl ################ */

/* ################ start ff_w_l_e_profile_PcsPlanar ################ */
/**
 * \defgroup ff_w_l_e_profile_PcsPlanar Pcs: W-L{E} planar-xs
 * \ingroup ff_plugins_fuzzy_planar
 *
 * \brief W-L{E} profile for PcsPlanar (doi:10.1186/s40488-021-00127-y)
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
 *       <td>lower boundary</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper boundary</td>
 *      </tr><tr>
 *       <td>\b r</td>
 *       <td>size scaling factor</td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_w_l_e_profile_PcsPlanar
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_planar
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_w_l_e_profile_PcsPlanar(scalar q, sasfit_param * p);

/**
 * \ingroup ff_w_l_e_profile_PcsPlanar
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_planar
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_w_l_e_profile_PcsPlanar_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_w_l_e_profile_PcsPlanar
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_planar
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_w_l_e_profile_PcsPlanar_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_w_l_e_profile_PcsPlanar ################ */

/* ################ start sd_n_l_c_ ################ */
/**
 * \defgroup sd_n_l_c_ N-L{C}
 * \ingroup sd_plugins_try
 *
 * \brief https://doi.org/10.1186/s40488-021-00127-y
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
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sd_n_l_c_
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_n_l_c_(scalar q, sasfit_param * p);

/**
 * \ingroup sd_n_l_c_
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_n_l_c__f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_n_l_c_
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_n_l_c__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_n_l_c_ ################ */

/* ################ start ff_n_l_c_profile ################ */
/**
 * \defgroup ff_n_l_c_profile N-L{C} SLD-profile
 * \ingroup ff_plugins_fuzzy_profiles
 *
 * \brief https://doi.org/10.1186/s40488-021-00127-y
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
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_n_l_c_profile
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_profiles
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_n_l_c_profile(scalar q, sasfit_param * p);

/**
 * \ingroup ff_n_l_c_profile
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_profiles
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_n_l_c_profile_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_n_l_c_profile
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_profiles
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_n_l_c_profile_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_n_l_c_profile ################ */

/* ################ start ff_n_l_c_profile_sphere ################ */
/**
 * \defgroup ff_n_l_c_profile_sphere N-L{C} sph-xs
 * \ingroup ff_plugins_fuzzy_sph
 *
 * \brief (doi:10.1186/s40488-021-00127-y)
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
 *       <td>\b r</td>
 *       <td>size scaling factor</td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_n_l_c_profile_sphere
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_sph
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_n_l_c_profile_sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_n_l_c_profile_sphere
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_sph
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_n_l_c_profile_sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_n_l_c_profile_sphere
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_sph
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_n_l_c_profile_sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_n_l_c_profile_sphere ################ */

/* ################ start ff_n_l_c_sphere ################ */
/**
 * \defgroup ff_n_l_c_sphere N-L{C}-sd of sph.
 * \ingroup ff_plugins_sd_spheres
 *
 * \brief N-L{C}-sd of spheres (doi:10.1186/s40488-021-00127-y)
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
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_n_l_c_sphere
 *
 * \sa sasfit_t_r_y.h, ff_plugins_n_l_c_spheres
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_n_l_c_sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_n_l_c_sphere
 *
 * \sa sasfit_t_r_y.h, ff_plugins_n_l_c_spheres
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_n_l_c_sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_n_l_c_sphere
 *
 * \sa sasfit_t_r_y.h, ff_plugins_sd_spheres
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_n_l_c_sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_n_l_c_sphere ################ */

/* ################ start ff_n_l_c_profile_PcsCyl ################ */
/**
 * \defgroup ff_n_l_c_profile_PcsCyl Pcs:N-L{C} cyl-xs
 * \ingroup ff_plugins_fuzzy_cyl
 *
 * \brief N-L{C} profile for PcsCyl (doi:10.1186/s40488-021-00127-y)
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
 *       <td>\b r</td>
 *       <td>size scaling factor</td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_n_l_c_profile_PcsCyl
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_cyl
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_n_l_c_profile_PcsCyl(scalar q, sasfit_param * p);

/**
 * \ingroup ff_n_l_c_profile_PcsCyl
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_cyl
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_n_l_c_profile_PcsCyl_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_n_l_c_profile_PcsCyl
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_cyl
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_n_l_c_profile_PcsCyl_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_n_l_c_profile_PcsCyl ################ */

/* ################ start ff_n_l_c_profile_PcsPlanar ################ */
/**
 * \defgroup ff_n_l_c_profile_PcsPlanar Pcs: N-L{C} planar-xs
 * \ingroup ff_plugins_fuzzy_planar
 *
 * \brief N-L{C} profile for PcsPlanar (doi:10.1186/s40488-021-00127-y)
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
 *       <td>\b r</td>
 *       <td>size scaling factor</td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_n_l_c_profile_PcsPlanar
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_planar
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_n_l_c_profile_PcsPlanar(scalar q, sasfit_param * p);

/**
 * \ingroup ff_n_l_c_profile_PcsPlanar
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_planar
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_n_l_c_profile_PcsPlanar_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_n_l_c_profile_PcsPlanar
 *
 * \sa sasfit_t_r_y.h, ff_plugins_fuzzy_planar
 */
sasfit_t_r_y_DLLEXP scalar sasfit_ff_n_l_c_profile_PcsPlanar_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_n_l_c_profile_PcsPlanar ################ */

/* ################ start sd_e_l_ll_ ################ */
/**
 * \defgroup sd_e_l_ll_ E-L{LL}
 * \ingroup sd_plugins_try
 *
 * \brief https://doi.org/10.1186/s40488-021-00127-y
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
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sd_e_l_ll_
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_e_l_ll_(scalar q, sasfit_param * p);

/**
 * \ingroup sd_e_l_ll_
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_e_l_ll__f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_e_l_ll_
 *
 * \sa sasfit_t_r_y.h, sd_plugins_try
 */
sasfit_t_r_y_DLLEXP scalar sasfit_sd_e_l_ll__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_e_l_ll_ ################ */
#endif // this file

#endif // header guard

