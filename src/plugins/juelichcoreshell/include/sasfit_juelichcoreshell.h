/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_JUELICHCORESHELL_H
#define SASFIT_PLUGIN_JUELICHCORESHELL_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_juelichcoreshell.h
 * Public available functions and descriptions of the juelichcoreshell plugin.
 */

/**
 * \def sasfit_juelichcoreshell_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_juelichcoreshell_EXPORTS)
	#ifdef sasfit_juelichcoreshell_DLLEXP
	#undef sasfit_juelichcoreshell_DLLEXP
	#endif
	#define sasfit_juelichcoreshell_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_juelichcoreshell_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_juelichcoreshell_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_juelichcoreshell ################ */
/** 
 * \defgroup ff_juelichcoreshell JuelichCoreShell
 * \ingroup ff_plugins_fuzzy_sph
 *
 * \brief \<some brief description of JuelichCoreShell function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b C </td>
 *       <td>scaling constant</td>
 *      </tr><tr>
 *       <td>\b Mcore</td>
 *       <td>molecular weight core (g/mol)</td>
 *      </tr><tr>
 *       <td>\b Mbrush</td>
 *       <td>molecular weight brush (g/mol)</td>
 *      </tr><tr>
 *       <td>\b rho_core</td>
 *       <td>mass density core matter (g/cm**3) </td>
 *      </tr><tr>
 *       <td>\b rho_brush</td>
 *       <td>mass density brush matter (g/cm**3)</td>
 *      </tr><tr>
 *       <td>\b b_core</td>
 *       <td>scattering length density core mat. (cm**-2)</td>
 *      </tr><tr>
 *       <td>\b b_brush</td>
 *       <td>scattering length density brush mat.(cm**-2) </td>
 *      </tr><tr>
 *       <td>\b Nagg</td>
 *       <td>aggregation number</td>
 *      </tr><tr>
 *       <td>\b d1_plus</td>
 *       <td>extra radius of shell1=core (compared to compact)</td>
 *      </tr><tr>
 *       <td>\b part23</td>
 *       <td>relative distribution of shell amount in shell2:shell3 (0..inf) </td>
 *      </tr><tr>
 *       <td>\b d2_plus</td>
 *       <td>extra radius of shell2 (compared to compact) </td>
 *      </tr><tr>
 *       <td>\b d3_plus</td>
 *       <td>extra radius of shell3 (compared to compact)</td>
 *      </tr><tr>
 *       <td>\b sigma1</td>
 *       <td>core smearing</td>
 *      </tr><tr>
 *       <td>\b sigma2</td>
 *       <td>shell2 smearing</td>
 *      </tr><tr>
 *       <td>\b sigma3</td>
 *       <td>shell3 smearing</td>
 *      </tr><tr>
 *       <td>\b partstar</td>
 *       <td>relative distribution of parbolic:starlike profile in shell3; one usually puts a very high value in order to consider only a star-like profile.</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>star-like exponent is 4/3, const=0</td>
 *      </tr><tr>
 *       <td>\b lparabol</td>
 *       <td>thickness of parabolic brush (must fit in shell3!) I put it = 0</td>
 *      </tr><tr>
 *       <td>\b f_brush</td>
 *       <td>scattering length density correction factor brush</td>
 *      </tr><tr>
 *       <td>\b f_core</td>
 *       <td>scattering length density correction factor core </td>
 *      </tr><tr>
 *       <td>\b rhosolv</td>
 *       <td>scattering length density of solvent </td>
 *      </tr></table>
 */

/**
 * \ingroup ff_juelichcoreshell
 *
 * \sa sasfit_juelichcoreshell.h, ff_plugins_fuzzy_sph
 */
sasfit_juelichcoreshell_DLLEXP scalar sasfit_ff_juelichcoreshell(scalar q, sasfit_param * p);

/**
 * \ingroup ff_juelichcoreshell
 *
 * \sa sasfit_juelichcoreshell.h, ff_plugins_fuzzy_sph
 */
sasfit_juelichcoreshell_DLLEXP scalar sasfit_ff_juelichcoreshell_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_juelichcoreshell
 *
 * \sa sasfit_juelichcoreshell.h, ff_plugins_fuzzy_sph
 */
sasfit_juelichcoreshell_DLLEXP scalar sasfit_ff_juelichcoreshell_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_juelichcoreshell ################ */


#endif // this file

