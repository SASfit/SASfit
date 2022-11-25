/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_THIN_OBJECTS_H
#define SASFIT_PLUGIN_THIN_OBJECTS_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_thin_objects.h
 * Public available functions and descriptions of the thin_objects plugin.
 */

/**
 * \def sasfit_thin_objects_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_thin_objects_EXPORTS)
	#ifdef sasfit_thin_objects_DLLEXP
	#undef sasfit_thin_objects_DLLEXP
	#endif
	#define sasfit_thin_objects_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_thin_objects_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_thin_objects_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_disc_sd_homoxs ################ */
/**
 * \defgroup ff_disc_sd_homoxs Disc+SD+homoXS
 * \ingroup ff_plugins_localplanar
 *
 * \brief form factor of a disc with a size distribution and a homogeneous cross-section
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b t</td>
 *       <td>most probable layer thickness</td>
 *      </tr><tr>
 *       <td>\b sigma_t</td>
 *       <td>width of thickness distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b R0</td>
 *       <td>most probable  disc radius</td>
 *      </tr><tr>
 *       <td>\b sigma_R0</td>
 *       <td>width of radius distribution (LogNorm) </td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta_l</td>
 *       <td>scattering length density of layer</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_disc_sd_homoxs
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_disc_sd_homoxs(scalar q, sasfit_param * p);

/**
 * \ingroup ff_disc_sd_homoxs
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_disc_sd_homoxs_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_disc_sd_homoxs
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_disc_sd_homoxs_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_disc_sd_homoxs ################ */

/* ################ start ff_ellsh_sd_homoxs ################ */
/**
 * \defgroup ff_ellsh_sd_homoxs EllSh+SD+homoXS
 * \ingroup ff_plugins_localplanar
 *
 * \brief form factor of a thin ellipsoidal shell with a size distribution and a homogeneous cross section function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b t</td>
 *       <td>most probable layer thickness</td>
 *      </tr><tr>
 *       <td>\b sigma_t</td>
 *       <td>width of thickness distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b R0</td>
 *       <td>most probable radius of unilamellar vesicle</td>
 *      </tr><tr>
 *       <td>\b sigma_R0</td>
 *       <td>width of radius distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b epsilon</td>
 *       <td>stretching factor</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta_l</td>
 *       <td>scattering length density of layer</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ellsh_sd_homoxs
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_ellsh_sd_homoxs(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellsh_sd_homoxs
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_ellsh_sd_homoxs_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellsh_sd_homoxs
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_ellsh_sd_homoxs_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ellsh_sd_homoxs ################ */

/* ################ start ff_cylsh_sd_homoxs ################ */
/**
 * \defgroup ff_cylsh_sd_homoxs CylSh+SD+homoXS (closed)
 * \ingroup ff_plugins_localplanar
 *
 * \brief form factor of a thin hollow cylinder with closed end and a size distribution and a homogeneous cross-section
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b t</td>
 *       <td>most probable layer thickness</td>
 *      </tr><tr>
 *       <td>\b sigma_t</td>
 *       <td>width of thickness distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b R</td>
 *       <td>most probable cylinder radius of unilamellar vesicle</td>
 *      </tr><tr>
 *       <td>\b sigma_R</td>
 *       <td>width of cylinder radius distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b H</td>
 *       <td>most probable cylinder heightof unilamellar vesicle</td>
 *      </tr><tr>
 *       <td>\b sigma_H</td>
 *       <td>width of cylinder height distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b eta_l</td>
 *       <td>scattering length density of layer</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_cylsh_sd_homoxs
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_cylsh_sd_homoxs(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylsh_sd_homoxs
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_cylsh_sd_homoxs_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylsh_sd_homoxs
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_cylsh_sd_homoxs_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_cylsh_sd_homoxs ################ */

/* ################ start ff_disc_sd_bilayergauss ################ */
/**
 * \defgroup ff_disc_sd_bilayergauss Disc+SD+BiLayerGauss
 * \ingroup ff_plugins_localplanar
 *
 * \brief \<some brief description of Disc+SD+BiLayerGauss function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b t</td>
 *       <td>most probable distance between layer centers</td>
 *      </tr><tr>
 *       <td>\b sigma_t</td>
 *       <td>width of distance  distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b R0</td>
 *       <td>most probable  disc radius</td>
 *      </tr><tr>
 *       <td>\b sigma_R0</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_out</td>
 *       <td>width of scattering density distribution in the head groups of the lipids forming the outer layers of the bilayer</td>
 *      </tr><tr>
 *       <td>\b b_out</td>
 *       <td>excess scattering length of the head groups</td>
 *      </tr><tr>
 *       <td>\b sigma_core</td>
 *       <td>width of scattering density distribution in the tail groups of the lipids forming the core of the bilayer</td>
 *      </tr><tr>
 *       <td>\b b_core</td>
 *       <td>excess scattering length of the tail groups</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_disc_sd_bilayergauss
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_disc_sd_bilayergauss(scalar q, sasfit_param * p);

/**
 * \ingroup ff_disc_sd_bilayergauss
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_disc_sd_bilayergauss_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_disc_sd_bilayergauss
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_disc_sd_bilayergauss_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_disc_sd_bilayergauss ################ */

/* ################ start ff_ellsh_sd_bilayergauss ################ */
/**
 * \defgroup ff_ellsh_sd_bilayergauss EllSh+SD+BiLayerGauss
 * \ingroup ff_plugins_localplanar
 *
 * \brief \<some brief description of EllSh+SD+BiLayerGauss function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b t</td>
 *       <td>most probable layer thickness</td>
 *      </tr><tr>
 *       <td>\b sigma_t</td>
 *       <td>width of thickness distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b R0</td>
 *       <td>most probable radius of unilamellar vesicle</td>
 *      </tr><tr>
 *       <td>\b sigma_R0</td>
 *       <td>width of radius distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b epsilon</td>
 *       <td>stretching factor</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_out</td>
 *       <td>width of scattering density distribution in the head groups of the lipids forming the outer layers of the bilayer</td>
 *      </tr><tr>
 *       <td>\b b_out</td>
 *       <td>excess scattering length of the head groups</td>
 *      </tr><tr>
 *       <td>\b sigma_core</td>
 *       <td>width of scattering density distribution in the tail groups of the lipids forming the core of the bilayer</td>
 *      </tr><tr>
 *       <td>\b b_core</td>
 *       <td>excess scattering length of the tail groups</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ellsh_sd_bilayergauss
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_ellsh_sd_bilayergauss(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellsh_sd_bilayergauss
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_ellsh_sd_bilayergauss_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellsh_sd_bilayergauss
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_ellsh_sd_bilayergauss_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ellsh_sd_bilayergauss ################ */

/* ################ start ff_cylsh_sd_bilayergauss ################ */
/**
 * \defgroup ff_cylsh_sd_bilayergauss CylSh+SD+BiLayerGauss
 * \ingroup ff_plugins_localplanar
 *
 * \brief \<some brief description of CylSh+SD+BiLayerGauss function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b t</td>
 *       <td>most probable distance between layer centers</td>
 *      </tr><tr>
 *       <td>\b sigma_t</td>
 *       <td>width of distance  distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b R0</td>
 *       <td>most probable radius of unilamellar cylindrical vesicle</td>
 *      </tr><tr>
 *       <td>\b sigma_R0</td>
 *       <td>width of radius distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b H0</td>
 *       <td>most probable height of unilamellar cylindrical vesicle</td>
 *      </tr><tr>
 *       <td>\b sigma_H0</td>
 *       <td>width of hight distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b sigma_out</td>
 *       <td>width of scattering density distribution in the head groups of the lipids forming the outer layers of the bilayer</td>
 *      </tr><tr>
 *       <td>\b b_out</td>
 *       <td>excess scattering length of the head groups</td>
 *      </tr><tr>
 *       <td>\b sigma_core</td>
 *       <td>width of scattering density distribution in the tail groups of the lipids forming the core of the bilayer</td>
 *      </tr><tr>
 *       <td>\b b_core</td>
 *       <td>excess scattering length of the tail groups</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_cylsh_sd_bilayergauss
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_cylsh_sd_bilayergauss(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylsh_sd_bilayergauss
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_cylsh_sd_bilayergauss_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylsh_sd_bilayergauss
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_cylsh_sd_bilayergauss_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_cylsh_sd_bilayergauss ################ */

/* ################ start ff_pcs_homogeneousplate ################ */
/**
 * \defgroup ff_pcs_homogeneousplate Pcs:homogeneousPlate
 * \ingroup ff_plugins_pcslocalplanar
 *
 * \brief cross section form factor of a homogeneous plane
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b t</td>
 *       <td>most probable thickness</td>
 *      </tr><tr>
 *       <td>\b sigma_t</td>
 *       <td>width of thickness distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta_L</td>
 *       <td>scattering length density of layer/td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_pcs_homogeneousplate
 *
 * \sa sasfit_thin_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_pcs_homogeneousplate(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_homogeneousplate
 *
 * \sa sasfit_thin_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_pcs_homogeneousplate_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_homogeneousplate
 *
 * \sa sasfit_thin_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_pcs_homogeneousplate_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_pcs_homogeneousplate ################ */


/* ################ start ff_pcs_plate_chains_rw_ ################ */
/**
 * \defgroup ff_pcs_plate_chains_rw_ Pcs:Plate+Chains(RW)
 * \ingroup ff_plugins_pcslocalplanar
 *
 * \brief cross-section form factor Pcs:Plate+Chains(RW) of a planar layer with Gaussian chains attached
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b L_core</td>
 *       <td>"L_core: thickness of the core of the planar layer</td>
 *      </tr><tr>
 *       <td>\b n_agg</td>
 *       <td>nagg: specific aggregation number\nnumber of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>V_brush: molecular volume of single chain in corona\n in \[nm^3\] for Q in nm^-1 or in \[A^3\] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>eta_core: scattering length density of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>eta_brush: scattering length density of a Gaussian chain in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>"eta_sol: scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b xsolv_core</td>
 *       <td>xsolv_core: amount of solvent in core</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>Rg: gyration radius of polymer chains in the corona</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>d: value should be around 1\nnon-penetration of the chains into the core is mimicked by d~1 for L_core>>Rg</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_pcs_plate_chains_rw_
 *
 * \sa sasfit_thin_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_pcs_plate_chains_rw_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_plate_chains_rw_
 *
 * \sa sasfit_thin_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_pcs_plate_chains_rw__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_plate_chains_rw_
 *
 * \sa sasfit_thin_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_pcs_plate_chains_rw__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_pcs_plate_chains_rw_ ################ */

/* ################ start ff_pcs_layeredcentrosymmetricxs ################ */
/**
 * \defgroup ff_pcs_layeredcentrosymmetricxs Pcs:LayeredCentroSymmetricXS
 * \ingroup ff_plugins_pcslocalplanar
 *
 * \brief cros section form factor with two infinitesimal thin centrosymmetric layers with a LogNorm layer distance distribution
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Lc</td>
 *       <td>most probable thickness of core</td>
 *      </tr><tr>
 *       <td>\b sigma_Lc</td>
 *       <td>width of core thickness distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b Lsh</td>
 *       <td>most probable thickshell thickness</td>
 *      </tr><tr>
 *       <td>\b sigma_Lsh</td>
 *       <td>width of shell thickness distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b eta_Lc</td>
 *       <td>scattering length density of core layer/td>
 *      </tr><tr>
 *       <td>\b eta_Lsh</td>
 *       <td>scattering length density of shell layer</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_pcs_layeredcentrosymmetricxs
 *
 * \sa sasfit_thin_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_pcs_layeredcentrosymmetricxs(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_layeredcentrosymmetricxs
 *
 * \sa sasfit_thin_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_pcs_layeredcentrosymmetricxs_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_layeredcentrosymmetricxs
 *
 * \sa sasfit_thin_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_pcs_layeredcentrosymmetricxs_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_pcs_layeredcentrosymmetricxs ################ */

/* ################ start ff_pcs_bilayergauss ################ */
/**
 * \defgroup ff_pcs_bilayergauss Pcs:BilayerGauss
 * \ingroup ff_plugins_pcslocalplanar
 *
 * \brief cross-section form factor Pcs(Q) for a bilayer with a Gaussian electron density profile
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b sigma_core</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density contrast of the central Gaussian profile </td>
 *      </tr><tr>
 *       <td>\b sigma_out</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta_out</td>
 *       <td>scattering length density contrast of the two outer Gaussian profiles</td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td>distance between the centers of the outer Gaussian profiles</td>
 *      </tr><tr>
 *       <td>\b sigma_D</td>
 *       <td>LogNormal distribution of the distance distance between the centers of the outer Gaussian profiles</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_pcs_bilayergauss
 *
 * \sa sasfit_thin_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_pcs_bilayergauss(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_bilayergauss
 *
 * \sa sasfit_thin_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_pcs_bilayergauss_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_bilayergauss
 *
 * \sa sasfit_thin_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_pcs_bilayergauss_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_pcs_bilayergauss ################ */

/* ################ start ff_pcs_twoinfinitelythinlayers ################ */
/**
 * \defgroup ff_pcs_twoinfinitelythinlayers Pcs:TwoInfinitelyThinLayers
 * \ingroup ff_plugins_pcslocalplanar
 *
 * \brief cross-section form factor of  two infinitely thin parallel layers
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b t</td>
 *       <td>layer distance</td>
 *      </tr><tr>
 *       <td>\b sigma_t</td>
 *       <td>width of distance distribution (LogNorm)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_pcs_twoinfinitelythinlayers
 *
 * \sa sasfit_thin_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_pcs_twoinfinitelythinlayers(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_twoinfinitelythinlayers
 *
 * \sa sasfit_thin_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_pcs_twoinfinitelythinlayers_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_twoinfinitelythinlayers
 *
 * \sa sasfit_thin_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_pcs_twoinfinitelythinlayers_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_pcs_twoinfinitelythinlayers ################ */

/* ################ start sq_p__q___thin_disc ################ */
/**
 * \defgroup sq_p__q___thin_disc P'(Q): Thin Disc
 * \ingroup sq_plugins_pprimelocalplanar
 *
 * \brief \<some brief description of P'(Q): Thin Disc function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>most probable radius</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width of radius distribution (LogNorm)</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_p__q___thin_disc
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___thin_disc(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___thin_disc
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___thin_disc_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___thin_disc
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___thin_disc_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_p__q___thin_disc ################ */

/* ################ start sq_p__q___thin_spherical_shell ################ */
/**
 * \defgroup sq_p__q___thin_spherical_shell P'(Q): Thin Spherical Shell
 * \ingroup sq_plugins_pprimelocalplanar
 *
 * \brief \<some brief description of P'(Q): Thin Spherical Shell function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>most probable radius</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width of radius distribution (LogNorm)</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_p__q___thin_spherical_shell
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___thin_spherical_shell(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___thin_spherical_shell
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___thin_spherical_shell_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___thin_spherical_shell
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___thin_spherical_shell_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_p__q___thin_spherical_shell ################ */

/* ################ start sq_p__q___thin_ellipsoidal_shell ################ */
/**
 * \defgroup sq_p__q___thin_ellipsoidal_shell P'(Q): Thin Ellipsoidal Shell
 * \ingroup sq_plugins_pprimelocalplanar
 *
 * \brief \<some brief description of P'(Q): Thin Ellipsoidal Shell function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>most probable radius</td>
 *      </tr><tr>
 *       <td>\b epsilon</td>
 *       <td>stretching factor</td>
 *      </tr><tr>
 *       <td>\b sigma_R</td>
 *       <td>width of radius distribution (LogNorm)</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_p__q___thin_ellipsoidal_shell
 *
 * \sa sasfit_thin_objects.h, sq_plugins_pprimelocalplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___thin_ellipsoidal_shell(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___thin_ellipsoidal_shell
 *
 * \sa sasfit_thin_objects.h, sq_plugins_pprimelocalplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___thin_ellipsoidal_shell_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___thin_ellipsoidal_shell
 *
 * \sa sasfit_thin_objects.h, sq_plugins_pprimelocalplanar
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___thin_ellipsoidal_shell_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_p__q___thin_ellipsoidal_shell ################ */

/* ################ start sq_p__q___thin_hollow_cylinder ################ */
/**
 * \defgroup sq_p__q___thin_hollow_cylinder P'(Q): Thin Hollow Cylinder (closed)
 * \ingroup sq_plugins_pprimelocalplanar
 *
 * \brief form factor P'(Q) of a infinitely thin cylindrical shell with closed ends
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>most probable cylinder radius</td>
 *      </tr><tr>
 *       <td>\b H</td>
 *       <td>most probable cylinder height</td>
 *      </tr><tr>
 *       <td>\b sigma_R</td>
 *       <td>width of radius distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b sigma_H</td>
 *       <td>width of cylinder height distribution (LogNorm)</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_p__q___thin_hollow_cylinder
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___thin_hollow_cylinder(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___thin_hollow_cylinder
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___thin_hollow_cylinder_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___thin_hollow_cylinder
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___thin_hollow_cylinder_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_p__q___thin_hollow_cylinder ################ */

/* ################ start sq_p__q___rod ################ */
/**
 * \defgroup sq_p__q___rod P'(Q): Rod
 * \ingroup sq_plugins_pprimelocalcylindrical
 *
 * \brief \<some brief description of P'(Q): Rod function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b L</td>
 *       <td>most probable length of rod</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width of length distribution (LogNorm)</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_p__q___rod
 *
 * \sa sasfit_thin_objects.h, sq_plugins, sq_plugins_pprimelocalcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___rod(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___rod
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___rod_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___rod
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___rod_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_p__q___rod ################ */

/* ################ start sq_p__q___worm_ps1_ ################ */
/**
 * \defgroup sq_p__q___worm_ps1_ P'(Q): Worm(PS1)
 * \ingroup sq_plugins_pprimelocalcylindrical
 *
 * \brief \<some brief description of P'(Q): Worm(PS1) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b lb</td>
 *       <td>Kuhn length</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>contour length</td>
 *      </tr><tr>
 *       <td>\b exvol</td>
 *       <td>excluded volume parameter \n exvol < 1 without excluded volume, exvol >= 1 with excluded volume</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_p__q___worm_ps1_
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps1_(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___worm_ps1_
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps1__f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___worm_ps1_
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps1__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_p__q___worm_ps1_ ################ */

/* ################ start sq_p__q___worm_ps2_ ################ */
/**
 * \defgroup sq_p__q___worm_ps2_ P'(Q): Worm(PS2)
 * \ingroup sq_plugins_pprimelocalcylindrical
 *
 * \brief \<some brief description of P'(Q): Worm(PS2) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b lb</td>
 *       <td>Kuhn length</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>contour lengrth</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_p__q___worm_ps2_
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps2_(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___worm_ps2_
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps2__f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___worm_ps2_
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps2__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_p__q___worm_ps2_ ################ */

/* ################ start sq_p__q___worm_ps3_ ################ */
/**
 * \defgroup sq_p__q___worm_ps3_ P'(Q): Worm(PS3)
 * \ingroup sq_plugins_pprimelocalcylindrical
 *
 * \brief \<some brief description of P'(Q): Worm(PS3) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b lb</td>
 *       <td>Kuhn length</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>contour length</td>
 *      </tr><tr>
 *       <td>\b exvol</td>
 *       <td>excluded volume parameter \n exvol < 1 without excluded volume, exvol >= 1 with excluded volume</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_p__q___worm_ps3_
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps3_(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___worm_ps3_
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps3__f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___worm_ps3_
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps3__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_p__q___worm_ps3_ ################ */

/* ################ start sq_p__q___worm_ps3_RPA ################ */
/**
 * \defgroup sq_p__q___worm_ps3_RPA P'(Q): Worm(PS3,RPA)
 * \ingroup sq_plugins_pprimelocalcylindrical
 *
 * \brief \<some brief description of P'(Q): Worm(PS3,RPA) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b lb</td>
 *       <td>Kuhn length</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>contour length</td>
 *      </tr><tr>
 *       <td>\b exvol</td>
 *       <td>excluded volume parameter \n exvol < 1 without excluded volume, exvol >= 1 with excluded volume</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>interaction parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b CLD</td>
 *       <td>a value larger than 0 applies an exponential contour length distribution otherwise it uses a monodisperse contour length</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_p__q___worm_ps3_RPA
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps3_RPA(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___worm_ps3_RPA
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps3_RPA_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___worm_ps3_RPA
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps3_RPA_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_p__q___worm_ps3_RPA ################ */

/* ################ start sq_p__q___worm_ps3_PRISM1 ################ */
/**
 * \defgroup sq_p__q___worm_ps3_PRISM1 P'(Q): Worm(PS3,PRISM1)
 * \ingroup sq_plugins_pprimelocalcylindrical
 *
 * \brief \<some brief description of P'(Q): Worm(PS3,PRISM1) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b lb</td>
 *       <td>Kuhn length</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>contour length</td>
 *      </tr><tr>
 *       <td>\b exvol</td>
 *       <td>excluded volume parameter \n exvol < 1 without excluded volume, exvol >= 1 with excluded volume</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>interaction parameter</td>
 *      </tr><tr>
 *       <td>\b Rc</td>
 *       <td>correlation hole</td>
 *      </tr><tr>
 *       <td>\b CLD</td>
 *       <td>a value larger than 0 applies an exponential contour length distribution otherwise it uses a monodisperse contour length</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_p__q___worm_ps3_PRISM1
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps3_PRISM1(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___worm_ps3_PRISM1
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps3_PRISM1_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___worm_ps3_PRISM1
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps3_PRISM1_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_p__q___worm_ps3_PRISM1 ################ */

/* ################ start sq_p__q___worm_ps3_PRISM2 ################ */
/**
 * \defgroup sq_p__q___worm_ps3_PRISM2 P'(Q): Worm(PS3,PRISM2)
 * \ingroup sq_plugins_pprimelocalcylindrical
 *
 * \brief \<some brief description of P'(Q): Worm(PS3,PRISM2) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b lb</td>
 *       <td>Kuhn length</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>contour length</td>
 *      </tr><tr>
 *       <td>\b exvol</td>
 *       <td>excluded volume parameter \n exvol < 1 without excluded volume, exvol >= 1 with excluded volume</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>interaction parameter</td>
 *      </tr><tr>
 *       <td>\b Lc</td>
 *       <td>rod length correlation hole</td>
 *      </tr><tr>
 *       <td>\b CLD</td>
 *       <td>a value larger than 0 applies an exponential contour length distribution otherwise it uses a monodisperse contour length</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_p__q___worm_ps3_PRISM2
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps3_PRISM2(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___worm_ps3_PRISM2
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps3_PRISM2_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___worm_ps3_PRISM2
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps3_PRISM2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_p__q___worm_ps3_PRISM2 ################ */

/* ################ start sq_p__q___worm_ps3_PRISM3 ################ */
/**
 * \defgroup sq_p__q___worm_ps3_PRISM3 P'(Q): Worm(PS3,PRISM3)
 * \ingroup sq_plugins_pprimelocalcylindrical
 *
 * \brief \<some brief description of P'(Q): Worm(PS3,PRISM3) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b lb</td>
 *       <td>Kuhn length</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>contour length</td>
 *      </tr><tr>
 *       <td>\b exvol</td>
 *       <td>excluded volume parameter \n exvol < 1 without excluded volume, exvol >= 1 with excluded volume</td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td>interaction parameter</td>
 *      </tr><tr>
 *       <td>\b Rc</td>
 *       <td>correlation hole Rc=2Log(sigma)+2</td>
 *      </tr><tr>
 *       <td>\b CLD</td>
 *       <td>a value larger than 0 applies an exponential contour length distribution otherwise it uses a monodisperse contour length</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_p__q___worm_ps3_PRISM3
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps3_PRISM3(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___worm_ps3_PRISM3
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps3_PRISM3_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___worm_ps3_PRISM3
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps3_PRISM3_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_p__q___worm_ps3_PRISM3 ################ */

/* ################ start sq_p__q___kholodenko_worm ################ */
/**
 * \defgroup sq_p__q___kholodenko_worm P'(Q): Kholodenko Worm
 * \ingroup sq_plugins_pprimelocalcylindrical
 *
 * \brief \<some brief description of P'(Q): Kholodenko Worm function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b lb</td>
 *       <td>Kuhn length</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>contour length</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_p__q___kholodenko_worm
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___kholodenko_worm(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___kholodenko_worm
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___kholodenko_worm_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___kholodenko_worm
 *
 * \sa sasfit_thin_objects.h, sq_plugins
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_p__q___kholodenko_worm_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_p__q___kholodenko_worm ################ */

/* ################ start sq_freely_joined_chain_of_rods ################ */
/**
 * \defgroup sq_freely_joined_chain_of_rods  P'(Q): freely joined chain of rods
 * \ingroup sq_plugins_pprimelocalcylindrical
 *
 * \brief \< some brief description of freely joined chain of rods function \>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b lb</td>
 *       <td>Kuhn length</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>contour length</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_freely_joined_chain_of_rods
 *
 * \sa sasfit_more_worms.h, sq_plugins_pprimelocalcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_freely_joined_chain_of_rods(scalar q, sasfit_param * p);

/**
 * \ingroup sq_freely_joined_chain_of_rods
 *
 * \sa sasfit_more_worms.h, sq_plugins_pprimelocalcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_freely_joined_chain_of_rods_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_freely_joined_chain_of_rods
 *
 * \sa sasfit_more_worms.h, sq_plugins_pprimelocalcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_freely_joined_chain_of_rods_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_freely_joined_chain_of_rods ################ */

/* ################ start sq_koyama_worms ################ */
/**
 * \defgroup sq_koyama_worms  P'(Q): Koyama worm
 * \ingroup sq_plugins_pprimelocalcylindrical
 *
 * \brief \<some brief description of Koyama worm function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b lb</td>
 *       <td>Kuhn length</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>contour length</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_koyama_worms
 *
 * \sa sasfit_more_worms.h, sq_plugins_pprimelocalcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_koyama_worms(scalar q, sasfit_param * p);

/**
 * \ingroup sq_koyama_worms
 *
 * \sa sasfit_more_worms.h, sq_plugins_pprimelocalcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_koyama_worms_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_koyama_worms
 *
 * \sa sasfit_more_worms.h, sq_plugins_pprimelocalcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_sq_koyama_worms_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_koyama_worms ################ */

/* ################ start ff_pcs_homogeneouscyl ################ */
/**
 * \defgroup ff_pcs_homogeneouscyl Pcs:homogeneousCyl
 * \ingroup ff_plugins_pcslocalcylindrical
 *
 * \brief cross section form factor of a homegeneous cylinder
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>most probable radius</td>
 *      </tr><tr>
 *       <td>\b sigma_R</td>
 *       <td>width of radius distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>scatterinmg length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_pcs_homogeneouscyl
 *
 * \sa sasfit_thin_objects.h, ff_plugins_pcslocalcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_pcs_homogeneouscyl(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_homogeneouscyl
 *
 * \sa sasfit_thin_objects.h, ff_plugins_pcslocalcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_pcs_homogeneouscyl_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_homogeneouscyl
 *
 * \sa sasfit_thin_objects.h, ff_plugins_pcslocalcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_pcs_homogeneouscyl_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_pcs_homogeneouscyl ################ */


/* ################ start ff_pcs_ellcylsh ################ */
/**
 * \defgroup ff_pcs_ellcylsh Pcs:ellCylSh
 * \ingroup ff_plugins_pcslocalcylindrical
 *
 * \brief \<some brief description of Pcs:ellCylSh function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R0</td>
 *       <td>most probable core radius</td>
 *      </tr><tr>
 *       <td>\b sigma_R0</td>
 *       <td>width of radius distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b epsilon</td>
 *       <td>stretching factor of elliptical cross-section</td>
 *      </tr><tr>
 *       <td>\b t0</td>
 *       <td>most probable shell thickness</td>
 *      </tr><tr>
 *       <td>\b sigma_t0</td>
 *       <td>width of thickness distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_pcs_ellcylsh
 *
 * \sa sasfit_thin_objects.h, ff_plugins_pcslocalcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_pcs_ellcylsh(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_ellcylsh
 *
 * \sa sasfit_thin_objects.h, ff_plugins_pcslocalcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_pcs_ellcylsh_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_ellcylsh
 *
 * \sa sasfit_thin_objects.h, ff_plugins_pcslocalcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_pcs_ellcylsh_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_pcs_ellcylsh ################ */

/* ################ start ff_rod_sd_ellcylsh ################ */
/**
 * \defgroup ff_rod_sd_ellcylsh Rod+SD+EllCylSh
 * \ingroup ff_plugins_localcylindrical
 *
 * \brief \<some brief description of Rod+SD+EllCylSh function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R0</td>
 *       <td>most probable core radius</td>
 *      </tr><tr>
 *       <td>\b sigma_R0</td>
 *       <td>width of core radius distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b epsilon</td>
 *       <td>stretching factor of elliptical cross-section</td>
 *      </tr><tr>
 *       <td>\b t0</td>
 *       <td>most probable shell thickness</td>
 *      </tr><tr>
 *       <td>\b sigma_t0</td>
 *       <td>width of thickness distribution</td>
 *      </tr><tr>
 *       <td>\b L0</td>
 *       <td>most probable rod length</td>
 *      </tr><tr>
 *       <td>\b sigma_L0</td>
 *       <td>width of lengths distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_rod_sd_ellcylsh
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_rod_sd_ellcylsh(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod_sd_ellcylsh
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_rod_sd_ellcylsh_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod_sd_ellcylsh
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_rod_sd_ellcylsh_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rod_sd_ellcylsh ################ */


/* ################ start ff_worm_ps1_sd_ellcylsh ################ */
/**
 * \defgroup ff_worm_ps1_sd_ellcylsh EllCylSh+SD+Worm(PS1)
 * \ingroup ff_plugins_localcylindrical
 *
 * \brief \<some brief description of EllCylSh+SD+Worm(PS1) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b sigma_R</td>
 *       <td>width of radius distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b epsilon</td>
 *       <td>stretching factor of elliptical cross-section</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b lb</td>
 *       <td>Kuhn length</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>contour length of worm</td>
 *      </tr><tr>
 *       <td>\b exvol</td>
 *       <td>excluded volume parameter (0: without excluded volume, 1: with excluded volume)</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_worm_ps1_sd_ellcylsh
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_worm_ps1_sd_ellcylsh(scalar q, sasfit_param * p);

/**
 * \ingroup ff_worm_ps1_sd_ellcylsh
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_worm_ps1_sd_ellcylsh_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_worm_ps1_sd_ellcylsh
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_worm_ps1_sd_ellcylsh_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_worm_ps1_sd_ellcylsh ################ */

/* ################ start ff_worm_ps2_sd_ellcylsh ################ */
/**
 * \defgroup ff_worm_ps2_sd_ellcylsh EllCylSh+SD+Worm(PS2)
 * \ingroup ff_plugins_localcylindrical
 *
 * \brief \<some brief description of EllCylSh+SD+Worm(PS2) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b sigma_R</td>
 *       <td>width of radius distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b epsilon</td>
 *       <td>stretching factor of elliptical cross-section</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b lb</td>
 *       <td>Kuhn length</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>contour length of worm</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_worm_ps2_sd_ellcylsh
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_worm_ps2_sd_ellcylsh(scalar q, sasfit_param * p);

/**
 * \ingroup ff_worm_ps2_sd_ellcylsh
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_worm_ps2_sd_ellcylsh_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_worm_ps2_sd_ellcylsh
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_worm_ps2_sd_ellcylsh_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_worm_ps2_sd_ellcylsh ################ */


/* ################ start ff_worm_ps3_sd_ellcylsh ################ */
/**
 * \defgroup ff_worm_ps3_sd_ellcylsh EllCylSh+SD+Worm(PS3)
 * \ingroup ff_plugins_localcylindrical
 *
 * \brief \<some brief description of EllCylSh+SD+Worm(PS2) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b sigma_R</td>
 *       <td>width of radius distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b epsilon</td>
 *       <td>stretching factor of elliptical cross-section</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b lb</td>
 *       <td>Kuhn length</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>contour length of worm</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_worm_ps3_sd_ellcylsh
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_worm_ps3_sd_ellcylsh(scalar q, sasfit_param * p);

/**
 * \ingroup ff_worm_ps3_sd_ellcylsh
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_worm_ps3_sd_ellcylsh_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_worm_ps3_sd_ellcylsh
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_worm_ps3_sd_ellcylsh_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_worm_ps3_sd_ellcylsh ################ */



/* ################ start ff_worm_kholodenko_sd_ellcylsh ################ */
/**
 * \defgroup ff_worm_kholodenko_sd_ellcylsh EllCylSh+SD+Worm(Kholodenko)
 * \ingroup ff_plugins_localcylindrical
 *
 * \brief \<some brief description of EllCylSh+SD+Worm(Kholodenko) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b sigma_R</td>
 *       <td>width of radius distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b epsilon</td>
 *       <td>stretching factor of elliptical cross-section</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b lb</td>
 *       <td>Kuhn length</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>contour length of worm</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_worm_kholodenko_sd_ellcylsh
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_worm_kholodenko_sd_ellcylsh(scalar q, sasfit_param * p);

/**
 * \ingroup ff_worm_kholodenko_sd_ellcylsh
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_worm_kholodenko_sd_ellcylsh_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_worm_kholodenko_sd_ellcylsh
 *
 * \sa sasfit_thin_objects.h, ff_plugins_localcylindrical
 */
sasfit_thin_objects_DLLEXP scalar sasfit_ff_worm_kholodenko_sd_ellcylsh_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_worm_kholodenko_sd_ellcylsh ################ */

#endif // this file

