/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_ANISOTROPIC_OBJECTS_H
#define SASFIT_PLUGIN_ANISOTROPIC_OBJECTS_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_anisotropic_objects.h
 * Public available functions and descriptions of the anisotropic_objects plugin.
 */

/**
 * \def sasfit_anisotropic_objects_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_anisotropic_objects_EXPORTS)
	#ifdef sasfit_anisotropic_objects_DLLEXP
	#undef sasfit_anisotropic_objects_DLLEXP
	#endif
	#define sasfit_anisotropic_objects_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_anisotropic_objects_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_anisotropic_objects_DLLEXP SASFIT_LIB_IMPORT
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
 *       <td>width of thickess distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td>most probable  disc diameter</td>
 *      </tr><tr>
 *       <td>\b sigma_D</td>
 *       <td>width of diameter distribution (LogNorm) </td>
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
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_disc_sd_homoxs(scalar q, sasfit_param * p);

/**
 * \ingroup ff_disc_sd_homoxs
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_disc_sd_homoxs_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_disc_sd_homoxs
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_disc_sd_homoxs_v(scalar q, sasfit_param * p, int dist);
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
 *       <td>eccentricity</td>
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
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_ellsh_sd_homoxs(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellsh_sd_homoxs
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_ellsh_sd_homoxs_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellsh_sd_homoxs
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_ellsh_sd_homoxs_v(scalar q, sasfit_param * p, int dist);
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
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_cylsh_sd_homoxs(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylsh_sd_homoxs
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_cylsh_sd_homoxs_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylsh_sd_homoxs
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_cylsh_sd_homoxs_v(scalar q, sasfit_param * p, int dist);
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
 *       <td>\b D</td>
 *       <td>most probable  disc diameter</td>
 *      </tr><tr>
 *       <td>\b sigma_D</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_out</td>
 *       <td>sigma_out</td>
 *      </tr><tr>
 *       <td>\b b_out</td>
 *       <td>b_out</td>
 *      </tr><tr>
 *       <td>\b sigma_core</td>
 *       <td>sigma_core</td>
 *      </tr><tr>
 *       <td>\b b_core</td>
 *       <td>b_core</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_disc_sd_bilayergauss
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_disc_sd_bilayergauss(scalar q, sasfit_param * p);

/**
 * \ingroup ff_disc_sd_bilayergauss
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_disc_sd_bilayergauss_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_disc_sd_bilayergauss
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_disc_sd_bilayergauss_v(scalar q, sasfit_param * p, int dist);
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
 *       <td>t</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ellsh_sd_bilayergauss
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_ellsh_sd_bilayergauss(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellsh_sd_bilayergauss
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_ellsh_sd_bilayergauss_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellsh_sd_bilayergauss
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_ellsh_sd_bilayergauss_v(scalar q, sasfit_param * p, int dist);
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
 *       <td>t</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_cylsh_sd_bilayergauss
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_cylsh_sd_bilayergauss(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylsh_sd_bilayergauss
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_cylsh_sd_bilayergauss_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylsh_sd_bilayergauss
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_cylsh_sd_bilayergauss_v(scalar q, sasfit_param * p, int dist);
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
 * \sa sasfit_anisotropic_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_pcs_homogeneousplate(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_homogeneousplate
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_pcs_homogeneousplate_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_homogeneousplate
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_pcs_homogeneousplate_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_pcs_homogeneousplate ################ */


/* ################ start ff_pcs_plate_chains_rw_ ################ */
/** 
 * \defgroup ff_pcs_plate_chains_rw_ Pcs:Plate+Chains(RW)
 * \ingroup ff_plugins_pcslocalplanar
 *
 * \brief \<some brief description of Pcs:Plate+Chains(RW) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b t</td>
 *       <td>t</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_pcs_plate_chains_rw_
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_pcs_plate_chains_rw_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_plate_chains_rw_
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_pcs_plate_chains_rw__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_plate_chains_rw_
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_pcs_plate_chains_rw__v(scalar q, sasfit_param * p, int dist);
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
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
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
 * \sa sasfit_anisotropic_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_pcs_layeredcentrosymmetricxs(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_layeredcentrosymmetricxs
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_pcs_layeredcentrosymmetricxs_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_layeredcentrosymmetricxs
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_pcs_layeredcentrosymmetricxs_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_pcs_layeredcentrosymmetricxs ################ */

/* ################ start ff_pcs_blayergaus ################ */
/** 
 * \defgroup ff_pcs_blayergaus Pcs:BlayerGaus
 * \ingroup ff_plugins_pcslocalplanar
 *
 * \brief \<some brief description of Pcs:BlayerGaus function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b t</td>
 *       <td>t</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_pcs_blayergaus
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_pcs_blayergaus(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_blayergaus
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_pcs_blayergaus_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_blayergaus
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_pcs_blayergaus_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_pcs_blayergaus ################ */

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
 * \sa sasfit_anisotropic_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_pcs_twoinfinitelythinlayers(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_twoinfinitelythinlayers
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_pcs_twoinfinitelythinlayers_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_twoinfinitelythinlayers
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_pcs_twoinfinitelythinlayers_v(scalar q, sasfit_param * p, int dist);
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
 * \sa sasfit_anisotropic_objects.h, sq_plugins
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___thin_disc(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___thin_disc
 *
 * \sa sasfit_anisotropic_objects.h, sq_plugins
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___thin_disc_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___thin_disc
 *
 * \sa sasfit_anisotropic_objects.h, sq_plugins
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___thin_disc_v(scalar q, sasfit_param * p, int dist);
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
 * \sa sasfit_anisotropic_objects.h, sq_plugins
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___thin_spherical_shell(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___thin_spherical_shell
 *
 * \sa sasfit_anisotropic_objects.h, sq_plugins
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___thin_spherical_shell_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___thin_spherical_shell
 *
 * \sa sasfit_anisotropic_objects.h, sq_plugins
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___thin_spherical_shell_v(scalar q, sasfit_param * p, int dist);
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
 *       <td>eccentricity</td>
 *      </tr><tr>
 *       <td>\b sigma_R</td>
 *       <td>width of radius distribution (LogNorm)</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_p__q___thin_ellipsoidal_shell
 *
 * \sa sasfit_anisotropic_objects.h, sq_plugins_pprimelocalplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___thin_ellipsoidal_shell(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___thin_ellipsoidal_shell
 *
 * \sa sasfit_anisotropic_objects.h, sq_plugins_pprimelocalplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___thin_ellipsoidal_shell_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___thin_ellipsoidal_shell
 *
 * \sa sasfit_anisotropic_objects.h, sq_plugins_pprimelocalplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___thin_ellipsoidal_shell_v(scalar q, sasfit_param * p, int dist);
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
 * \sa sasfit_anisotropic_objects.h, sq_plugins
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___thin_hollow_cylinder(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___thin_hollow_cylinder
 *
 * \sa sasfit_anisotropic_objects.h, sq_plugins
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___thin_hollow_cylinder_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___thin_hollow_cylinder
 *
 * \sa sasfit_anisotropic_objects.h, sq_plugins
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___thin_hollow_cylinder_v(scalar q, sasfit_param * p, int dist);
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
 * \sa sasfit_anisotropic_objects.h, sq_plugins, sq_plugins_pprimelocalcylindrical
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___rod(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___rod
 *
 * \sa sasfit_anisotropic_objects.h, sq_plugins
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___rod_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___rod
 *
 * \sa sasfit_anisotropic_objects.h, sq_plugins
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___rod_v(scalar q, sasfit_param * p, int dist);
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
 * \sa sasfit_anisotropic_objects.h, sq_plugins
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps1_(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___worm_ps1_
 *
 * \sa sasfit_anisotropic_objects.h, sq_plugins
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps1__f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___worm_ps1_
 *
 * \sa sasfit_anisotropic_objects.h, sq_plugins
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps1__v(scalar q, sasfit_param * p, int dist);
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
 * \sa sasfit_anisotropic_objects.h, sq_plugins
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps2_(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___worm_ps2_
 *
 * \sa sasfit_anisotropic_objects.h, sq_plugins
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps2__f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___worm_ps2_
 *
 * \sa sasfit_anisotropic_objects.h, sq_plugins
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps2__v(scalar q, sasfit_param * p, int dist);
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
 * \sa sasfit_anisotropic_objects.h, sq_plugins
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps3_(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___worm_ps3_
 *
 * \sa sasfit_anisotropic_objects.h, sq_plugins
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps3__f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___worm_ps3_
 *
 * \sa sasfit_anisotropic_objects.h, sq_plugins
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___worm_ps3__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_p__q___worm_ps3_ ################ */

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
 * \sa sasfit_anisotropic_objects.h, sq_plugins
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___kholodenko_worm(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___kholodenko_worm
 *
 * \sa sasfit_anisotropic_objects.h, sq_plugins
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___kholodenko_worm_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_p__q___kholodenko_worm
 *
 * \sa sasfit_anisotropic_objects.h, sq_plugins
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_sq_p__q___kholodenko_worm_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_p__q___kholodenko_worm ################ */

/* ################ start ff_pcs_homogeneouscyl ################ */
/** 
 * \defgroup ff_pcs_homogeneouscyl Pcs:homogeneousCyl
 * \ingroup ff_plugins_pcslocalplanar
 *
 * \brief \<some brief description of Pcs:homogeneousCyl function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b r</td>
 *       <td>most probable radius</td>
 *      </tr><tr>
 *       <td>\b sigma_r</td>
 *       <td>width of radius distribution (LogNorm)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_pcs_homogeneouscyl
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_pcs_homogeneouscyl(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_homogeneouscyl
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_pcs_homogeneouscyl_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_homogeneouscyl
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_pcslocalplanar
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_pcs_homogeneouscyl_v(scalar q, sasfit_param * p, int dist);
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
 *       <td>\b R</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b sigma_R</td>
 *       <td>width of radius distribution (LogNorm)</td>
 *      </tr><tr>
 *       <td>\b epsilon</td>
 *       <td>eccentricity of elliptical cross-section</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
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
 * \sa sasfit_anisotropic_objects.h, ff_plugins_pcslocalcylindrical
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_pcs_ellcylsh(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_ellcylsh
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_pcslocalcylindrical
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_pcs_ellcylsh_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pcs_ellcylsh
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_pcslocalcylindrical
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_pcs_ellcylsh_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_pcs_ellcylsh ################ */

/* ################ start ff_ellcylsh_sd_rod ################ */
/** 
 * \defgroup ff_ellcylsh_sd_rod EllCylSh+SD+Rod
 * \ingroup ff_plugins_localcylindrical
 *
 * \brief \<some brief description of EllCylSh+SD+Rod function\>
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
 *       <td>eccentricity of elliptical cross-section</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>length of Rod</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_L</td>
 *       <td>width of length distribution (LogNorm)</td>
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
 * \ingroup ff_ellcylsh_sd_rod
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localcylindrical
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_ellcylsh_sd_rod(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellcylsh_sd_rod
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localcylindrical
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_ellcylsh_sd_rod_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellcylsh_sd_rod
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localcylindrical
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_ellcylsh_sd_rod_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ellcylsh_sd_rod ################ */


/* ################ start ff_ellcylsh_sd_worm_ps1 ################ */
/** 
 * \defgroup ff_ellcylsh_sd_worm_ps1 EllCylSh+SD+Worm(PS1)
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
 *       <td>eccentricity of elliptical cross-section</td>
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
 * \ingroup ff_ellcylsh_sd_worm_ps1
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localcylindrical
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_ellcylsh_sd_worm_ps1(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellcylsh_sd_worm_ps1
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localcylindrical
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_ellcylsh_sd_worm_ps1_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellcylsh_sd_worm_ps1
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localcylindrical
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_ellcylsh_sd_worm_ps1_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ellcylsh_sd_worm_ps1 ################ */

/* ################ start ff_ellcylsh_sd_worm_ps2 ################ */
/** 
 * \defgroup ff_ellcylsh_sd_worm_ps2 EllCylSh+SD+Worm(PS2)
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
 *       <td>eccentricity of elliptical cross-section</td>
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
 * \ingroup ff_ellcylsh_sd_worm_ps2
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localcylindrical
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_ellcylsh_sd_worm_ps2(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellcylsh_sd_worm_ps2
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localcylindrical
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_ellcylsh_sd_worm_ps2_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellcylsh_sd_worm_ps2
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localcylindrical
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_ellcylsh_sd_worm_ps2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ellcylsh_sd_worm_ps2 ################ */


/* ################ start ff_ellcylsh_sd_worm_ps3 ################ */
/** 
 * \defgroup ff_ellcylsh_sd_worm_ps3 EllCylSh+SD+Worm(PS3)
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
 *       <td>eccentricity of elliptical cross-section</td>
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
 * \ingroup ff_ellcylsh_sd_worm_ps3
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localcylindrical
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_ellcylsh_sd_worm_ps3(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellcylsh_sd_worm_ps3
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localcylindrical
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_ellcylsh_sd_worm_ps3_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellcylsh_sd_worm_ps3
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localcylindrical
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_ellcylsh_sd_worm_ps3_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ellcylsh_sd_worm_ps3 ################ */



/* ################ start ff_ellcylsh_sd_worm_kholodenko ################ */
/** 
 * \defgroup ff_ellcylsh_sd_worm_kholodenko EllCylSh+SD+Worm(Kholodenko)
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
 *       <td>eccentricity of elliptical cross-section</td>
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
 * \ingroup ff_ellcylsh_sd_worm_kholodenko
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localcylindrical
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_ellcylsh_sd_worm_kholodenko(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellcylsh_sd_worm_kholodenko
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localcylindrical
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_ellcylsh_sd_worm_kholodenko_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellcylsh_sd_worm_kholodenko
 *
 * \sa sasfit_anisotropic_objects.h, ff_plugins_localcylindrical
 */
sasfit_anisotropic_objects_DLLEXP scalar sasfit_ff_ellcylsh_sd_worm_kholodenko_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ellcylsh_sd_worm_kholodenko ################ */

#endif // this file

