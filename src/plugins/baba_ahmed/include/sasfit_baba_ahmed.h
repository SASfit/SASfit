/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_BABA_AHMED_H
#define SASFIT_PLUGIN_BABA_AHMED_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_baba_ahmed.h
 * Public available functions and descriptions of the baba_ahmed plugin.
 */

/**
 * \def sasfit_baba_ahmed_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_baba_ahmed_EXPORTS) 
	#ifdef sasfit_baba_ahmed_DLLEXP
	#undef sasfit_baba_ahmed_DLLEXP
	#endif
	#define sasfit_baba_ahmed_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_baba_ahmed_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_baba_ahmed_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style


/* ################ start sq_baba_ahmed_1 ################ */
/** 
 * \defgroup sq_baba_ahmed_1 Baba Ahmed 1
 * \ingroup sq_plugins
 *
 * \brief \<some brief description of Baba Ahmed 1 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b D_eff</td>
 *       <td>effective diameter in [nm]</td>
 *      </tr><tr>
 *       <td>\b psi0_eff</td>
 *       <td>effective surface potential in [Nm/C]</td>
 *      </tr><tr>
 *       <td>\b epsilon</td>
 *       <td>dielectric constant of solvent in [C^2 / (Nm)^2]</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>volume fraction</td>
 *      </tr><tr>
 *       <td>\b kD</td>
 *       <td>inverse Debye screening length in [nm^1]</td>
 *      </tr><tr>
 *       <td>\b T</td>
 *       <td>temperature in [K]</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_baba_ahmed_1
 *
 * \sa sasfit_baba_ahmed.h, sq_plugins
 */
sasfit_baba_ahmed_DLLEXP scalar sasfit_sq_baba_ahmed_1(scalar q, sasfit_param * p);

/**
 * \ingroup sq_baba_ahmed_1
 *
 * \sa sasfit_baba_ahmed.h, sq_plugins
 */
sasfit_baba_ahmed_DLLEXP scalar sasfit_sq_baba_ahmed_1_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_baba_ahmed_1
 *
 * \sa sasfit_baba_ahmed.h, sq_plugins
 */
sasfit_baba_ahmed_DLLEXP scalar sasfit_sq_baba_ahmed_1_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_baba_ahmed_1 ################ */

/* ################ start sq_baba_ahmed_2 ################ */
/** 
 * \defgroup sq_baba_ahmed_2 Baba Ahmed 2
 * \ingroup sq_plugins
 *
 * \brief \<some brief description of Baba Ahmed 2 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b r_eff</td>
 *       <td>effective radius in [nm]</td>
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
 *       <td>\b eta</td>
 *       <td>volume fraction</td>
 *      </tr><tr>
 *       <td>\b T</td>
 *       <td>temperature in [K]</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_baba_ahmed_2
 *
 * \sa sasfit_baba_ahmed.h, sq_plugins
 */
sasfit_baba_ahmed_DLLEXP scalar sasfit_sq_baba_ahmed_2(scalar q, sasfit_param * p);

/**
 * \ingroup sq_baba_ahmed_2
 *
 * \sa sasfit_baba_ahmed.h, sq_plugins
 */
sasfit_baba_ahmed_DLLEXP scalar sasfit_sq_baba_ahmed_2_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_baba_ahmed_2
 *
 * \sa sasfit_baba_ahmed.h, sq_plugins
 */
sasfit_baba_ahmed_DLLEXP scalar sasfit_sq_baba_ahmed_2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_baba_ahmed_2 ################ */

/* ################ start sq_baba_ahmed_syl ################ */
/** 
 * \defgroup sq_baba_ahmed_syl Baba Ahmed (sylvain)
 * \ingroup sq_plugins
 *
 * \brief \<some brief description of Baba Ahmed (sylvain) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b r_ref</td>
 *       <td>reference radius in [nm]: r_eff = alpha * r_ref</td>
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
 *       <td>\b eta_ref</td>
 *       <td>reference volume fraction: eta = alpha * eta_ref</td>
 *      </tr><tr>
 *       <td>\b T</td>
 *       <td>temperature in [K]</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_baba_ahmed_syl
 *
 * \sa sasfit_baba_ahmed.h, sq_plugins
 */
sasfit_baba_ahmed_DLLEXP scalar sasfit_sq_baba_ahmed_syl(scalar q, sasfit_param * p);

/**
 * \ingroup sq_baba_ahmed_syl
 *
 * \sa sasfit_baba_ahmed.h, sq_plugins
 */
sasfit_baba_ahmed_DLLEXP scalar sasfit_sq_baba_ahmed_syl_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_baba_ahmed_syl
 *
 * \sa sasfit_baba_ahmed.h, sq_plugins
 */
sasfit_baba_ahmed_DLLEXP scalar sasfit_sq_baba_ahmed_syl_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_baba_ahmed_2 ################ */

#endif // this file

