/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_SPHERICALSHELLS_H
#define SASFIT_PLUGIN_SPHERICALSHELLS_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_sphericalshells.h
 * Public available functions and descriptions of the sphericalshells plugin.
 */

/**
 * \def sasfit_sphericalshells_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_sphericalshells_EXPORTS)
	#ifdef sasfit_sphericalshells_DLLEXP
	#undef sasfit_sphericalshells_DLLEXP
	#endif
	#define sasfit_sphericalshells_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_sphericalshells_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_sphericalshells_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_multilamellar_vesicle ################ */
/**
 * \defgroup ff_multilamellar_vesicle multilamellar vesicle
 * \ingroup ff_plugins_spheres_shells
 *
 * \brief \<some brief description of multilamellar vesicle function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_c</td>
 *       <td>core radius containing solvent</td>
 *      </tr><tr>
 *       <td>\b t_sh</td>
 *       <td>layer thickness</td>
 *      </tr><tr>
 *       <td>\b t_sol</td>
 *       <td>thickness of solvent layer</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b n</td>
 *       <td>number of layers</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_multilamellar_vesicle
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_multilamellar_vesicle(scalar q, sasfit_param * p);

/**
 * \ingroup ff_multilamellar_vesicle
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_multilamellar_vesicle_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_multilamellar_vesicle
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_multilamellar_vesicle_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_multilamellar_vesicle ################ */

/* ################ start ff_spherical_shell_iii ################ */
/**
 * \defgroup ff_spherical_shell_iii spherical shell iii
 * \ingroup ff_plugins_spheres_shells
 *
 * \brief \<some brief description of spherical shell iii function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>inner radius</td>
 *      </tr><tr>
 *       <td>\b dR</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b eta1</td>
 *       <td>scattering contrast of core</td>
 *      </tr><tr>
 *       <td>\b eta2</td>
 *       <td>scattering contrast of shell</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_spherical_shell_iii
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_spherical_shell_iii(scalar q, sasfit_param * p);

/**
 * \ingroup ff_spherical_shell_iii
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_spherical_shell_iii_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_spherical_shell_iii
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_spherical_shell_iii_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_spherical_shell_iii ################ */

/* ################ start ff_spherical_shell_ii ################ */
/**
 * \defgroup ff_spherical_shell_ii spherical shell ii
 * \ingroup ff_plugins_spheres_shells
 *
 * \brief \<some brief description of spherical shell ii function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>outer radius</td>
 *      </tr><tr>
 *       <td>\b nu</td>
 *       <td>nu*R: inner radius</td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td>mu*eta: scattering contrast of inner core</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering contrast of shell</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_spherical_shell_ii
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_spherical_shell_ii(scalar q, sasfit_param * p);

/**
 * \ingroup ff_spherical_shell_ii
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_spherical_shell_ii_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_spherical_shell_ii
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_spherical_shell_ii_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_spherical_shell_ii ################ */

/* ################ start ff_spherical_shell_i ################ */
/**
 * \defgroup ff_spherical_shell_i spherical shell i
 * \ingroup ff_plugins_spheres_shells
 *
 * \brief \<some brief description of spherical shell i function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R1</td>
 *       <td>outer radius</td>
 *      </tr><tr>
 *       <td>\b R2</td>
 *       <td>inner radius</td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td>mu*eta: scattering contrast of inner core</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>eta: scattering contrast of shell</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_spherical_shell_i
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_spherical_shell_i(scalar q, sasfit_param * p);

/**
 * \ingroup ff_spherical_shell_i
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_spherical_shell_i_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_spherical_shell_i
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_spherical_shell_i_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_spherical_shell_i ################ */

/* ################ start ff_sphere ################ */
/**
 * \defgroup ff_sphere sphere
 * \ingroup ff_plugins_spheres_shells
 *
 * \brief \<some brief description of sphere function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering_contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_sphere
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_sphere ################ */

/* ################ start ff_rnd_multilamellar_vesicle ################ */
/**
 * \defgroup ff_rnd_multilamellar_vesicle rnd multilamellar vesicle
 * \ingroup ff_plugins_spheres_shells
 *
 * \brief \<some brief description of rnd multilamellar vesicle function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_c</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_rnd_multilamellar_vesicle
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_rnd_multilamellar_vesicle(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rnd_multilamellar_vesicle
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_rnd_multilamellar_vesicle_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rnd_multilamellar_vesicle
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_rnd_multilamellar_vesicle_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rnd_multilamellar_vesicle ################ */

/* ################ start ff_rnd_multilamellar_vesicle_2 ################ */
/**
 * \defgroup ff_rnd_multilamellar_vesicle_2 rnd multilamellar vesicle 2
 * \ingroup ff_plugins_spheres_shells
 *
 * \brief \<some brief description of rnd multilamellar vesicle 2 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_c</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_rnd_multilamellar_vesicle_2
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_rnd_multilamellar_vesicle_2(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rnd_multilamellar_vesicle_2
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_rnd_multilamellar_vesicle_2_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rnd_multilamellar_vesicle_2
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_rnd_multilamellar_vesicle_2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rnd_multilamellar_vesicle_2 ################ */

/* ################ start ff_mlv_frielinghaus ################ */
/**
 * \defgroup ff_mlv_frielinghaus MLV Frielinghaus
 * \ingroup ff_plugins_spheres_shells
 *
 * \brief \<some brief description of MLV Frielinghaus function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b n</td>
 *       <td>number of layers</td>
 *      </tr><tr>
 *       <td>\b nw</td>
 *       <td>width of distribution</td>
 *      </tr><tr>
 *       <td>\b rhoW</td>
 *       <td>scattering length density of waterf</td>
 *      </tr><tr>
 *       <td>\b rho1</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b rho2</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b sig1</td>
 *       <td>core thickness</td>
 *      </tr><tr>
 *       <td>\b sig2</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b Rmain</td>
 *       <td>distances between layers</td>
 *      </tr><tr>
 *       <td>\b zz</td>
 *       <td>Schultz Z</td>
 *      </tr><tr>
 *       <td>\b Rshift</td>
 *       <td>maximum possible shifts</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_mlv_frielinghaus
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_mlv_frielinghaus(scalar q, sasfit_param * p);

/**
 * \ingroup ff_mlv_frielinghaus
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_mlv_frielinghaus_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_mlv_frielinghaus
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_mlv_frielinghaus_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_mlv_frielinghaus ################ */

/* ################ start ff_bilayered_vesicle ################ */
/**
 * \defgroup ff_bilayered_vesicle bilayered vesicle
 * \ingroup ff_plugins_spheres_shells
 *
 * \brief \<some brief description of bilayered vesicle function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rc</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_bilayered_vesicle
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_bilayered_vesicle(scalar q, sasfit_param * p);

/**
 * \ingroup ff_bilayered_vesicle
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_bilayered_vesicle_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_bilayered_vesicle
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_bilayered_vesicle_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_bilayered_vesicle ################ */

/* ################ start ff_hollow_sphere ################ */
/**
 * \defgroup ff_hollow_sphere hollow sphere
 * \ingroup ff_plugins_spheres_shells
 *
 * \brief \<some brief description of hollow sphere function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>core radius (solvent)</td>
 *      </tr><tr>
 *       <td>\b dR</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_hollow_sphere
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_hollow_sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_hollow_sphere
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_hollow_sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_hollow_sphere
 *
 * \sa sasfit_sphericalshells.h, ff_plugins_spheres_shells
 */
sasfit_sphericalshells_DLLEXP scalar sasfit_ff_hollow_sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_hollow_sphere ################ */


#endif // this file

