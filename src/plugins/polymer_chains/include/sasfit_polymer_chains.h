/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_POLYMER_CHAINS_H
#define SASFIT_PLUGIN_POLYMER_CHAINS_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_polymer_chains.h
 * Public available functions and descriptions of the polymer_chains plugin.
 */

/**
 * \def sasfit_polymer_chains_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_polymer_chains_EXPORTS)
	#ifdef sasfit_polymer_chains_DLLEXP
	#undef sasfit_polymer_chains_DLLEXP
	#endif
	#define sasfit_polymer_chains_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_polymer_chains_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_polymer_chains_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_gauss ################ */
/** 
 * \defgroup ff_gauss Gauss
 * \ingroup ff_polymer_chains
 *
 * \brief \<some brief description of Gauss function\>
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
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering I(Q=0)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_gauss
 *
 * \sa sasfit_polymer_chains.h, ff_polymer_chains
 */
sasfit_polymer_chains_DLLEXP scalar sasfit_ff_gauss(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gauss
 *
 * \sa sasfit_polymer_chains.h, ff_polymer_chains
 */
sasfit_polymer_chains_DLLEXP scalar sasfit_ff_gauss_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gauss
 *
 * \sa sasfit_polymer_chains.h, ff_polymer_chains
 */
sasfit_polymer_chains_DLLEXP scalar sasfit_ff_gauss_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_gauss ################ */

/* ################ start ff_gauss2 ################ */
/** 
 * \defgroup ff_gauss2 Gauss2
 * \ingroup ff_polymer_chains
 *
 * \brief \<some brief description of Gauss2 function\>
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
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b b_p</td>
 *       <td>scattering length of polymer in \[cm\]</td>
 *      </tr><tr>
 *       <td>\b V</td>
 *       <td>volume of polymer Mw/(Na rho_p) in \[cm^3\]</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta_s</td>
 *       <td>scattering length density of solvent in \[cm^-2\]</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_gauss2
 *
 * \sa sasfit_polymer_chains.h, ff_polymer_chains
 */
sasfit_polymer_chains_DLLEXP scalar sasfit_ff_gauss2(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gauss2
 *
 * \sa sasfit_polymer_chains.h, ff_polymer_chains
 */
sasfit_polymer_chains_DLLEXP scalar sasfit_ff_gauss2_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gauss2
 *
 * \sa sasfit_polymer_chains.h, ff_polymer_chains
 */
sasfit_polymer_chains_DLLEXP scalar sasfit_ff_gauss2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_gauss2 ################ */

/* ################ start ff_gauss3 ################ */
/** 
 * \defgroup ff_gauss3 Gauss3
 * \ingroup ff_polymer_chains
 *
 * \brief \<some brief description of Gauss3 function\>
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
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b b_p</td>
 *       <td>scattering length of polymer in \[cm\]</td>
 *      </tr><tr>
 *       <td>\b Mw</td>
 *       <td>weight averaged molecular weight of polymer in \[g/mol\]</td>
 *      </tr><tr>
 *       <td>\b rho_p</td>
 *       <td>mass density of polymer in \[g/cm^3\]</td>
 *      </tr><tr>
 *       <td>\b eta_s</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_gauss3
 *
 * \sa sasfit_polymer_chains.h, ff_polymer_chains
 */
sasfit_polymer_chains_DLLEXP scalar sasfit_ff_gauss3(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gauss3
 *
 * \sa sasfit_polymer_chains.h, ff_polymer_chains
 */
sasfit_polymer_chains_DLLEXP scalar sasfit_ff_gauss3_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gauss3
 *
 * \sa sasfit_polymer_chains.h, ff_polymer_chains
 */
sasfit_polymer_chains_DLLEXP scalar sasfit_ff_gauss3_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_gauss3 ################ */

/* ################ start ff_gauspoly ################ */
/** 
 * \defgroup ff_gauspoly GausPoly
 * \ingroup ff_polymer_chains
 *
 * \brief \<some brief description of GausPoly function\>
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
 *       <td>\b Mw</td>
 *       <td>weight averaged molecular weight of polymer in \[g/mol\]</td>
 *      </tr><tr>
 *       <td>\b Mn</td>
 *       <td>number generalized Gaussian Coilaveraged molecular weight of polymer in \[g/mol\]</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering I(Q=0)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_gauspoly
 *
 * \sa sasfit_polymer_chains.h, ff_polymer_chains
 */
sasfit_polymer_chains_DLLEXP scalar sasfit_ff_gauspoly(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gauspoly
 *
 * \sa sasfit_polymer_chains.h, ff_polymer_chains
 */
sasfit_polymer_chains_DLLEXP scalar sasfit_ff_gauspoly_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gauspoly
 *
 * \sa sasfit_polymer_chains.h, ff_polymer_chains
 */
sasfit_polymer_chains_DLLEXP scalar sasfit_ff_gauspoly_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_gauspoly ################ */

/* ################ start ff_generalized_gaussian_coil ################ */
/** 
 * \defgroup ff_generalized_gaussian_coil generalized Gaussian coil
 * \ingroup ff_polymer_chains
 *
 * \brief \<some brief description of generalized Gaussian coil function\>
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
 *       <td>\b nu</td>
 *       <td>Flory exponent\ncollapsed coil nu=1/3, coil im theta-solvent nu=1/2, swollen coil nu=3/5</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_generalized_gaussian_coil
 *
 * \sa sasfit_polymer_chains.h, ff_polymer_chains
 */
sasfit_polymer_chains_DLLEXP scalar sasfit_ff_generalized_gaussian_coil(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_gaussian_coil
 *
 * \sa sasfit_polymer_chains.h, ff_polymer_chains
 */
sasfit_polymer_chains_DLLEXP scalar sasfit_ff_generalized_gaussian_coil_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_gaussian_coil
 *
 * \sa sasfit_polymer_chains.h, ff_polymer_chains
 */
sasfit_polymer_chains_DLLEXP scalar sasfit_ff_generalized_gaussian_coil_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_generalized_gaussian_coil ################ */

/* ################ start ff_generalized_gaussian_coil_2 ################ */
/** 
 * \defgroup ff_generalized_gaussian_coil_2 generalized Gaussian coil 2
 * \ingroup ff_polymer_chains
 *
 * \brief \<some brief description of generalized Gaussian coil 2 function\>
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
 *       <td>\b nu</td>
 *       <td>Flory exponent\ncollapsed coil nu=1/3, coil im theta-solvent nu=1/2, swollen coil nu=3/5</td>
 *      </tr><tr>
 *       <td>\b b_p</td>
 *       <td>scattering length of polymer in \[cm\]</td>
 *      </tr><tr>
 *       <td>\b V</td>
 *       <td>volume of polymer Mw/(Na rho_p) in \[cm^3\]</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta_s</td>
 *       <td>scattering length density of solvent in \[cm^-2\]</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_generalized_gaussian_coil_2
 *
 * \sa sasfit_polymer_chains.h, ff_polymer_chains
 */
sasfit_polymer_chains_DLLEXP scalar sasfit_ff_generalized_gaussian_coil_2(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_gaussian_coil_2
 *
 * \sa sasfit_polymer_chains.h, ff_polymer_chains
 */
sasfit_polymer_chains_DLLEXP scalar sasfit_ff_generalized_gaussian_coil_2_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_gaussian_coil_2
 *
 * \sa sasfit_polymer_chains.h, ff_polymer_chains
 */
sasfit_polymer_chains_DLLEXP scalar sasfit_ff_generalized_gaussian_coil_2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_generalized_gaussian_coil_2 ################ */

/* ################ start ff_generalized_gaussian_coil_3 ################ */
/** 
 * \defgroup ff_generalized_gaussian_coil_3 generalized Gaussian coil 3
 * \ingroup ff_polymer_chains
 *
 * \brief \<some brief description of generalized Gaussian coil 3 function\>
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
 *       <td>\b nu</td>
 *       <td>Flory exponent\ncollapsed coil nu=1/3, coil im theta-solvent nu=1/2, swollen coil nu=3/5</td>
 *      </tr><tr>
 *       <td>\b b_p</td>
 *       <td>scattering length of polymer in \[cm\]</td>
 *      </tr><tr>
 *       <td>\b Mw</td>
 *       <td>weight averaged molecular weight of polymer in \[g/mol\]</td>
 *      </tr><tr>
 *       <td>\b rho_p</td>
 *       <td>mass density of polymer in \[g/cm^3\]</td>
 *      </tr><tr>
 *       <td>\b eta_s</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_generalized_gaussian_coil_3
 *
 * \sa sasfit_polymer_chains.h, ff_polymer_chains
 */
sasfit_polymer_chains_DLLEXP scalar sasfit_ff_generalized_gaussian_coil_3(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_gaussian_coil_3
 *
 * \sa sasfit_polymer_chains.h, ff_polymer_chains
 */
sasfit_polymer_chains_DLLEXP scalar sasfit_ff_generalized_gaussian_coil_3_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_gaussian_coil_3
 *
 * \sa sasfit_polymer_chains.h, ff_polymer_chains
 */
sasfit_polymer_chains_DLLEXP scalar sasfit_ff_generalized_gaussian_coil_3_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_generalized_gaussian_coil_3 ################ */


#endif // this file

