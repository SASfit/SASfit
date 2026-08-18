/*
 * Author(s) of this file:
 *   (see top-level README.md)
 */

#ifndef SASFIT_PLUGIN_RMSA_H
#define SASFIT_PLUGIN_RMSA_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_RMSA.h
 * Public available functions and descriptions of the RMSA plugin.
 */

/**
 * \def sasfit_RMSA_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_RMSA_EXPORTS)
	#ifdef sasfit_RMSA_DLLEXP
	#undef sasfit_RMSA_DLLEXP
	#endif
	#define sasfit_RMSA_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_RMSA_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_RMSA_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the structure factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq_RMSA ################ */
/**
 * \defgroup sq_RMSA RMSA (rescaled MSA charged sphere)
 * \ingroup sq_plugins_user1
 *
 * \brief Rescaled mean spherical approximation (RMSA) structure
 *        factor for charged colloidal spheres interacting via a
 *        screened Coulomb (Yukawa) potential.
 *
 * Solves the Hayter & Penfold (Mol. Phys. 42, 109 (1981)) analytic
 * mean spherical approximation for the structure factor of charged
 * hard spheres, with the Hansen & Hayter (Mol. Phys. 46, 651 (1982))
 * rescaling procedure applied automatically whenever the unrescaled
 * solution would give an unphysical (negative) contact value g(1+).
 * Physical parameters (macroion charge, salt concentration,
 * temperature, dielectric constant) are converted to the model's own
 * dimensionless (gamma, kappa*sigma) form internally via the standard
 * linearized Poisson-Boltzmann (Debye-Hueckel) treatment.
 *
 * The underlying solve (quartic root search over the exact MSA
 * closure, physical-root selection by direct evaluation of g(r) near
 * contact, rather than the original Newton-iteration algorithm known
 * to sometimes converge on the wrong root at strong coupling) is
 * cached across repeated calls with unchanged physical parameters, and
 * warm-started (Newton-refined from the previous solve) for small
 * parameter perturbations -- both automatic, requiring no action from
 * the GUI or a fit script calling this function repeatedly.
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b RADIUS</td>
 *       <td>effective hard-sphere radius, in Angstrom</td>
 *      </tr><tr>
 *       <td>\b CHARGE</td>
 *       <td>effective macroion charge Z*, in elementary-charge units</td>
 *      </tr><tr>
 *       <td>\b VOLFRACTION</td>
 *       <td>macroion volume fraction, 0 &lt; eta &lt; 1</td>
 *      </tr><tr>
 *       <td>\b TEMPERATURE</td>
 *       <td>solution temperature, in kelvin (not Celsius)</td>
 *      </tr><tr>
 *       <td>\b SALT_MOLARITY</td>
 *       <td>added monovalent (1:1) salt concentration, in mol/L</td>
 *      </tr><tr>
 *       <td>\b DIELECTRIC_CONST</td>
 *       <td>solvent relative permittivity (~78.5 for water at 25 C)</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_RMSA
 *
 * \sa sasfit_RMSA.h, sq_plugins_user1
 */
sasfit_RMSA_DLLEXP scalar sasfit_sq_RMSA(scalar q, sasfit_param * p);

/**
 * \ingroup sq_RMSA
 *
 * \sa sasfit_RMSA.h, sq_plugins_user1
 */
sasfit_RMSA_DLLEXP scalar sasfit_sq_RMSA_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_RMSA
 *
 * \sa sasfit_RMSA.h, sq_plugins_user1
 */
sasfit_RMSA_DLLEXP scalar sasfit_sq_RMSA_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_RMSA ################ */

#endif // this file
