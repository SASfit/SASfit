/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_PCS_FUZZY_CYL_H
#define SASFIT_PLUGIN_PCS_FUZZY_CYL_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_pcs_fuzzy_cyl.h
 * Public available functions and descriptions of the pcs_fuzzy_cyl plugin.
 */

/**
 * \def sasfit_pcs_fuzzy_cyl_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_pcs_fuzzy_cyl_EXPORTS)
	#ifdef sasfit_pcs_fuzzy_cyl_DLLEXP
	#undef sasfit_pcs_fuzzy_cyl_DLLEXP
	#endif
	#define sasfit_pcs_fuzzy_cyl_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_pcs_fuzzy_cyl_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_pcs_fuzzy_cyl_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_boucher_cyl ################ */
/**
 * \defgroup ff_boucher_cyl Pcs:Boucher cyl.
 * \ingroup ff_plugins_pcslocalcylindrical
 *
 * \brief \<some brief description of Pcs:Boucher cyl. function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R0</td>
 *       <td> most probable radius</td>
 *      </tr></tr>
 *       <td>\b sigma_R0</td>
 *       <td>width sigma_R0 of radius distribution (LogNorm)</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td> shape parameter alpha</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Delta eta</td>
 *       <td> scattering length density contrast Delta eta</td>
 *      </tr></table>

/**
 * \ingroup ff_pcs_fuzzy_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_pcslocalcylindrical
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_boucher_cyl(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_fuzzy_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_pcslocalcylindrical
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_boucher_cyl_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_fuzzy_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_pcslocalcylindrical
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_boucher_cyl_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_boucher_cyl ################ */

/* ################ start ff_profile_boucher_cyl ################ */
/**
 * \defgroup ff_profile_boucher_cyl profile:Boucher cyl.
 * \ingroup ff_plugins_pcslocalcylindrical
 *
 * \brief \<some brief description of profile:Boucher cyl. function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td> shape parameter alpha</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Delta eta</td>
 *       <td> scattering length density contrast Delta eta</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_pcs_fuzzy_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_pcslocalcylindrical
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_profile_boucher_cyl(scalar r, sasfit_param * p);

/**
 * \ingroup ff_pcs_fuzzy_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_pcslocalcylindrical
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_profile_boucher_cyl_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_fuzzy_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_pcslocalcylindrical
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_profile_boucher_cyl_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_profile_boucher_cyl ################ */

/* ################ start ff_boucher2_cyl ################ */
/**
 * \defgroup ff_boucher2_cyl Pcs:Boucher2 cyl.
 * \ingroup ff_plugins_pcslocalcylindrical
 *
 * \brief \<some brief description of Pcs:Boucher2 cyl. function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R0</td>
 *       <td> most probable radius</td>
 *      </tr></tr>
 *       <td>\b sigma_R0</td>
 *       <td>width sigma_R0 of radius distribution (LogNorm)</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td> shape parameter alpha</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Delta eta</td>
 *       <td> scattering length density contrast Delta eta</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_pcs_fuzzy_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_pcslocalcylindrical
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_boucher2_cyl(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_fuzzy_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_pcslocalcylindrical
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_boucher2_cyl_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_fuzzy_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_pcslocalcylindrical
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_boucher2_cyl_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_boucher2_cyl ################ */

/* ################ start ff_profile_boucher2_cyl ################ */
/**
 * \defgroup ff_profile_boucher2_cyl profile:Boucher2 cyl.
 * \ingroup ff_plugins_pcslocalcylindrical
 *
 * \brief \<some brief description of profile:Boucher2 cyl. function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R0</td>
 *       <td> most probable radius</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td> shape parameter alpha</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Delta eta</td>
 *       <td> scattering length density contrast Delta eta</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_pcs_fuzzy_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_pcslocalcylindrical
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_profile_boucher2_cyl(scalar r, sasfit_param * p);

/**
 * \ingroup ff_pcs_fuzzy_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_pcslocalcylindrical
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_profile_boucher2_cyl_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_fuzzy_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_pcslocalcylindrical
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_profile_boucher2_cyl_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_profile_boucher2_cyl ################ */


/* ################ start ff_fuzzy_cyl ################ */
/**
 * \defgroup ff_fuzzy_cyl Pcs:fuzzy cyl.
 * \ingroup ff_plugins_pcslocalcylindrical
 *
 * \brief \<some brief description of Pcs:fuzzy cyl. function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R0</td>
 *       <td> most probable radius</td>
 *      </tr></tr>
 *       <td>\b sigma_R0</td>
 *       <td>width sigma_R0 of radius distribution (LogNorm)</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td> width of fuzzy interface</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta_cyl</td>
 *       <td> scattering length density of cylindrical core</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td> scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_pcs_fuzzy_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_pcslocalcylindrical
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_fuzzy_cyl(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_fuzzy_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_pcslocalcylindrical
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_fuzzy_cyl_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_fuzzy_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_pcslocalcylindrical
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_fuzzy_cyl_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_fuzzy_cyl ################ */

/* ################ start ff_profile_fuzzy_cyl ################ */
/**
 * \defgroup ff_profile_fuzzy_cyl profile:fuzzy cyl.
 * \ingroup ff_plugins_pcslocalcylindrical
 *
 * \brief \<some brief description of profile:fuzzy cyl. function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td> radius</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td> width of fuzzy interface</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta_cyl</td>
 *       <td> scattering length density of cylindrical core</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td> scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_pcs_fuzzy_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_pcslocalcylindrical
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_profile_fuzzy_cyl(scalar r, sasfit_param * p);

/**
 * \ingroup ff_pcs_fuzzy_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_pcslocalcylindrical
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_profile_fuzzy_cyl_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_fuzzy_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_pcslocalcylindrical
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_profile_fuzzy_cyl_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_profile_fuzzy_cyl ################ */

/* ################ start ff_linsh_cyl ################ */
/**
 * \defgroup ff_linsh_cyl Pcs:linear shell cyl.
 * \ingroup ff_plugins_pcslocalcylindrical
 *
 * \brief \<some brief description of Pcs:linear shell cyl. function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R0</td>
 *       <td> most probable radius</td>
 *      </tr></tr>
 *       <td>\b sigma_R0</td>
 *       <td>width sigma_R0 of radius distribution (LogNorm)</td>
 *      </tr></tr>
 *       <td>\b t</td>
 *       <td>thickness of shell with linear scattering length density profile</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td> dummy</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr></tr>
 *       <td>\b eta_c</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh_i</td>
 *       <td>scattering length density at r=R</td>
 *      </tr><tr>
 *       <td>\b eta_sh_o</td>
 *       <td>scattering length density at r=R+t</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_pcs_fuzzy_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_pcslocalcylindrical
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_linsh_cyl(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_fuzzy_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_pcslocalcylindrical
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_linsh_cyl_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_fuzzy_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_pcslocalcylindrical
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_linsh_cyl_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_profile_cyl ################ */

/* ################ start ff_profile_linsh_cyl ################ */
/**
 * \defgroup ff_profile_linsh_cyl profile:linear shell cyl.
 * \ingroup ff_plugins_pcslocalcylindrical
 *
 * \brief \<some brief description of profile:linear shell cyl. function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R0</td>
 *       <td> most probable radius</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr></tr>
 *       <td>\b t</td>
 *       <td>thickness of shell with linear scattering length density profile</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td> dummy</td>
 *      </tr></tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr></tr>
 *       <td>\b eta_c</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh_i</td>
 *       <td>scattering length density at r=R</td>
 *      </tr><tr>
 *       <td>\b eta_sh_o</td>
 *       <td>scattering length density at r=R+t</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_pcs_fuzzy_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_pcslocalcylindrical
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_profile_linsh_cyl(scalar r, sasfit_param * p);

/**
 * \ingroup ff_pcs_fuzzy_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_pcslocalcylindrical
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_profile_linsh_cyl_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_fuzzy_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_pcslocalcylindrical
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_profile_linsh_cyl_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_profile_linsh_cyl ################ */


/* ################ start ff_rc_exp_sh_cyl ################ */
/**
 * \defgroup ff_rc_exp_sh_cyl Rc+Exp_Sh_Cyl
 * \ingroup ff_plugins_fuzzy_cyl
 *
 * \brief http://dx.doi.org/10.1021/ma102624b
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rc</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>shape parameter for Exp-profile</td>
 *      </tr><tr>
 *       <td>\b sigma_s</td>
 *       <td>smoothing parameter for shell/solvent transition</td>
 *      </tr><tr>
 *       <td>\b eta_c</td>
 *       <td>SLD of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh_in</td>
 *       <td>SLD of shell at core/shell boundary</td>
 *      </tr><tr>
 *       <td>\b eta_sh_out</td>
 *       <td>SLD of shell at shell/solvent boundary</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>SLD of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_rc_exp_sh_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_fuzzy_cyl
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_rc_exp_sh_cyl(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rc_exp_sh_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_fuzzy_cyl
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_rc_exp_sh_cyl_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rc_exp_sh_cyl
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_fuzzy_cyl
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_rc_exp_sh_cyl_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rc_exp_sh_cyl ################ */

/* ################ start ff_rc_exp_sh_cyl_profile ################ */
/**
 * \defgroup ff_rc_exp_sh_cyl_profile profile Rc+Exp_Sh_Cyl
 * \ingroup ff_plugins_fuzzy_profiles
 *
 * \brief http://dx.doi.org/10.1021/ma102624b
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rc</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>shape parameter for Exp-profile</td>
 *      </tr><tr>
 *       <td>\b sigma_s</td>
 *       <td>smoothing parameter for shell/solvent transition</td>
 *      </tr><tr>
 *       <td>\b eta_c</td>
 *       <td>SLD of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh_in</td>
 *       <td>SLD of shell at core/shell boundary</td>
 *      </tr><tr>
 *       <td>\b eta_sh_out</td>
 *       <td>SLD of shell at shell/solvent boundary</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>SLD of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_rc_exp_sh_cyl_profile
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_fuzzy_profiles
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_rc_exp_sh_cyl_profile(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rc_exp_sh_cyl_profile
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_fuzzy_profiles
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_rc_exp_sh_cyl_profile_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rc_exp_sh_cyl_profile
 *
 * \sa sasfit_pcs_fuzzy_cyl.h, ff_plugins_fuzzy_profiles
 */
sasfit_pcs_fuzzy_cyl_DLLEXP scalar sasfit_ff_rc_exp_sh_cyl_profile_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rc_exp_sh_cyl_profile ################ */


#endif // this file

