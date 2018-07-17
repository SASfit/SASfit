/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_MIE_H
#define SASFIT_PLUGIN_MIE_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_mie.h
 * Public available functions and descriptions of the mie plugin.
 */

/**
 * \def sasfit_mie_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_mie_EXPORTS)
	#ifdef sasfit_mie_DLLEXP
	#undef sasfit_mie_DLLEXP
	#endif
	#define sasfit_mie_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_mie_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_mie_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_miesphere ################ */
/** 
 * \defgroup ff_miesphere MieSphere
 * \ingroup ff_plugins_mie
 *
 * \brief \<some brief description of MieSphere function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius of sphere in nm</td>
 *      </tr><tr>
 *       <td>\b lam_sol</td>
 *       <td>wavelength of light in solvent in nm</td>
 *      </tr><tr>
 *       <td>\b m_re</td>
 *       <td>real part of relative refraction index</td>
 *      </tr><tr>
 *       <td>\b m_im</td>
 *       <td>imaginary part of relative refraction index</td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>pol=0: unpolarized\np>0 (p<0) polarization parralel (perpendiculat) to scattering plane</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_miesphere
 *
 * \sa sasfit_mie.h, ff_plugins_mie
 */
sasfit_mie_DLLEXP scalar sasfit_ff_miesphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_miesphere
 *
 * \sa sasfit_mie.h, ff_plugins_mie
 */
sasfit_mie_DLLEXP scalar sasfit_ff_miesphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_miesphere
 *
 * \sa sasfit_mie.h, ff_plugins_mie
 */
sasfit_mie_DLLEXP scalar sasfit_ff_miesphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_miesphere ################ */

/* ################ start ff_mieshell ################ */
/** 
 * \defgroup ff_mieshell MieShell
 * \ingroup ff_plugins_mie
 *
 * \brief \<some brief description of MieShell function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>core radius of sphere in nm</td>
 *      </tr><tr>
 *       <td>\b dR</td>
 *       <td>shell thickness of sphere in nm</td>
 *      </tr><tr>
 *       <td>\b lam_sol</td>
 *       <td>wavelength of light in solvent in nm</td>
 *      </tr><tr>
 *       <td>\b mc_re</td>
 *       <td>real part of relative refraction index of core</td>
 *      </tr><tr>
 *       <td>\b mc_im</td>
 *       <td>maginary part of relative refraction index of core</td>
 *      </tr><tr>
 *       <td>\b ms_re</td>
 *       <td>real part of relative refraction index of shell</td>
 *      </tr><tr>
 *       <td>\b ms_im</td>
 *       <td>maginary part of relative refraction index of shell</td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>pol=0: unpolarized\np>0 (p<0) polarization parralel (perpendiculat) to scattering plane</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_mieshell
 *
 * \sa sasfit_mie.h, ff_plugins_mie
 */
sasfit_mie_DLLEXP scalar sasfit_ff_mieshell(scalar q, sasfit_param * p);

/**
 * \ingroup ff_mieshell
 *
 * \sa sasfit_mie.h, ff_plugins_mie
 */
sasfit_mie_DLLEXP scalar sasfit_ff_mieshell_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_mieshell
 *
 * \sa sasfit_mie.h, ff_plugins_mie
 */
sasfit_mie_DLLEXP scalar sasfit_ff_mieshell_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_mieshell ################ */


#endif // this file

