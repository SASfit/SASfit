/*
 * Author(s) of this file:
 *   (see README.md)
 */

#ifndef SASFIT_PLUGIN_RY_POLYDISPERSE_YUKAWA_H
#define SASFIT_PLUGIN_RY_POLYDISPERSE_YUKAWA_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_ry_polydisperse_yukawa.h
 * Public available functions and descriptions of the ry_polydisperse_yukawa plugin.
 */

/**
 * \def sasfit_ry_polydisperse_yukawa_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_ry_polydisperse_yukawa_EXPORTS)
	#ifdef sasfit_ry_polydisperse_yukawa_DLLEXP
	#undef sasfit_ry_polydisperse_yukawa_DLLEXP
	#endif
	#define sasfit_ry_polydisperse_yukawa_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_ry_polydisperse_yukawa_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_ry_polydisperse_yukawa_DLLEXP SASFIT_LIB_IMPORT
#endif

/* ################ start ff_RYPolydisperseYukawa ################ */
/**
 * \defgroup ff_RYPolydisperseYukawa RY polydisperse hard-core Yukawa
 * \ingroup ff_plugins_user1
 *
 * \brief Measured structure factor of a size- and charge-polydisperse
 *        hard-core Yukawa (charged colloid) dispersion, from a numerical
 *        multicomponent Ornstein-Zernike solve with the thermodynamically
 *        self-consistent Rogers-Young closure.
 *
 * Unlike the analytically solvable MSA/RMSA (see the RMSA and two-Yukawa
 * plugins), the Rogers-Young closure has no closed-form solution and the
 * coupled multicomponent OZ equations are solved numerically here. RY is
 * reported to be substantially more accurate than RMSA for strongly coupled
 * charged colloids: comparing with Monte-Carlo data, D'Aguanno & Klein find
 * RY reproduces the excess energy to within 1% and the excess pressure to
 * within 3%, while noting that RMSA "disagrees substantially" with the same
 * simulation data.
 *
 * The continuous Schulz size distribution is reduced to \b nclass discrete
 * components by moment matching (Gauss-generalized-Laguerre quadrature),
 * which reproduces the first 2*nclass-1 moments exactly. D'Aguanno & Klein
 * report -- and this was reproduced independently during development -- that
 * \b nclass = 3 already gives results indistinguishable from 5 components up
 * to 30% polydispersity, so there is normally no reason to raise it.
 *
 * The solve is cached: the OZ system is re-solved only when a parameter
 * actually changes, and successive solves warm-start from the previous
 * solution, so a q-scan or a fit costs one solve per parameter set rather
 * than one per q-point.
 *
 * Since the particles are charged, size polydispersity necessarily implies
 * charge polydispersity. How the valence scales with size is a user choice
 * (\b chargeExp), because it is genuinely unsettled: constant surface charge
 * density gives Z ~ sigma^2 (the assumption D'Aguanno & Klein make, and
 * explicitly flag as having "little precise experimental information"),
 * whereas charge renormalisation saturation and electrophoresis on highly
 * charged low-salt spheres both indicate Z ~ sigma. Values are effective
 * (renormalised) charges, not bare ones.
 *
 * \note Lengths are in units of the mean hard-sphere diameter \b sigma.
 * \note Default (Size) Distribution: \ref delta
 *
 * \note DELIVERED AS A FORM FACTOR, though what it returns is the measured
 *       structure factor S^M(q). The reason is that S^M already carries the
 *       form-factor weighting and is normalised by &lt;F^2&gt;: offered as an
 *       sq_, SASfit would multiply a SECOND, independently parametrised form
 *       factor onto it, and nothing would check that the two used the same
 *       size distribution. A width of 0.15 here against 0.25 there gives a
 *       plausible curve, no warning, and a fit that converges to meaningless
 *       parameters. Leave SASfit's own structure factor at 1.
 *
 * \note USE THIS MODEL ALONE. SASfit's amplitude and volume functions exist
 *       so it can build the SIMPLIFIED polydisperse structure factors --
 *       decoupling, local monodisperse and the rest, which assume the size
 *       average and the interaction separate. This model does the exact
 *       calculation instead: the polydispersity is coupled to the
 *       interaction through the multicomponent OZ equations and the average
 *       is performed inside, over \b nclass size classes of width \b s_rel.
 *       Those approximations therefore do not apply to it, and an
 *       interacting polydisperse system has no single amplitude to offer
 *       them. Both functions return 0, SASfit's own signal that the quantity
 *       is unavailable. Leave the structure factor at 1 and do not attach a
 *       size distribution: the one it would apply is already applied.
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b sigma</td>
 *       <td>mean hard-sphere diameter in [nm]</td>
 *      </tr><tr>
 *       <td>\b s_rel</td>
 *       <td>relative width of the Schulz size distribution, sqrt(&lt;sigma^2&gt;-&lt;sigma&gt;^2)/&lt;sigma&gt;; 0 gives the monodisperse limit</td>
 *      </tr><tr>
 *       <td>\b phi</td>
 *       <td>volume fraction</td>
 *      </tr><tr>
 *       <td>\b Z</td>
 *       <td>effective valence of the particle of diameter &lt;sigma&gt;</td>
 *      </tr><tr>
 *       <td>\b L_B</td>
 *       <td>Bjerrum length in the same units as sigma</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>Rogers-Young mixing parameter; alpha-&gt;0 recovers Percus-Yevick, alpha-&gt;infinity recovers HNC. Should be fixed by thermodynamic consistency (see the note below)</td>
 *      </tr><tr>
 *       <td>\b nclass</td>
 *       <td>number of discrete size classes (3 is usually enough, max 12)</td>
 *      </tr><tr>
 *       <td>\b chargeExp</td>
 *       <td>valence scaling Z_i = Z (sigma_i/&lt;sigma&gt;)^chargeExp: 2 = constant surface charge density, 1 = linear in size (charge-renormalisation saturated), 0 = size independent</td>
 *      </tr></table>
 *
 * \note \b alpha is NOT determined automatically here. Rogers-Young fixes it
 *       by requiring the compressibility and virial routes to the pressure to
 *       agree, which costs three further OZ solves per trial value and is far
 *       too slow inside a fit. Determine it once for a representative
 *       parameter set with the "find thermodynamically consistent value"
 *       option in the Python OZ solver GUI (src/pyozGUI), then keep it fixed
 *       -- it is a slowly varying function of density, which is the same
 *       reasoning D'Aguanno & Klein use when they hold it constant across
 *       their own finite-difference density derivative.
 */

/**
 * \ingroup ff_RYPolydisperseYukawa
 *
 * \sa sasfit_ry_polydisperse_yukawa.h, ff_plugins_user1
 */
sasfit_ry_polydisperse_yukawa_DLLEXP scalar sasfit_ff_RYPolydisperseYukawa(scalar q, sasfit_param * p);

/**
 * \ingroup ff_RYPolydisperseYukawa
 *
 * Returns 0: SASfit uses the amplitude to build the simplified polydisperse
 * structure factors, which do not apply here -- this model performs the size
 * average exactly, inside, coupled to the interaction. See the note on the
 * group above.
 *
 * \sa sasfit_ry_polydisperse_yukawa.h, ff_plugins_user1
 */
sasfit_ry_polydisperse_yukawa_DLLEXP scalar sasfit_ff_RYPolydisperseYukawa_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_RYPolydisperseYukawa
 *
 * Returns 0: the volume serves the same simplified schemes as the amplitude
 * and is unavailable for the same reason.
 *
 * \sa sasfit_ry_polydisperse_yukawa.h, ff_plugins_user1
 */
sasfit_ry_polydisperse_yukawa_DLLEXP scalar sasfit_ff_RYPolydisperseYukawa_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_RYPolydisperseYukawa ################ */

#endif // this file
