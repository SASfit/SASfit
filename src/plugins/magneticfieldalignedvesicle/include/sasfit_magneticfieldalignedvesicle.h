/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_MAGNETICFIELDALIGNEDVESICLE_H
#define SASFIT_PLUGIN_MAGNETICFIELDALIGNEDVESICLE_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_magneticfieldalignedvesicle.h
 * Public available functions and descriptions of the magneticfieldalignedvesicle plugin.
 */

/**
 * \def sasfit_magneticfieldalignedvesicle_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_magneticfieldalignedvesicle_EXPORTS) 
	#ifdef sasfit_magneticfieldalignedvesicle_DLLEXP
	#undef sasfit_magneticfieldalignedvesicle_DLLEXP
	#endif
	#define sasfit_magneticfieldalignedvesicle_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_magneticfieldalignedvesicle_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_magneticfieldalignedvesicle_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_magneticfieldalignedvesicle_p2psi ################ */
/** 
 * \defgroup ff_magneticfieldalignedvesicle_p2psi MagneticFieldAlignedVesicleP2Psi
 * \ingroup ff_plugins
 *
 * \brief \<some brief description of MagneticFieldAlignedVesicleP2Psi function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rsh</td>
 *       <td>radius of spherical vesicle shell</td>
 *      </tr><tr>
 *       <td>\b theta1</td>
 *       <td>angle to describe size of first capped side in units of deg</td>
 *      </tr><tr>
 *       <td>\b theta2</td>
 *       <td>angle to describe size of second capped side in units of deg</td>
 *      </tr><tr>
 *       <td>\b t_sh</td>
 *       <td>thickness of spherical vesicle shell</td>
 *      </tr><tr>
 *       <td>\b t_cs</td>
 *       <td>thickness of flat capped sides</td>
 *      </tr><tr>
 *       <td>\b D_eta_sh</td>
 *       <td>scattering length density of spherical vesicle shell</td>
 *      </tr><tr>
 *       <td>\b D_eta_cs</td>
 *       <td>scattering length density of capped sides</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width of lognormal size distribution\n</td> 
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>direction of the scattering vector in units of deg\n</td>
 *      </tr><tr>
 *       <td>\b kappa </td>
 *       <td>orientation parameter\n</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_magneticfieldalignedvesicle_p2psi
 *
 * \sa sasfit_magneticfieldalignedvesicle.h, form_fac
 */
sasfit_magneticfieldalignedvesicle_DLLEXP scalar sasfit_ff_magneticfieldalignedvesicle_p2psi(scalar q, sasfit_param * p);

/**
 * \ingroup ff_magneticfieldalignedvesicle_p2psi
 *
 * \sa sasfit_magneticfieldalignedvesicle.h, form_fac
 */
sasfit_magneticfieldalignedvesicle_DLLEXP scalar sasfit_ff_magneticfieldalignedvesicle_p2psi_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_magneticfieldalignedvesicle_p2psi
 *
 * \sa sasfit_magneticfieldalignedvesicle.h, form_fac
 */
sasfit_magneticfieldalignedvesicle_DLLEXP scalar sasfit_ff_magneticfieldalignedvesicle_p2psi_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_magneticfieldalignedvesicle_p2psi ################ */

/* ################ start ff_magneticfieldalignedvesicle_psi ################ */
/** 
 * \defgroup ff_magneticfieldalignedvesicle_psi MagneticFieldAlignedVesiclePsi
 * \ingroup ff_plugins
 *
 * \brief \<some brief description of MagneticFieldAlignedVesiclePsi function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rsh</td>
 *       <td>radius of spherical vesicle shell</td>
 *      </tr><tr>
 *       <td>\b theta1</td>
 *       <td>angle to describe size of first capped side in units of deg</td>
 *      </tr><tr>
 *       <td>\b theta2</td>
 *       <td>angle to describe size of second capped side in units of deg</td>
 *      </tr><tr>
 *       <td>\b t_sh</td>
 *       <td>thickness of spherical vesicle shell</td>
 *      </tr><tr>
 *       <td>\b t_c</td>
 *       <td>thickness of first flat capped side</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>scattering length density of spherical vesicle shell</td>
 *      </tr><tr>
 *       <td>\b eta_c</td>
 *       <td>scattering length density of capped sides</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>scattering length density of solvent</td> 
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>direction of the scattering vector in units of deg</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_magneticfieldalignedvesicle_psi
 *
 * \sa sasfit_magneticfieldalignedvesicle.h, example_group
 */
sasfit_magneticfieldalignedvesicle_DLLEXP scalar sasfit_ff_magneticfieldalignedvesicle_psi(scalar q, sasfit_param * p);

/**
 * \ingroup ff_magneticfieldalignedvesicle_psi
 *
 * \sa sasfit_magneticfieldalignedvesicle.h, example_group
 */
sasfit_magneticfieldalignedvesicle_DLLEXP scalar sasfit_ff_magneticfieldalignedvesicle_psi_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_magneticfieldalignedvesicle_psi
 *
 * \sa sasfit_magneticfieldalignedvesicle.h, example_group
 */
sasfit_magneticfieldalignedvesicle_DLLEXP scalar sasfit_ff_magneticfieldalignedvesicle_psi_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_magneticfieldalignedvesicle_psi ################ */



/** 
 * \defgroup ff_magneticfieldalignedvesicle_psi_new MagneticFieldAlignedVesiclePsiNew
 * \ingroup ff_plugins
 *
 * \brief \<some brief description of MagneticFieldAlignedVesiclePsiNew function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rsh</td>
 *       <td>inner radius of spherical vesicle shell</td>
 *      </tr><tr>
 *       <td>\b theta1</td>
 *       <td>angle to describe size of first capped side in units of deg</td>
 *      </tr><tr>
 *       <td>\b R_1</td>
 *       <td>radius of curvature of first capped side</td>
 *      </tr><tr>
 *       <td>\b theta2</td>
 *       <td>angle to describe size of second capped side in units of deg</td>
 *      </tr><tr>
 *       <td>\b R_2</td>
 *       <td>radius of curvature of second capped side</td>
 *      </tr><tr>
 *       <td>\b t_sh</td>
 *       <td>thickness of spherical vesicle shell</td>
 *      </tr><tr>
 *       <td>\b t_c</td>
 *       <td>thickness of curved capped side</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>scattering length density of spherical vesicle shell</td>
 *      </tr><tr>
 *       <td>\b eta_c</td>
 *       <td>scattering length density of capped sides</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>scattering length density of solvent</td> 
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>direction of the scattering vector in units of deg</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width of lognormal size distribution</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>orientation parameter</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_magneticfieldalignedvesicle_psi_new
 *
 * \sa sasfit_magneticfieldalignedvesicle.h, example_group
 */
sasfit_magneticfieldalignedvesicle_DLLEXP scalar sasfit_ff_magneticfieldalignedvesicle_psi_new(scalar q, sasfit_param * p);

/**
 * \ingroup ff_magneticfieldalignedvesicle_psi_new
 *
 * \sa sasfit_magneticfieldalignedvesicle.h, example_group
 */
sasfit_magneticfieldalignedvesicle_DLLEXP scalar sasfit_ff_magneticfieldalignedvesicle_psi_new_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_magneticfieldalignedvesicle_psi_new
 *
 * \sa sasfit_magneticfieldalignedvesicle.h, example_group
 */
sasfit_magneticfieldalignedvesicle_DLLEXP scalar sasfit_ff_magneticfieldalignedvesicle_psi_new_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_magneticfieldalignedvesicle_psi_new ################ */

#endif 

