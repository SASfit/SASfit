/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_SPHERICAL_ELLIPSOIDAL_MICELLES_H
#define SASFIT_PLUGIN_SPHERICAL_ELLIPSOIDAL_MICELLES_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_spherical_ellipsoidal_micelles.h
 * Public available functions and descriptions of the spherical_ellipsoidal_micelles plugin.
 */

/**
 * \def sasfit_spherical_ellipsoidal_micelles_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_spherical_ellipsoidal_micelles_EXPORTS)
	#ifdef sasfit_spherical_ellipsoidal_micelles_DLLEXP
	#undef sasfit_spherical_ellipsoidal_micelles_DLLEXP
	#endif
	#define sasfit_spherical_ellipsoidal_micelles_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_spherical_ellipsoidal_micelles_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_spherical_ellipsoidal_micelles_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_sphere_chains_rw__nagg ################ */
/**
 * \defgroup ff_sphere_chains_rw__nagg sphere+chains(RW)_Nagg
 * \ingroup ff_spherical_micelles
 *
 * \brief sphere with Gaussian chains attached, the micelle consists of block copolymers, the core radius R_core is calculated from the aggregation number Nagg and the molecular volume V_core of the copolymer block unit in the core.
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Nagg</td>
 *       <td>aggregation number of copolymer molecules per micelle</td>
 *      </tr><tr>
 *       <td>\b V_core</td>
 *       <td>molecular volume of the copolymer block unit inside core in [nm^3] for Q in nm^-1 and in [A^3] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>molecular volume of the copolymer block unit in corona in [nm^3] for Q in nm^-1 and in [A^3] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of spherical core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of a Gaussian chain in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solvc</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b xsolv_core</td>
 *       <td>amount of solvent in core</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>gyration radius of polymer chains in the corona</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>value should be around 1, non-penetration of the chains into the core is mimicked by d~1 for R_core>>Rg</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_sphere_chains_rw__nagg
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_chains_rw__nagg(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere_chains_rw__nagg
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_chains_rw__nagg_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere_chains_rw__nagg
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_chains_rw__nagg_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_sphere_chains_rw__nagg ################ */

/* ################ start ff_sphere_chains_rw_ ################ */
/**
 * \defgroup ff_sphere_chains_rw_ sphere+Chains(RW)
 * \ingroup ff_spherical_micelles
 *
 * \brief sphere with Gaussian chains attached, in contrast to the form factor *_Rc and *_Nagg this one does not necassary consist of copolymers
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_core</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b n_agg</td>
 *       <td>specific aggregation number, number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>molecular volume of the copolymer block unit in corona in [nm^3] for Q in nm^-1 and in [A^3] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of spherical core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of a Gaussian chain in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solvc</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b xsolv_core</td>
 *       <td>amount of solvent in core</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>gyration radius of polymer chains in the corona</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>value should be around 1, non-penetration of the chains into the core is mimicked by d~1 for R_core>>Rg</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_sphere_chains_rw_
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_chains_rw_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere_chains_rw_
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_chains_rw__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere_chains_rw_
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_chains_rw__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_sphere_chains_rw_ ################ */

/* ################ start ff_sphere_chains_rw__rc ################ */
/**
 * \defgroup ff_sphere_chains_rw__rc sphere+chains(RW)_Rc
 * \ingroup ff_spherical_micelles
 *
 * \brief sphere with Gaussian chains attached, the micelle consists of block copolymers, the aggregation number Nagg is calculated from the core radius R_core and the molecular volume V_core of the copolymer block unit in the core
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_core</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b V_core</td>
 *       <td>molecular volume of the copolymer block unit inside core in [nm^3] for Q in nm^-1 and in [A^3] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>molecular volume of the copolymer block unit in corona in [nm^3] for Q in nm^-1 and in [A^3] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of spherical core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of a Gaussian chain in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solvc</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b xsolv_core</td>
 *       <td>amount of solvent in core</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>gyration radius of polymer chains in the corona</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>value should be around 1, non-penetration of the chains into the core is mimicked by d~1 for R_core>>Rg</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_sphere_chains_rw__rc
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_chains_rw__rc(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere_chains_rw__rc
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_chains_rw__rc_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere_chains_rw__rc
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_chains_rw__rc_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_sphere_chains_rw__rc ################ */

/* ################ start ff_sphere_chains_saw__nagg ################ */
/**
 * \defgroup ff_sphere_chains_saw__nagg sphere+chains(SAW)_Nagg
 * \ingroup ff_spherical_micelles
 *
 * \brief \<some brief description of sphere+chains(SAW)_Nagg function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Nagg</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_sphere_chains_saw__nagg
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_chains_saw__nagg(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere_chains_saw__nagg
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_chains_saw__nagg_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere_chains_saw__nagg
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_chains_saw__nagg_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_sphere_chains_saw__nagg ################ */

/* ################ start ff_sphere_chains_saw_ ################ */
/**
 * \defgroup ff_sphere_chains_saw_ sphere+chains(SAW)
 * \ingroup ff_spherical_micelles
 *
 * \brief \<some brief description of sphere+chains(SAW) function\>
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
 * \ingroup ff_sphere_chains_saw_
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_chains_saw_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere_chains_saw_
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_chains_saw__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere_chains_saw_
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_chains_saw__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_sphere_chains_saw_ ################ */

/* ################ start ff_sphere_chains_saw__rc ################ */
/**
 * \defgroup ff_sphere_chains_saw__rc sphere+chains(SAW)_Rc
 * \ingroup ff_spherical_micelles
 *
 * \brief \<some brief description of sphere+chains(SAW)_Rc function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_core</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b n_agg</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_sphere_chains_saw__rc
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_chains_saw__rc(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere_chains_saw__rc
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_chains_saw__rc_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere_chains_saw__rc
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_chains_saw__rc_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_sphere_chains_saw__rc ################ */

/* ################ start ff_sphere_r__a_nagg ################ */
/**
 * \defgroup ff_sphere_r__a_nagg sphere+R^-a_Nagg
 * \ingroup ff_spherical_micelles
 *
 * \brief sphere with a radial profile r^-alpha, the micelle consists of block copolymers, the core radius R_core is calculated from the aggregation number Nagg and the molecular volume V_core of the copolymer block unit in the core
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Nagg</td>
 *       <td>aggregation number of copolymer molecules per micelle</td>
 *      </tr><tr>
 *       <td>\b V_core</td>
 *       <td>molecular volume of the copolymer block unit inside core in [nm^3] for Q in nm^-1 and in [A^3] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>molecular volume of the copolymer block unit in corona in [nm^3] for Q in nm^-1 and in [A^3] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of spherical core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of a Gaussian chain in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>exponent of radial profile (r^-alpha), alpha=2: polyelectrolyte, alpha=4/3: star-like polymer, alpha=0: compact shell</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>contour length of polymer in shell</td>
 *      </tr><tr>
 *       <td>\b b</td>
 *       <td>Kuhn length of polymer in shell</td>
 *      </tr><tr>
 *       <td>\b xsolv_core</td>
 *       <td>amount of solvent in core</td>
 *      </tr>
 */

/**
 * \ingroup ff_sphere_r__a_nagg
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_r__a_nagg(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere_r__a_nagg
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_r__a_nagg_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere_r__a_nagg
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_r__a_nagg_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_sphere_r__a_nagg ################ */

/* ################ start ff_sphere_r__a ################ */
/**
 * \defgroup ff_sphere_r__a sphere+R^-a
 * \ingroup ff_spherical_micelles
 *
 * \brief <sphere with a radial profile r^-alpha, in contrast to the form factor *_Rc and *_Nagg this one does not neccassary consist of copolymers>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_core</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b n_agg</td>
 *       <td>specific aggregation number, number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>molecular volume of the copolymer block unit in corona in [nm^3] for Q in nm^-1 and in [A^3] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of spherical core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of a Gaussian chain in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>exponent of radial profile (r^-alpha), alpha=2: polyelectrolyte, alpha=4/3: star-like polymer, alpha=0: compact shell</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>contour length of polymer in shell</td>
 *      </tr><tr>
 *       <td>\b b</td>
 *       <td>Kuhn length of polymer in shell</td>
 *      </tr><tr>
 *       <td>\b xsolv_core</td>
 *       <td>amount of solvent in core</td>
 *      </tr>
 */

/**
 * \ingroup ff_sphere_r__a
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_r__a(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere_r__a
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_r__a_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere_r__a
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_r__a_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_sphere_r__a ################ */

/* ################ start ff_sphere_r__a_rc ################ */
/**
 * \defgroup ff_sphere_r__a_rc sphere+R^-a_Rc
 * \ingroup ff_spherical_micelles
 *
 * \brief sphere with a radial profile of r^-alpha, the micelle consists of block copolymers, the aggregation number Nagg is calculated from the core radius R_core and the molecular volume V_core of the copolymer block unit in the core
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_core</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b V_core</td>
 *       <td>molecular volume of the copolymer block unit inside core in [nm^3] for Q in nm^-1 and in [A^3] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>molecular volume of the copolymer block unit in corona in [nm^3] for Q in nm^-1 and in [A^3] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of spherical core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of a Gaussian chain in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>exponent of radial profile (r^-alpha), alpha=2: polyelectrolyte, alpha=4/3: star-like polymer, alpha=0: compact shell</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>contour length of polymer in shell</td>
 *      </tr><tr>
 *       <td>\b b</td>
 *       <td>Kuhn length of polymer in shell</td>
 *      </tr><tr>
 *       <td>\b xsolv_core</td>
 *       <td>amount of solvent in core</td>
 *      </tr>
 */

/**
 * \ingroup ff_sphere_r__a_rc
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_r__a_rc(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere_r__a_rc
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_r__a_rc_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere_r__a_rc
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_r__a_rc_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_sphere_r__a_rc ################ */

/* ################ start ff_sphere_smooth_interface_r__a_nagg ################ */
/**
 * \defgroup ff_sphere_smooth_interface_r__a_nagg sphere_smooth_interface+R^-a_Nagg
 * \ingroup ff_spherical_micelles
 *
 * \brief sphere with a radial profile r^-alpha, the micelle consists of block copolymers, the core radius R_core is calculated from the aggregation number Nagg and the molecular volume V_core of the copolymer block unit in the core
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N_agg</td>
 *       <td>aggregation number of copolymer molecules per micelle</td>
 *      </tr><tr>
 *       <td>\b V_core</td>
 *       <td>molecular volume of the copolymer block unit inside core in [nm^3] for Q in nm^-1 and in [A^3] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>molecular volume of the copolymer block unit in corona in [nm^3] for Q in nm^-1 and in [A^3] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of spherical core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of a Gaussian chain in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>exponent of radial profile (r^-alpha), alpha=2: polyelectrolyte, alpha=4/3: star-like polymer, alpha=0: compact shell</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>contour length of polymer in shell</td>
 *      </tr><tr>
 *       <td>\b b</td>
 *       <td>Kuhn length of polymer in shell</td>
 *      </tr>
 */

/**
 * \ingroup ff_sphere_smooth_interface_r__a_nagg
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_smooth_interface_r__a_nagg(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere_smooth_interface_r__a_nagg
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_smooth_interface_r__a_nagg_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere_smooth_interface_r__a_nagg
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_smooth_interface_r__a_nagg_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_sphere_smooth_interface_r__a_nagg ################ */

/* ################ start ff_sphere_2nd_r__a_rc ################ */
/**
 * \defgroup ff_sphere_2nd_r__a_rc sphere_2nd+R^-a_Rc
 * \ingroup ff_spherical_micelles
 *
 * \brief \<some brief description of sphere_2nd+R^-a_Rc function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_core</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b V_tot</td>
 *       <td>total molecular volume of the copolymer in [nm^3] for Q in nm^-1 and in [A^3] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>molecular volume of the copolymer block unit in corona in [nm^3] for Q in nm^-1 and in [A^3] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of spherical core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of a Gaussian chain in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>exponent of radial profile (r^-alpha), alpha=2: polyelectrolyte, alpha=4/3: star-like polymer, alpha=0: compact shell</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>contour length of polymer in shell</td>
 *      </tr><tr>
 *       <td>\b b</td>
 *       <td>Kuhn length of polymer in shell</td>
 *      </tr><tr>
 *       <td>\b xsolv_core</td>
 *       <td>amount of solvent in core</td>
 *      </tr>
 */

/**
 * \ingroup ff_sphere_2nd_r__a_rc
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_2nd_r__a_rc(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere_2nd_r__a_rc
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_2nd_r__a_rc_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphere_smooth_interface_r__a_rc
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_sphere_2nd_r__a_rc_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_sphere_2nd_r__a_rc ################ */

/* ################ start ff_ell_chains_rw__nagg ################ */
/**
 * \defgroup ff_ell_chains_rw__nagg ell+chains(RW)_Nagg
 * \ingroup ff_spherical_micelles
 *
 * \brief ellipsoid with Gaussian chains attached, the micelle consists of block copolymers, the core radius R_core is calculated from the aggregation number Nagg and the molecular volume V_core of the copolymer block unit in the core
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N_agg</td>
 *       <td>aggregation number of copolymer molecules per micelle</td>
 *      </tr><tr>
 *       <td>\b V_core</td>
 *       <td>molecular volume of the copolymer block unit inside core in [nm^3] for Q in nm^-1 and in [A^3] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>molecular volume of the copolymer block unit in corona in [nm^3] for Q in nm^-1 and in [A^3] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of ellipsoidal core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of a Gaussian chain in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b xsolv_core</td>
 *       <td>amount of solvent in core</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>gyration radius of polymer chains in the corona</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>value should be around 1, non-penetration of the chains into the core is mimicked by d~1 for R_core>>Rg</td>
 *      </tr><tr>
 *       <td>\b epsilon</td>
 *       <td>eccentricity of the ellipsoid (R,R,epsilon*R)</td>
 *      </tr>
 */

/**
 * \ingroup ff_ell_chains_rw__nagg
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_ell_chains_rw__nagg(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ell_chains_rw__nagg
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_ell_chains_rw__nagg_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ell_chains_rw__nagg
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_ell_chains_rw__nagg_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ell_chains_rw__nagg ################ */

/* ################ start ff_ell_chains_rw_ ################ */
/**
 * \defgroup ff_ell_chains_rw_ ell+chains(RW)
 * \ingroup ff_spherical_micelles
 *
 * \brief ellipsoid with Gaussian chains attached, in contrast to the form factor *_Rc and *_Nagg this one does not necessary consist of copolymers
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_core</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b n_agg</td>
 *       <td>specific aggregation number, number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>molecular volume of the copolymer block unit in corona in [nm^3] for Q in nm^-1 and in [A^3] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of ellipsoidal core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of a Gaussian chain in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b xsolv_core</td>
 *       <td>amount of solvent in core</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>gyration radius of polymer chains in the corona</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>value should be around 1, non-penetration of the chains into the core is mimicked by d~1 for R_core>>Rg</td>
 *      </tr><tr>
 *       <td>\b epsilon</td>
 *       <td>eccentricity of the ellipsoid (R,R,epsilon*R)</td>
 *      </tr>
 */

/**
 * \ingroup ff_ell_chains_rw_
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_ell_chains_rw_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ell_chains_rw_
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_ell_chains_rw__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ell_chains_rw_
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_ell_chains_rw__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ell_chains_rw_ ################ */

/* ################ start ff_ell_chains_rw__rc ################ */
/**
 * \defgroup ff_ell_chains_rw__rc ell+chains(RW)_Rc
 * \ingroup ff_spherical_micelles
 *
 * \brief ellipsoid with Gaussian chains attached, the micelle consists of block copolymers, the aggregation number Nagg is calculated from the core radius R_core and the molecular volume V_core of the copolymer block unit in the core
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_core</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b V_core</td>
 *       <td>molecular volume of the copolymer block unit inside core in [nm^3] for Q in nm^-1 and in [A^3] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>molecular volume of the copolymer block unit in corona in [nm^3] for Q in nm^-1 and in [A^3] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of ellipsoidal core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of a Gaussian chain in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b xsolv_core</td>
 *       <td>amount of solvent in core</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>gyration radius of polymer chains in the corona</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>value should be around 1, non-penetration of the chains into the core is mimicked by d~1 for R_core>>Rg</td>
 *      </tr><tr>
 *       <td>\b epsilon</td>
 *       <td>eccentricity of the ellipsoid (R,R,epsilon*R)</td>
 *      </tr>
 */

/**
 * \ingroup ff_ell_chains_rw__rc
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_ell_chains_rw__rc(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ell_chains_rw__rc
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_ell_chains_rw__rc_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ell_chains_rw__rc
 *
 * \sa sasfit_spherical_ellipsoidal_micelles.h, ff_spherical_micelles
 */
sasfit_spherical_ellipsoidal_micelles_DLLEXP scalar sasfit_ff_ell_chains_rw__rc_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ell_chains_rw__rc ################ */


#endif // this file

