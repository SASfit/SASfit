/*
 * Author(s) of this file:
 *   (see top-level README.md)
 */

#ifndef SASFIT_PLUGIN_ROBERTUS_SHS_H
#define SASFIT_PLUGIN_ROBERTUS_SHS_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_robertus_shs.h
 * Public available functions and descriptions of the robertus_shs plugin.
 */

/**
 * \def sasfit_robertus_shs_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_robertus_shs_EXPORTS)
	#ifdef sasfit_robertus_shs_DLLEXP
	#undef sasfit_robertus_shs_DLLEXP
	#endif
	#define sasfit_robertus_shs_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_robertus_shs_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_robertus_shs_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style
//
// Categorised as a FORM FACTOR (ff_*, group ff_user1), not a structure
// factor, even though it internally solves a multicomponent Percus-Yevick
// structure-factor problem: the model combines that interaction physics
// with a per-class particle form factor (Eq. 14 of the paper) and returns
// the full resulting intensity I(q), which is what a SASfit form factor is
// expected to provide. See robertus_shs_core.h for the underlying engine,
// which still exposes the S(q)-only (Eq. 18) route separately for anyone
// who wants to pair it with a different form factor instead.

/* ################ start ff_RobertusSHS_CoreShell ################ */
/**
 * \defgroup ff_RobertusSHS_CoreShell RobertusSHS (core-shell)
 * \ingroup ff_user1
 *
 * \brief Multicomponent Percus-Yevick sticky (adhesive) hard-sphere
 *        structure factor, combined with a sharp core-shell sphere
 *        form factor to give the full self-consistent polydisperse
 *        intensity.
 *
 * Solves the multicomponent quadratic Percus-Yevick equations of
 * Robertus, Philipse, Joosten & Levine, J. Chem. Phys. 90(8), 4482
 * (1989) for a size distribution discretised into \b nclass classes,
 * then combines the resulting partial structure factors with a
 * sharp-interface core-shell sphere form factor for each class
 * (Eq. 14 of the paper). Returns the full intensity I(q); when using
 * this model in a fit, set the overall scale to 1 and pair it with a
 * "None"/unity structure factor, since the interaction physics is
 * already folded in here.
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>mean hard-sphere (interaction) radius in [nm]</td>
 *      </tr><tr>
 *       <td>\b sigma_rel</td>
 *       <td>relative width sigma/R of the size distribution</td>
 *      </tr><tr>
 *       <td>\b phi</td>
 *       <td>hard-sphere volume fraction</td>
 *      </tr><tr>
 *       <td>\b tau</td>
 *       <td>stickiness (Baxter convention; small = sticky, large = hard sphere)</td>
 *      </tr><tr>
 *       <td>\b dist</td>
 *       <td>size distribution: 1=Schulz-Zimm, 2=LogNormal, 3=Gaussian, 4=Weibull,
 *           5=BiLogNormal (sum of two lognormals, see \b Rmean2/\b sigma_rel2/\b w1 below)</td>
 *      </tr><tr>
 *       <td>\b nclass</td>
 *       <td>number of size classes used to discretise the distribution (3-300;
 *           solved via SUNDIALS KINSOL/GMRES, which stays fast to well
 *           past nclass=300 -- the ceiling here is a "how long are you
 *           willing to wait" limit (nclass=300 still solves in well
 *           under 1s), not a numerical one; see robertus_shs_core.h for
 *           details)</td>
 *      </tr><tr>
 *       <td>\b Rmean2</td>
 *       <td>(only used if \b dist=5) mean radius of the second lognormal mode in [nm]</td>
 *      </tr><tr>
 *       <td>\b sigma_rel2</td>
 *       <td>(only used if \b dist=5) relative width sigma/R of the second lognormal mode</td>
 *      </tr><tr>
 *       <td>\b w1</td>
 *       <td>(only used if \b dist=5) weight of mode 1 in [0,1]; mode 2's weight is 1-w1</td>
 *      </tr><tr>
 *       <td>\b t_shell</td>
 *       <td>shell thickness in [nm], same for every size class</td>
 *      </tr><tr>
 *       <td>\b rho_core</td>
 *       <td>core scattering-length/electron density</td>
 *      </tr><tr>
 *       <td>\b rho_shell</td>
 *       <td>shell scattering-length/electron density</td>
 *      </tr><tr>
 *       <td>\b rho_solv</td>
 *       <td>solvent scattering-length/electron density</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_RobertusSHS_CoreShell
 *
 * \sa sasfit_robertus_shs.h, ff_user1
 */
sasfit_robertus_shs_DLLEXP scalar sasfit_ff_RobertusSHS_CoreShell(scalar q, sasfit_param * p);

/**
 * \ingroup ff_RobertusSHS_CoreShell
 *
 * \sa sasfit_robertus_shs.h, ff_user1
 */
sasfit_robertus_shs_DLLEXP scalar sasfit_ff_RobertusSHS_CoreShell_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_RobertusSHS_CoreShell
 *
 * \sa sasfit_robertus_shs.h, ff_user1
 */
sasfit_robertus_shs_DLLEXP scalar sasfit_ff_RobertusSHS_CoreShell_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_RobertusSHS_CoreShell ################ */

/* ################ start ff_RobertusSHS_Fuzzy ################ */
/**
 * \defgroup ff_RobertusSHS_Fuzzy RobertusSHS (fuzzy interface)
 * \ingroup ff_user1
 *
 * \brief Multicomponent Percus-Yevick sticky (adhesive) hard-sphere
 *        structure factor, combined with a homogeneous sphere form
 *        factor with a diffuse ("fuzzy") interface.
 *
 * Same interaction physics as \ref ff_RobertusSHS_CoreShell, combined
 * instead with the Bartlett-Ottewill fuzzy-sphere form factor. Returns
 * the full intensity I(q); pair with scale=1 and a "None"/unity
 * structure factor, as for \ref ff_RobertusSHS_CoreShell.
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>mean hard-sphere (interaction) radius in [nm]</td>
 *      </tr><tr>
 *       <td>\b sigma_rel</td>
 *       <td>relative width sigma/R of the size distribution</td>
 *      </tr><tr>
 *       <td>\b phi</td>
 *       <td>hard-sphere volume fraction</td>
 *      </tr><tr>
 *       <td>\b tau</td>
 *       <td>stickiness (Baxter convention)</td>
 *      </tr><tr>
 *       <td>\b dist</td>
 *       <td>size distribution: 1=Schulz-Zimm, 2=LogNormal, 3=Gaussian, 4=Weibull,
 *           5=BiLogNormal (sum of two lognormals, see \b Rmean2/\b sigma_rel2/\b w1 below)</td>
 *      </tr><tr>
 *       <td>\b nclass</td>
 *       <td>number of size classes (3-300; solved via SUNDIALS KINSOL/GMRES,
 *           which stays fast to well past nclass=300 -- the ceiling here is a
 *           "how long are you willing to wait" limit (nclass=300 still solves
 *           in well under 1s), not a numerical one; see robertus_shs_core.h
 *           for details)</td>
 *      </tr><tr>
 *       <td>\b Rmean2</td>
 *       <td>(only used if \b dist=5) mean radius of the second lognormal mode in [nm]</td>
 *      </tr><tr>
 *       <td>\b sigma_rel2</td>
 *       <td>(only used if \b dist=5) relative width sigma/R of the second lognormal mode</td>
 *      </tr><tr>
 *       <td>\b w1</td>
 *       <td>(only used if \b dist=5) weight of mode 1 in [0,1]; mode 2's weight is 1-w1</td>
 *      </tr><tr>
 *       <td>\b sigma_fuzzy</td>
 *       <td>interface diffuseness (rms width) in [nm], same for every size class</td>
 *      </tr><tr>
 *       <td>\b rho_particle</td>
 *       <td>particle scattering-length/electron density</td>
 *      </tr><tr>
 *       <td>\b rho_solv</td>
 *       <td>solvent scattering-length/electron density</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_RobertusSHS_Fuzzy
 *
 * \sa sasfit_robertus_shs.h, ff_user1
 */
sasfit_robertus_shs_DLLEXP scalar sasfit_ff_RobertusSHS_Fuzzy(scalar q, sasfit_param * p);

/**
 * \ingroup ff_RobertusSHS_Fuzzy
 *
 * \sa sasfit_robertus_shs.h, ff_user1
 */
sasfit_robertus_shs_DLLEXP scalar sasfit_ff_RobertusSHS_Fuzzy_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_RobertusSHS_Fuzzy
 *
 * \sa sasfit_robertus_shs.h, ff_user1
 */
sasfit_robertus_shs_DLLEXP scalar sasfit_ff_RobertusSHS_Fuzzy_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_RobertusSHS_Fuzzy ################ */

/* ################ start ff_RobertusSHS_GenericDemo ################ */
/**
 * \defgroup ff_RobertusSHS_GenericDemo RobertusSHS (generic demo)
 * \ingroup ff_user1
 *
 * \brief Worked example of the generic composability layer: the same
 *        multicomponent solver driven by a toy LogNormal size
 *        distribution and a toy homogeneous-sphere amplitude, both
 *        written in the ordinary scalar func(scalar,sasfit_param*)
 *        calling convention rather than being one of the two
 *        built-in models above.
 *
 * See robertus_shs_core.h's rshs_make_classes_generic()/
 * rshs_intensity_generic() for the underlying mechanism: any
 * (centrosymmetric) particle amplitude or size-distribution PDF can be
 * substituted here without touching the Percus-Yevick solver or the
 * structure-factor code.
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>mean hard-sphere (interaction) radius in [nm]</td>
 *      </tr><tr>
 *       <td>\b sigma_rel</td>
 *       <td>relative width sigma/R of the size distribution</td>
 *      </tr><tr>
 *       <td>\b phi</td>
 *       <td>hard-sphere volume fraction</td>
 *      </tr><tr>
 *       <td>\b tau</td>
 *       <td>stickiness (Baxter convention)</td>
 *      </tr><tr>
 *       <td>\b nclass</td>
 *       <td>number of size classes (3-300; solved via SUNDIALS KINSOL/GMRES,
 *           which stays fast to well past nclass=300 -- the ceiling here is a
 *           "how long are you willing to wait" limit (nclass=300 still solves
 *           in well under 1s), not a numerical one; see robertus_shs_core.h
 *           for details)</td>
 *      </tr><tr>
 *       <td>\b drho</td>
 *       <td>particle-minus-solvent scattering-length/electron density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_RobertusSHS_GenericDemo
 *
 * \sa sasfit_robertus_shs.h, ff_user1
 */
sasfit_robertus_shs_DLLEXP scalar sasfit_ff_RobertusSHS_GenericDemo(scalar q, sasfit_param * p);

/**
 * \ingroup ff_RobertusSHS_GenericDemo
 *
 * \sa sasfit_robertus_shs.h, ff_user1
 */
sasfit_robertus_shs_DLLEXP scalar sasfit_ff_RobertusSHS_GenericDemo_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_RobertusSHS_GenericDemo
 *
 * \sa sasfit_robertus_shs.h, ff_user1
 */
sasfit_robertus_shs_DLLEXP scalar sasfit_ff_RobertusSHS_GenericDemo_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_RobertusSHS_GenericDemo ################ */


#endif // this file
