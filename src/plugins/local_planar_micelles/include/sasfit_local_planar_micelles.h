/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_LOCAL_PLANAR_MICELLES_H
#define SASFIT_PLUGIN_LOCAL_PLANAR_MICELLES_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_local_planar_micelles.h
 * Public available functions and descriptions of the local_planar_micelles plugin.
 */

/**
 * \def sasfit_local_planar_micelles_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_local_planar_micelles_EXPORTS)
	#ifdef sasfit_local_planar_micelles_DLLEXP
	#undef sasfit_local_planar_micelles_DLLEXP
	#endif
	#define sasfit_local_planar_micelles_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_local_planar_micelles_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_local_planar_micelles_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_disc_chain_rw__nagg ################ */
/**
 * \defgroup ff_disc_chain_rw__nagg DISC+Chain(RW)_nagg
 * \ingroup ff_local_planar_micelles
 *
 * \brief Disc with Gaussian chains attached, the micelle consists of block copolymers.\nThe core thickness L_core is calculated from nagg and V_core.
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b nagg</td>
 *       <td>nagg: specific aggregation number\nnumber of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b V_core</td>
 *       <td>V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>eta_core: scattering length density of spherical core\n</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>eta_brush: scattering length density of a Gaussian chain in corona\n</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>eta_sol: scattering length density of solvent\n</td>
 *      </tr><tr>
 *       <td>\b xsolv_core</td>
 *       <td>xsolv_core: amount of solvent in core\n</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>Rg: gyration radius of polymer chains in the corona\n</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>d: value should be around 1\nnon-penetration of the chains into the core is mimicked by d~1 for L_core>>Rg</td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td>D: disc diameter\n</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy\n</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_disc_chain_rw__nagg
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_disc_chain_rw__nagg(scalar q, sasfit_param * p);

/**
 * \ingroup ff_disc_chain_rw__nagg
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_disc_chain_rw__nagg_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_disc_chain_rw__nagg
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_disc_chain_rw__nagg_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_disc_chain_rw__nagg ################ */

/* ################ start ff_disc_chain_rw__lc ################ */
/**
 * \defgroup ff_disc_chain_rw__lc DISC+Chain(RW)_Lc
 * \ingroup ff_local_planar_micelles
 *
 * \brief Disc with Gaussian chains attached, the micelle consists of block copolymers.\nThe aggregation number Nagg is calculated from L_core and V_core.
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b L_core</td>
 *       <td>L_core: thickness of the disc core</td>
 *      </tr><tr>
 *       <td>\b V_core</td>
 *       <td>V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>eta_core: scattering length density of spherical core\n</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>eta_brush: scattering length density of a Gaussian chain in corona\n</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>eta_sol: scattering length density of solvent\n</td>
 *      </tr><tr>
 *       <td>\b xsolv_core</td>
 *       <td>xsolv_core: amount of solvent in core\n</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>Rg: gyration radius of polymer chains in the corona\n</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>d: value should be around 1\nnon-penetration of the chains into the core is mimicked by d~1 for L_core>>Rg</td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td>D: disc diameter\n</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy\n</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_disc_chain_rw__lc
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_disc_chain_rw__lc(scalar q, sasfit_param * p);

/**
 * \ingroup ff_disc_chain_rw__lc
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_disc_chain_rw__lc_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_disc_chain_rw__lc
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_disc_chain_rw__lc_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_disc_chain_rw__lc ################ */

/* ################ start ff_disc_chain_rw_ ################ */
/**
 * \defgroup ff_disc_chain_rw_ DISC+Chain(RW)
 * \ingroup ff_local_planar_micelles
 *
 * \brief Disc with Gaussian chains attached\ndisc with Gaussian chains attached\nIn contrast to *_Lc and *_nagg this one does not need to consist of copolymers.
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b L_core</td>
 *       <td>L_core: thickness of the disc core\n</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>nagg: specific aggregation number\nnumber of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>eta_core: scattering length density of spherical core\n</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>eta_brush: scattering length density of a Gaussian chain in corona\n</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>eta_sol: scattering length density of solvent\n</td>
 *      </tr><tr>
 *       <td>\b xsolv_core</td>
 *       <td>xsolv_core: amount of solvent in core\n</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>Rg: gyration radius of polymer chains in the corona\n</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>d: value should be around 1\nnon-penetration of the chains into the core is mimicked by d~1 for L_core>>Rg</td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td>D: disc diameter\n</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy\n</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_disc_chain_rw_
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_disc_chain_rw_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_disc_chain_rw_
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_disc_chain_rw__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_disc_chain_rw_
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_disc_chain_rw__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_disc_chain_rw_ ################ */

/* ################ start ff_sphulv_chain_rw__nagg ################ */
/**
 * \defgroup ff_sphulv_chain_rw__nagg SphULV+Chain(RW)_nagg
 * \ingroup ff_local_planar_micelles
 *
 * \brief \<some brief description of SphULV+Chain(RW)_nagg function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b nagg</td>
 *       <td>nagg: specific aggregation number\nnumber of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b V_core</td>
 *       <td>V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b V_brush</td>
 *       <td>V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>eta_core: scattering length density of spherical core\n</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>eta_brush: scattering length density of a Gaussian chain in corona\n</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>eta_sol: scattering length density of solvent\n</td>
 *      </tr><tr>
 *       <td>\b xsolv_core</td>
 *       <td>xsolv_core: amount of solvent in core\n</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>Rg: gyration radius of polymer chains in the corona\n</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>d: value should be around 1\nnon-penetration of the chains into the core is mimicked by d~1 for L_core>>Rg</td>
 *      </tr><tr>
 *       <td>\b R</td>
 *       <td>R: radius of spherical unilamellar vesicle\n</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy\n</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_sphulv_chain_rw__nagg
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_sphulv_chain_rw__nagg(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphulv_chain_rw__nagg
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_sphulv_chain_rw__nagg_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphulv_chain_rw__nagg
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_sphulv_chain_rw__nagg_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_sphulv_chain_rw__nagg ################ */

/* ################ start ff_sphulv_chain_rw__tc ################ */
/**
 * \defgroup ff_sphulv_chain_rw__tc SphULV+Chain(RW)_tc
 * \ingroup ff_local_planar_micelles
 *
 * \brief \<some brief description of SphULV+Chain(RW)_tc function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b n</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_sphulv_chain_rw__tc
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_sphulv_chain_rw__tc(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphulv_chain_rw__tc
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_sphulv_chain_rw__tc_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphulv_chain_rw__tc
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_sphulv_chain_rw__tc_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_sphulv_chain_rw__tc ################ */

/* ################ start ff_sphulv_chain_rw_ ################ */
/**
 * \defgroup ff_sphulv_chain_rw_ SphULV+Chain(RW)
 * \ingroup ff_local_planar_micelles
 *
 * \brief \<some brief description of SphULV+Chain(RW) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b n</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_sphulv_chain_rw_
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_sphulv_chain_rw_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphulv_chain_rw_
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_sphulv_chain_rw__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sphulv_chain_rw_
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_sphulv_chain_rw__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_sphulv_chain_rw_ ################ */

/* ################ start ff_ellulv_chain_rw__nagg ################ */
/**
 * \defgroup ff_ellulv_chain_rw__nagg EllULV+Chain(RW)_nagg
 * \ingroup ff_local_planar_micelles
 *
 * \brief \<some brief description of EllULV+Chain(RW)_nagg function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b n</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ellulv_chain_rw__nagg
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_ellulv_chain_rw__nagg(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellulv_chain_rw__nagg
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_ellulv_chain_rw__nagg_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellulv_chain_rw__nagg
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_ellulv_chain_rw__nagg_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ellulv_chain_rw__nagg ################ */

/* ################ start ff_ellulv_chain_rw__tc ################ */
/**
 * \defgroup ff_ellulv_chain_rw__tc EllULV+Chain(RW)_tc
 * \ingroup ff_local_planar_micelles
 *
 * \brief \<some brief description of EllULV+Chain(RW)_tc function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b n</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ellulv_chain_rw__tc
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_ellulv_chain_rw__tc(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellulv_chain_rw__tc
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_ellulv_chain_rw__tc_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellulv_chain_rw__tc
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_ellulv_chain_rw__tc_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ellulv_chain_rw__tc ################ */

/* ################ start ff_ellulv_chain_rw_ ################ */
/**
 * \defgroup ff_ellulv_chain_rw_ EllULV+Chain(RW)
 * \ingroup ff_local_planar_micelles
 *
 * \brief \<some brief description of EllULV+Chain(RW) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b n</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ellulv_chain_rw_
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_ellulv_chain_rw_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellulv_chain_rw_
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_ellulv_chain_rw__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellulv_chain_rw_
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_ellulv_chain_rw__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ellulv_chain_rw_ ################ */

/* ################ start ff_cylulv_chain_rw__nagg ################ */
/**
 * \defgroup ff_cylulv_chain_rw__nagg CylULV+Chain(RW)_nagg
 * \ingroup ff_local_planar_micelles
 *
 * \brief \<some brief description of CylULV+Chain(RW)_nagg function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b n</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_cylulv_chain_rw__nagg
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_cylulv_chain_rw__nagg(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylulv_chain_rw__nagg
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_cylulv_chain_rw__nagg_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylulv_chain_rw__nagg
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_cylulv_chain_rw__nagg_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_cylulv_chain_rw__nagg ################ */

/* ################ start ff_cylulv_chain_rw__tc ################ */
/**
 * \defgroup ff_cylulv_chain_rw__tc CylULV+Chain(RW)_tc
 * \ingroup ff_local_planar_micelles
 *
 * \brief \<some brief description of CylULV+Chain(RW)_tc function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b n</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_cylulv_chain_rw__tc
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_cylulv_chain_rw__tc(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylulv_chain_rw__tc
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_cylulv_chain_rw__tc_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylulv_chain_rw__tc
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_cylulv_chain_rw__tc_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_cylulv_chain_rw__tc ################ */

/* ################ start ff_cylulv_chain_rw_ ################ */
/**
 * \defgroup ff_cylulv_chain_rw_ CylULV+Chain(RW)
 * \ingroup ff_local_planar_micelles
 *
 * \brief \<some brief description of CylULV+Chain(RW) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b n</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_cylulv_chain_rw_
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_cylulv_chain_rw_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylulv_chain_rw_
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_cylulv_chain_rw__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cylulv_chain_rw_
 *
 * \sa sasfit_local_planar_micelles.h, ff_local_planar_micelles
 */
sasfit_local_planar_micelles_DLLEXP scalar sasfit_ff_cylulv_chain_rw__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_cylulv_chain_rw_ ################ */


#endif // this file

