/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_VESICLE_PEG_H
#define SASFIT_PLUGIN_VESICLE_PEG_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_vesicle_peg.h
 * Public available functions and descriptions of the vesicle_peg plugin.
 */

/**
 * \def sasfit_vesicle_peg_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_vesicle_peg_EXPORTS)
	#ifdef sasfit_vesicle_peg_DLLEXP
	#undef sasfit_vesicle_peg_DLLEXP
	#endif
	#define sasfit_vesicle_peg_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_vesicle_peg_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_vesicle_peg_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_vesicle_peg_piecew_const ################ */
/**
 * \defgroup ff_vesicle_peg_piecew_const vesicle (PEG, piecew. const.)
 * \ingroup ff_devel
 *
 * \brief Vesicle (PEGylated w. piecewise constant profile)
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>average vesicle radius</td>
 *      </tr><tr>
 *       <td>\b sigma_R</td>
 *       <td>width of LogNorm size distribution of radii</td>
 *      </tr><tr>
 *       <td>\b T</td>
 *       <td>thickness of central tail layer</td>
 *      </tr><tr>
 *       <td>\b sigma_T</td>
 *       <td>width of LogNorm thickness distribution</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of attached PEG molecules</td>
 *      </tr><tr>
 *       <td>\b f_PEG_i</td>
 *       <td>fraction of PEG decorated lipids, where PEG is pointing inside the vesicle<</td>
 *      </tr><tr>
 *       <td>\b f_PEG_o</td>
 *       <td>fraction of PEG decorated lipids, where PEG is pointing outside the vesicle</td>
 *      </tr><tr>
 *       <td>\b V_PEG</td>
 *       <td>molecular PEG volume</td>
 *      </tr><tr>
 *       <td>\b eta_PEG</td>
 *       <td>scattering length density of PEG</td>
 *      </tr><tr>
 *       <td>\b V_head</td>
 *       <td>molecular volume of lipid head group</td>
 *      </tr><tr>
 *       <td>\b eta_head</td>
 *       <td>scattering length density of lipid head group</td>
 *      </tr><tr>
 *       <td>\b V_tail</td>
 *       <td>molecular volume of hydrocarbon tail group of the lipids</td>
 *      </tr><tr>
 *       <td>\b eta_tail</td>
 *       <td>scattering length of hydrocarbon tail group of lipids</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_vesicle_peg_piecew_const
 *
 * \sa sasfit_vesicle_peg.h, ff_devel
 */
sasfit_vesicle_peg_DLLEXP scalar sasfit_ff_vesicle_peg_piecew_const(scalar q, sasfit_param * p);

/**
 * \ingroup ff_vesicle_peg_piecew_const
 *
 * \sa sasfit_vesicle_peg.h, ff_devel
 */
sasfit_vesicle_peg_DLLEXP scalar sasfit_ff_vesicle_peg_piecew_const_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_vesicle_peg_piecew_const
 *
 * \sa sasfit_vesicle_peg.h, ff_devel
 */
sasfit_vesicle_peg_DLLEXP scalar sasfit_ff_vesicle_peg_piecew_const_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_vesicle_peg_piecew_const ################ */


/* ################ start ff_vesicle_peg_piecew_const_thin_approx ################ */
/**
 * \defgroup ff_vesicle_peg_piecew_const_thin_approx vesicle (PEG, piecew. const., thin approx)
 * \ingroup ff_devel
 *
 * \brief Vesicle (PEGylated w. piecewise constant profile in thin approximation)
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>average vesicle radius</td>
 *      </tr><tr>
 *       <td>\b sigma_R</td>
 *       <td>width of LogNorm size distribution of radii</td>
 *      </tr><tr>
 *       <td>\b T</td>
 *       <td>thickness of central tail layer</td>
 *      </tr><tr>
 *       <td>\b sigma_T</td>
 *       <td>width of LogNorm thickness distribution</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of attached PEG molecules</td>
 *      </tr><tr>
 *       <td>\b f_PEG_i</td>
 *       <td>fraction of PEG decorated lipids, where PEG is pointing inside the vesicle<</td>
 *      </tr><tr>
 *       <td>\b f_PEG_o</td>
 *       <td>fraction of PEG decorated lipids, where PEG is pointing outside the vesicle</td>
 *      </tr><tr>
 *       <td>\b V_PEG</td>
 *       <td>molecular PEG volume</td>
 *      </tr><tr>
 *       <td>\b eta_PEG</td>
 *       <td>scattering length density of PEG</td>
 *      </tr><tr>
 *       <td>\b V_head</td>
 *       <td>molecular volume of lipid head group</td>
 *      </tr><tr>
 *       <td>\b eta_head</td>
 *       <td>scattering length density of lipid head group</td>
 *      </tr><tr>
 *       <td>\b V_tail</td>
 *       <td>molecular volume of hydrocarbon tail group of the lipids</td>
 *      </tr><tr>
 *       <td>\b eta_tail</td>
 *       <td>scattering length of hydrocarbon tail group of lipids</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_vesicle_peg_piecew_const_thin_approx
 *
 * \sa sasfit_vesicle_peg.h, ff_devel
 */
sasfit_vesicle_peg_DLLEXP scalar sasfit_ff_vesicle_peg_piecew_const_thin_approx(scalar q, sasfit_param * p);

/**
 * \ingroup ff_vesicle_peg_piecew_const_thin_approx
 *
 * \sa sasfit_vesicle_peg.h, ff_devel
 */
sasfit_vesicle_peg_DLLEXP scalar sasfit_ff_vesicle_peg_piecew_const_thin_approx_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_vesicle_peg_piecew_const_thin_approx
 *
 * \sa sasfit_vesicle_peg.h, ff_devel
 */
sasfit_vesicle_peg_DLLEXP scalar sasfit_ff_vesicle_peg_piecew_const_thin_approx_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_vesicle_peg_piecew_const_thin_approx ################ */


/* ################ start ff_vesicle_peg_laplace ################ */
/**
 * \defgroup ff_vesicle_peg_laplace vesicle (PEG, Laplace)
 * \ingroup ff_devel
 *
 * \brief Vesicle (PEGylated w. laplacian profile)
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>average vesicle radius</td>
 *      </tr><tr>
 *       <td>\b sigma_R</td>
 *       <td>width of LogNorm size distribution of radii</td>
 *      </tr><tr>
 *       <td>\b T</td>
 *       <td>thickness of central tail layer</td>
 *      </tr><tr>
 *       <td>\b sigma_T</td>
 *       <td>width of LogNorm thickness distribution</td>
 *      </tr><tr>
 *       <td>\b sigma_i</td>
 *       <td>smearing width parameter of membrane interfaces inside vesicle</td>
 *      </tr><tr>
 *       <td>\b sigma_o</td>
 *       <td>smearing width parameter of membrane interfaces outside vesicle</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of attached PEG molecules</td>
 *      </tr><tr>
 *       <td>\b f_PEG_i</td>
 *       <td>fraction of PEG decorated lipids, where PEG is pointing inside the vesicle<</td>
 *      </tr><tr>
 *       <td>\b f_PEG_o</td>
 *       <td>fraction of PEG decorated lipids, where PEG is pointing outside the vesicle</td>
 *      </tr><tr>
 *       <td>\b V_PEG</td>
 *       <td>molecular PEG volume</td>
 *      </tr><tr>
 *       <td>\b eta_PEG</td>
 *       <td>scattering length density of PEG</td>
 *      </tr><tr>
 *       <td>\b V_head</td>
 *       <td>molecular volume of lipid head group</td>
 *      </tr><tr>
 *       <td>\b eta_head</td>
 *       <td>scattering length density of lipid head group</td>
 *      </tr><tr>
 *       <td>\b V_tail</td>
 *       <td>molecular volume of hydrocarbon tail group of the lipids</td>
 *      </tr><tr>
 *       <td>\b eta_tail</td>
 *       <td>scattering length of hydrocarbon tail group of lipids</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_vesicle_peg_laplace
 *
 * \sa sasfit_vesicle_peg.h, ff_devel
 */
sasfit_vesicle_peg_DLLEXP scalar sasfit_ff_vesicle_peg_laplace(scalar q, sasfit_param * p);

/**
 * \ingroup ff_vesicle_peg_laplace
 *
 * \sa sasfit_vesicle_peg.h, ff_devel
 */
sasfit_vesicle_peg_DLLEXP scalar sasfit_ff_vesicle_peg_laplace_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_vesicle_peg_laplace
 *
 * \sa sasfit_vesicle_peg.h, ff_devel
 */
sasfit_vesicle_peg_DLLEXP scalar sasfit_ff_vesicle_peg_laplace_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_vesicle_peg_laplace ################ */


/* ################ start ff_vesicle_peg_gaussian_thin_approx ################ */
/**
 * \defgroup ff_vesicle_peg_gaussian_thin_approx vesicle (PEG, Gaussian, thin approx)
 * \ingroup ff_devel
 *
 * \brief Vesicle (PEGylated w. gaussian profile in thin approximation)
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>average vesicle radius</td>
 *      </tr><tr>
 *       <td>\b sigma_R</td>
 *       <td>width of LogNorm size distribution of radii</td>
 *      </tr><tr>
 *       <td>\b T</td>
 *       <td>thickness of central tail layer</td>
 *      </tr><tr>
 *       <td>\b sigma_T</td>
 *       <td>width of LogNorm thickness distribution</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of attached PEG molecules</td>
 *      </tr><tr>
 *       <td>\b f_PEG_i</td>
 *       <td>fraction of PEG decorated lipids, where PEG is pointing inside the vesicle<</td>
 *      </tr><tr>
 *       <td>\b f_PEG_o</td>
 *       <td>fraction of PEG decorated lipids, where PEG is pointing outside the vesicle</td>
 *      </tr><tr>
 *       <td>\b V_PEG</td>
 *       <td>molecular PEG volume</td>
 *      </tr><tr>
 *       <td>\b eta_PEG</td>
 *       <td>scattering length density of PEG</td>
 *      </tr><tr>
 *       <td>\b V_head</td>
 *       <td>molecular volume of lipid head group</td>
 *      </tr><tr>
 *       <td>\b eta_head</td>
 *       <td>scattering length density of lipid head group</td>
 *      </tr><tr>
 *       <td>\b V_tail</td>
 *       <td>molecular volume of hydrocarbon tail group of the lipids</td>
 *      </tr><tr>
 *       <td>\b eta_tail</td>
 *       <td>scattering length of hydrocarbon tail group of lipids</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_vesicle_peg_gaussian_thin_approx
 *
 * \sa sasfit_vesicle_peg.h, ff_devel
 */
sasfit_vesicle_peg_DLLEXP scalar sasfit_ff_vesicle_peg_gaussian_thin_approx(scalar q, sasfit_param * p);

/**
 * \ingroup ff_vesicle_peg_gaussian_thin_approx
 *
 * \sa sasfit_vesicle_peg.h, ff_devel
 */
sasfit_vesicle_peg_DLLEXP scalar sasfit_ff_vesicle_peg_gaussian_thin_approx_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_vesicle_peg_gaussian_thin_approx
 *
 * \sa sasfit_vesicle_peg.h, ff_devel
 */
sasfit_vesicle_peg_DLLEXP scalar sasfit_ff_vesicle_peg_gaussian_thin_approx_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_vesicle_peg_gaussian_thin_approx ################ */


#endif // this file

