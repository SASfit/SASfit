/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_TRIAX_ELLIP_SHELL_H
#define SASFIT_PLUGIN_TRIAX_ELLIP_SHELL_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_triax_ellip_shell.h
 * Public available functions and descriptions of the triax_ellip_shell plugin.
 */

/**
 * \def sasfit_triax_ellip_shell_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_triax_ellip_shell_EXPORTS) 
	#ifdef sasfit_triax_ellip_shell_DLLEXP
	#undef sasfit_triax_ellip_shell_DLLEXP
	#endif
	#define sasfit_triax_ellip_shell_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_triax_ellip_shell_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_triax_ellip_shell_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_triax_ellip_shell ################ */
/** 
 * \defgroup ff_triax_ellip_shell triax ellip shell
 * \ingroup ff_plugins
 *
 * \brief \<some brief description of triax ellip shell function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>semi-axis</td>
 *      </tr><tr>
 *       <td>\b b</td>
 *       <td>semi-axis</td>
 *      </tr><tr>
 *       <td>\b c</td>
 *       <td>semi-axis</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b eta_c</td>
 *       <td>core scattering length density</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>shell scattering length density</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>solvent scattering length density</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_triax_ellip_shell
 *
 * \sa sasfit_triax_ellip_shell.h, ff_plugins
 */
sasfit_triax_ellip_shell_DLLEXP scalar sasfit_ff_triax_ellip_shell(scalar q, sasfit_param * p);

/**
 * \ingroup ff_triax_ellip_shell
 *
 * \sa sasfit_triax_ellip_shell.h, ff_plugins
 */
sasfit_triax_ellip_shell_DLLEXP scalar sasfit_ff_triax_ellip_shell_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_triax_ellip_shell
 *
 * \sa sasfit_triax_ellip_shell.h, ff_plugins
 */
sasfit_triax_ellip_shell_DLLEXP scalar sasfit_ff_triax_ellip_shell_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_triax_ellip_shell ################ */

/* ################ start ff_triax_ellip_shell_syl ################ */
/** 
 * \defgroup ff_triax_ellip_shell_syl triax ellip shell (sylvain)
 * \ingroup ff_plugins
 *
 * \brief \<some brief description of triax ellip shell function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>semi-axis</td>
 *      </tr><tr>
 *       <td>\b delta_b</td>
 *       <td>b = a + delta_b</td>
 *      </tr><tr>
 *       <td>\b delta_c</td>
 *       <td>c = b + delta_c</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b eta_c</td>
 *       <td>core scattering length density</td>
 *      </tr><tr>
 *       <td>\b eta_sh_dry</td>
 *       <td>scattering length density of the polar part and solvation of the shell</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>solvent scattering length density</td>
 *      </tr><tr>
 *       <td>\b phi</td>
 *       <td>volume fraction</td>
 *      </tr><tr>
 *       <td>\b v_pol_ap</td>
 *       <td>v_pol/v_ap</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_triax_ellip_shell_syl
 *
 * \sa sasfit_triax_ellip_shell.h, ff_plugins
 */
sasfit_triax_ellip_shell_DLLEXP scalar sasfit_ff_triax_ellip_shell_syl(scalar q, sasfit_param * p);

/**
 * \ingroup ff_triax_ellip_shell_syl
 *
 * \sa sasfit_triax_ellip_shell.h, ff_plugins
 */
sasfit_triax_ellip_shell_DLLEXP scalar sasfit_ff_triax_ellip_shell_syl_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_triax_ellip_shell_syl
 *
 * \sa sasfit_triax_ellip_shell.h, ff_plugins
 */
sasfit_triax_ellip_shell_DLLEXP scalar sasfit_ff_triax_ellip_shell_syl_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_triax_ellip_shell ################ */

/* ################ start ff_triax_ellip_shell_sq_baba_ahmed_1 ################ */
/** 
 * \defgroup ff_triax_ellip_shell_sq_baba_ahmed_1 triax ellip shell (incl SQ:BabaAhmed)(1)
 * \ingroup ff_plugins
 *
 * \brief \<some brief description of triax ellip shell function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>semi-axis</td>
 *      </tr><tr>
 *       <td>\b delta_b</td>
 *       <td>b = a + delta_b</td>
 *      </tr><tr>
 *       <td>\b delta_c</td>
 *       <td>c = b + delta_c</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b eta_c</td>
 *       <td>core scattering length density</td>
 *      </tr><tr>
 *       <td>\b eta_sh_dry</td>
 *       <td>scattering length density of the polar part and solvation of the shell</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>solvent scattering length density</td>
 *      </tr><tr>
 *       <td>\b phi</td>
 *       <td>volume fraction</td>
 *      </tr><tr>
 *       <td>\b v_pol_ap</td>
 *       <td>v_pol/v_ap</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>adjustment parameter</td>
 *      </tr><tr>
 *       <td>\b z_p</td>
 *       <td>charge per colloid particle in units of e0</td>
 *      </tr><tr>
 *       <td>\b epsilon_r</td>
 *       <td>relative permittivity</td>
 *      </tr><tr>
 *       <td>\b mB</td>
 *       <td>molarity of ion in [mol/l]</td>
 *      </tr><tr>
 *       <td>\b zB</td>
 *       <td>charge number of ion in units of e0</td>
 *      </tr><tr>
 *       <td>\b T</td>
 *       <td>temperature in [K]</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_triax_ellip_shell_sq_baba_ahmed_1
 *
 * \sa sasfit_triax_ellip_shell.h, ff_plugins
 */
sasfit_triax_ellip_shell_DLLEXP scalar 
sasfit_ff_triax_ellip_shell_sq_baba_ahmed_1(scalar q, sasfit_param * p);

/**
 * \ingroup ff_triax_ellip_shell_sq_baba_ahmed_1
 *
 * \sa sasfit_triax_ellip_shell.h, ff_plugins
 */
sasfit_triax_ellip_shell_DLLEXP scalar
sasfit_ff_triax_ellip_shell_sq_baba_ahmed_1_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_triax_ellip_shell_sq_baba_ahmed_1
 *
 * \sa sasfit_triax_ellip_shell.h, ff_plugins
 */
sasfit_triax_ellip_shell_DLLEXP scalar 
sasfit_ff_triax_ellip_shell_sq_baba_ahmed_1_v(scalar q, sasfit_param * p, int dist);

/* ################ stop ff_triax_ellip_shell_sq_baba_ahmed_1 ################ */

/* ################ start ff_triax_ellip_shell_sq_baba_ahmed_2 ################ */
/** 
 * \defgroup ff_triax_ellip_shell_sq_baba_ahmed_2 triax ellip shell (incl SQ:BabaAhmed)(2)
 * \ingroup ff_plugins
 *
 * \brief \<some brief description of triax ellip shell function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>semi-axis</td>
 *      </tr><tr>
 *       <td>\b delta_b</td>
 *       <td>b = a + delta_b</td>
 *      </tr><tr>
 *       <td>\b delta_c</td>
 *       <td>c = b + delta_c</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b eta_c</td>
 *       <td>core scattering length density</td>
 *      </tr><tr>
 *       <td>\b eta_sh_dry</td>
 *       <td>scattering length density of the polar part and solvation of the shell</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>solvent scattering length density</td>
 *      </tr><tr>
 *       <td>\b phi</td>
 *       <td>volume fraction</td>
 *      </tr><tr>
 *       <td>\b v_pol_ap</td>
 *       <td>v_pol/v_ap</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>adjustment parameter</td>
 *      </tr><tr>
 *       <td>\b z_p</td>
 *       <td>charge per colloid particle in units of e0</td>
 *      </tr><tr>
 *       <td>\b epsilon_r</td>
 *       <td>relative permittivity</td>
 *      </tr><tr>
 *       <td>\b mB</td>
 *       <td>molarity of ion in [mol/l]</td>
 *      </tr><tr>
 *       <td>\b zB</td>
 *       <td>charge number of ion in units of e0</td>
 *      </tr><tr>
 *       <td>\b T</td>
 *       <td>temperature in [K]</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_triax_ellip_shell_sq_baba_ahmed_2
 *
 * \sa sasfit_triax_ellip_shell.h, ff_plugins
 */
sasfit_triax_ellip_shell_DLLEXP scalar 
sasfit_ff_triax_ellip_shell_sq_baba_ahmed_2(scalar q, sasfit_param * p);

/**
 * \ingroup ff_triax_ellip_shell_sq_baba_ahmed_2
 *
 * \sa sasfit_triax_ellip_shell.h, ff_plugins
 */
sasfit_triax_ellip_shell_DLLEXP scalar
sasfit_ff_triax_ellip_shell_sq_baba_ahmed_2_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_triax_ellip_shell_sq_baba_ahmed_2
 *
 * \sa sasfit_triax_ellip_shell.h, ff_plugins
 */
sasfit_triax_ellip_shell_DLLEXP scalar 
sasfit_ff_triax_ellip_shell_sq_baba_ahmed_2_v(scalar q, sasfit_param * p, int dist);

/* ################ stop ff_triax_ellip_shell_sq_baba_ahmed_2 ################ */

#endif // this file

