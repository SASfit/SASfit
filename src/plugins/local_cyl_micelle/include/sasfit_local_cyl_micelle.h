/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_LOCAL_CYL_MICELLE_H
#define SASFIT_PLUGIN_LOCAL_CYL_MICELLE_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_local_cyl_micelle.h
 * Public available functions and descriptions of the local_cyl_micelle plugin.
 */

/**
 * \def sasfit_local_cyl_micelle_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_local_cyl_micelle_EXPORTS)
	#ifdef sasfit_local_cyl_micelle_DLLEXP
	#undef sasfit_local_cyl_micelle_DLLEXP
	#endif
	#define sasfit_local_cyl_micelle_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_local_cyl_micelle_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_local_cyl_micelle_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_cyl_chains_rw__nagg ################ */
/** 
 * \defgroup ff_cyl_chains_rw__nagg cyl_chains(RW)_nagg
 * \ingroup ff_local_cyl_micelles
 *
 * \brief \<some brief description of cyl_chains(RW)_nagg function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Nagg</td>
 *       <td>aggregation number of copolymer per micelle</td>
 *      </tr><tr>
 *       <td>\b V_core</td>
 *       <td>molecular volume of the copolymer block unit inside core</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>molecular volume of the copolymer block unit in corona</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of cylindrical core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of Gaussian chains in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b xsolv_core</td>
 *       <td>amounnt of solvent in core</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>gyration radius of polymer chains in corona</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>d: value should be around 1; non-penetration of chains into the core is mimicked by d~1 for R_core>>Rg</td>
 *      </tr><tr>
 *       <td>\b H</td>
 *       <td>height of cylinder</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_cyl_chains_rw__nagg
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_cyl_chains_rw__nagg(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cyl_chains_rw__nagg
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_cyl_chains_rw__nagg_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cyl_chains_rw__nagg
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_cyl_chains_rw__nagg_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_cyl_chains_rw__nagg ################ */

/* ################ start ff_cyl_chains_rw__rc ################ */
/** 
 * \defgroup ff_cyl_chains_rw__rc cyl_chains(RW)_Rc
 * \ingroup ff_local_cyl_micelles
 *
 * \brief \<some brief description of cyl_chains(RW)_Rc function\>
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
 *       <td>molecular volume of the copolymer block unit inside core</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>molecular volume of the copolymer block unit in corona</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of cylindrical core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of Gaussian chains in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b xsolv_core</td>
 *       <td>amounnt of solvent in core</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_cyl_chains_rw__rc
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_cyl_chains_rw__rc(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cyl_chains_rw__rc
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_cyl_chains_rw__rc_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cyl_chains_rw__rc
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_cyl_chains_rw__rc_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_cyl_chains_rw__rc ################ */

/* ################ start ff_cyl_chains_rw_ ################ */
/** 
 * \defgroup ff_cyl_chains_rw_ cyl_chains(RW)
 * \ingroup ff_local_cyl_micelles
 *
 * \brief \<some brief description of cyl_chains(RW) function\>
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
 *       <td>aggregation number density, chains per surface area</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>molecular volume of the copolymer block unit in corona</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of cylindrical core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of Gaussian chains in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b xsolv_core</td>
 *       <td>amounnt of solvent in core</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_cyl_chains_rw_
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_cyl_chains_rw_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cyl_chains_rw_
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_cyl_chains_rw__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cyl_chains_rw_
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_cyl_chains_rw__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_cyl_chains_rw_ ################ */

/* ################ start ff_worm_chains_rw__nagg ################ */
/** 
 * \defgroup ff_worm_chains_rw__nagg worm_chains(RW)_nagg
 * \ingroup ff_local_cyl_micelles
 *
 * \brief \<some brief description of worm_chains(RW)_nagg function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Nagg</td>
 *       <td>aggregation number of copolymer per micelle</td>
 *      </tr><tr>
 *       <td>\b V_core</td>
 *       <td>molecular volume of the copolymer block unit inside core</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>molecular volume of the copolymer block unit in corona</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of cylindrical core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of Gaussian chains in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b xsolv_core</td>
 *       <td>amounnt of solvent in core</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_worm_chains_rw__nagg
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_worm_chains_rw__nagg(scalar q, sasfit_param * p);

/**
 * \ingroup ff_worm_chains_rw__nagg
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_worm_chains_rw__nagg_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_worm_chains_rw__nagg
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_worm_chains_rw__nagg_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_worm_chains_rw__nagg ################ */

/* ################ start ff_worm_chains_rw__rc ################ */
/** 
 * \defgroup ff_worm_chains_rw__rc worm_chains(RW)_Rc
 * \ingroup ff_local_cyl_micelles
 *
 * \brief \<some brief description of worm_chains(RW)_Rc function\>
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
 *       <td>molecular volume of the copolymer block unit inside core</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>molecular volume of the copolymer block unit in corona</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of cylindrical core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of Gaussian chains in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b xsolv_core</td>
 *       <td>amounnt of solvent in core</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_worm_chains_rw__rc
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_worm_chains_rw__rc(scalar q, sasfit_param * p);

/**
 * \ingroup ff_worm_chains_rw__rc
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_worm_chains_rw__rc_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_worm_chains_rw__rc
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_worm_chains_rw__rc_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_worm_chains_rw__rc ################ */

/* ################ start ff_worm_chains_rw_ ################ */
/** 
 * \defgroup ff_worm_chains_rw_ worm_chains(RW)
 * \ingroup ff_local_cyl_micelles
 *
 * \brief \<some brief description of worm_chains(RW) function\>
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
 *       <td>aggregation number density, chains per surface area</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>molecular volume of the copolymer block unit in corona</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of cylindrical core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of Gaussian chains in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b xsolv_core</td>
 *       <td>amounnt of solvent in core</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_worm_chains_rw_
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_worm_chains_rw_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_worm_chains_rw_
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_worm_chains_rw__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_worm_chains_rw_
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_worm_chains_rw__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_worm_chains_rw_ ################ */

/* ################ start ff_rod_chains_rw__nagg ################ */
/** 
 * \defgroup ff_rod_chains_rw__nagg rod_chains(RW)_nagg
 * \ingroup ff_local_cyl_micelles
 *
 * \brief \<some brief description of rod_chains(RW)_nagg function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b nagg</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b V_core</td>
 *       <td>molecular volume of the copolymer block unit inside core</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>molecular volume of the copolymer block unit in corona</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of cylindrical core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of Gaussian chains in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b xsolv_core</td>
 *       <td>amounnt of solvent in core</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_rod_chains_rw__nagg
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_chains_rw__nagg(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod_chains_rw__nagg
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_chains_rw__nagg_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod_chains_rw__nagg
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_chains_rw__nagg_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rod_chains_rw__nagg ################ */

/* ################ start ff_rod_chains_rw__rc ################ */
/** 
 * \defgroup ff_rod_chains_rw__rc rod_chains(RW)_Rc
 * \ingroup ff_local_cyl_micelles
 *
 * \brief \<some brief description of rod_chains(RW)_Rc function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rc</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b V_core</td>
 *       <td>molecular volume of the copolymer block unit inside core</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>molecular volume of the copolymer block unit in corona</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of cylindrical core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of Gaussian chains in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b xsolv_core</td>
 *       <td>amounnt of solvent in core</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_rod_chains_rw__rc
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_chains_rw__rc(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod_chains_rw__rc
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_chains_rw__rc_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod_chains_rw__rc
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_chains_rw__rc_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rod_chains_rw__rc ################ */

/* ################ start ff_rod_chains_rw_ ################ */
/** 
 * \defgroup ff_rod_chains_rw_ rod_chains(RW)
 * \ingroup ff_local_cyl_micelles
 *
 * \brief \<some brief description of rod_chains(RW) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rc</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b n_agg</td>
 *       <td>aggregation number density, chains per surface area</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>molecular volume of the copolymer block unit in corona</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of cylindrical core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of Gaussian chains in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b xsolv_core</td>
 *       <td>amounnt of solvent in core</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_rod_chains_rw_
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_chains_rw_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod_chains_rw_
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_chains_rw__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod_chains_rw_
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_chains_rw__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rod_chains_rw_ ################ */

/* ################ start ff_rod__r__a_nagg ################ */
/** 
 * \defgroup ff_rod__r__a_nagg rod_+R^-a_nagg
 * \ingroup ff_local_cyl_micelles
 *
 * \brief \<some brief description of rod_+R^-a_nagg function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b nagg</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_rod__r__a_nagg
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod__r__a_nagg(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod__r__a_nagg
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod__r__a_nagg_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod__r__a_nagg
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod__r__a_nagg_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rod__r__a_nagg ################ */

/* ################ start ff_rod__r__a_rc ################ */
/** 
 * \defgroup ff_rod__r__a_rc rod_+R^-a_Rc
 * \ingroup ff_local_cyl_micelles
 *
 * \brief \<some brief description of rod_+R^-a_Rc function\>
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
 * \ingroup ff_rod__r__a_rc
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod__r__a_rc(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod__r__a_rc
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod__r__a_rc_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod__r__a_rc
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod__r__a_rc_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rod__r__a_rc ################ */

/* ################ start ff_rod__r__a ################ */
/** 
 * \defgroup ff_rod__r__a rod_+R^-a
 * \ingroup ff_local_cyl_micelles
 *
 * \brief \<some brief description of rod_+R^-a function\>
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
 * \ingroup ff_rod__r__a
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod__r__a(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod__r__a
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod__r__a_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod__r__a
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod__r__a_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rod__r__a ################ */

/* ################ start ff_rod_gauss_nagg ################ */
/** 
 * \defgroup ff_rod_gauss_nagg rod_Gauss_nagg
 * \ingroup ff_local_cyl_micelles
 *
 * \brief \<some brief description of rod_Gauss_nagg function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b nagg</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_rod_gauss_nagg
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_gauss_nagg(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod_gauss_nagg
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_gauss_nagg_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod_gauss_nagg
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_gauss_nagg_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rod_gauss_nagg ################ */

/* ################ start ff_rod_gauss_rc ################ */
/** 
 * \defgroup ff_rod_gauss_rc rod_Gauss_Rc
 * \ingroup ff_local_cyl_micelles
 *
 * \brief \<some brief description of rod_Gauss_Rc function\>
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
 * \ingroup ff_rod_gauss_rc
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_gauss_rc(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod_gauss_rc
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_gauss_rc_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod_gauss_rc
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_gauss_rc_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rod_gauss_rc ################ */

/* ################ start ff_rod_gauss ################ */
/** 
 * \defgroup ff_rod_gauss rod_Gauss
 * \ingroup ff_local_cyl_micelles
 *
 * \brief \<some brief description of rod_Gauss function\>
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
 * \ingroup ff_rod_gauss
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_gauss(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod_gauss
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_gauss_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod_gauss
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_gauss_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rod_gauss ################ */

/* ################ start ff_rod_exp_nagg ################ */
/** 
 * \defgroup ff_rod_exp_nagg rod_exp_nagg
 * \ingroup ff_local_cyl_micelles
 *
 * \brief \<some brief description of rod_exp_nagg function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b nagg</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_rod_exp_nagg
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_exp_nagg(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod_exp_nagg
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_exp_nagg_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod_exp_nagg
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_exp_nagg_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rod_exp_nagg ################ */

/* ################ start ff_rod_exp_rc ################ */
/** 
 * \defgroup ff_rod_exp_rc rod_exp_Rc
 * \ingroup ff_local_cyl_micelles
 *
 * \brief \<some brief description of rod_exp_Rc function\>
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
 * \ingroup ff_rod_exp_rc
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_exp_rc(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod_exp_rc
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_exp_rc_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod_exp_rc
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_exp_rc_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rod_exp_rc ################ */

/* ################ start ff_rod_exp ################ */
/** 
 * \defgroup ff_rod_exp rod_exp
 * \ingroup ff_local_cyl_micelles
 *
 * \brief \<some brief description of rod_exp function\>
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
 * \ingroup ff_rod_exp
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_exp(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod_exp
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_exp_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod_exp
 *
 * \sa sasfit_local_cyl_micelle.h, ff_local_cyl_micelles
 */
sasfit_local_cyl_micelle_DLLEXP scalar sasfit_ff_rod_exp_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rod_exp ################ */


#endif // this file

