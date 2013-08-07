/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_FERROFLUID_H
#define SASFIT_PLUGIN_FERROFLUID_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_ferrofluid.h
 * Public available functions and descriptions of the ferrofluid plugin.
 */

/**
 * \def sasfit_ferrofluid_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_ferrofluid_EXPORTS)
	#ifdef sasfit_ferrofluid_DLLEXP
	#undef sasfit_ferrofluid_DLLEXP
	#endif
	#define sasfit_ferrofluid_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_ferrofluid_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_ferrofluid_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_saturated_ff_chain_rw_radial ################ */
/**
 * \defgroup ff_saturated_ff_chain_rw_radial saturated FF+(Chain, RW) - (rad. avg.)
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief \<some brief description of saturated FF+(Chain, RW) - (rad. avg.) function\>
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_rw_radial
 *
 * \sa sasfit_ferrofluid.h, ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_radial(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_rw_radial
 *
 * \sa sasfit_ferrofluid.h, ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_radial_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_rw_radial
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_radial_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_rw_radial ################ */

/* ################ start ff_saturated_ff_chain_rw_psi ################ */
/**
 * \defgroup ff_saturated_ff_chain_rw_psi saturated FF+(Chain, RW) - psi
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief \<some brief description of saturated FF+(Chain, RW) - psi function\>
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between scattering and magnetization vector</td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_rw_psi
 *
 * \sa sasfit_ferrofluid.h, ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_psi(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_rw_psi
 *
 * \sa sasfit_ferrofluid.h, ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_psi_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_rw_psi
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_psi_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_rw_psi ################ */

/* ################ start ff_saturated_ff_chain_rw_psi_pp ################ */
/**
 * \defgroup ff_saturated_ff_chain_rw_psi_pp saturated FF+(Chain, RW) - psi (++)
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief some brief description of saturated FF+(Chain, RW) - psi (++) function
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between scattering and magnetization vector</td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_rw_psi_pp
 *
 * \sa sasfit_ferrofluid.h, ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_psi_pp(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_rw_psi_pp
 *
 * \sa sasfit_ferrofluid.h, ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_psi_pp_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_rw_psi_pp
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_psi_pp_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_rw_psi_pp ################ */

/* ################ start ff_saturated_ff_chain_rw_psi_mm ################ */
/**
 * \defgroup ff_saturated_ff_chain_rw_psi_mm saturated FF+(Chain, RW) - psi (--)
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief some brief description of saturated FF+(Chain, RW) - psi (--) function
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between scattering and magnetization vector</td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_rw_psi_mm
 *
 * \sa sasfit_ferrofluid.h, ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_psi_mm(scalar q, sasfit_param * p);

/**`
 * \ingroup ff_saturated_ff_chain_rw_psi_mm
 *
 * \sa sasfit_ferrofluid.h, ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_psi_mm_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_rw_psi_mm
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_psi_mm_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_rw_psi_mm ################ */

/* ################ start ff_saturated_ff_chain_rw_psi_pm ################ */
/**
 * \defgroup ff_saturated_ff_chain_rw_psi_pm saturated FF+(Chain, RW) - psi (+-)
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief some brief description of saturated FF+(Chain, RW) - psi (+-) function
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between scattering and magnetization vector</td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_rw_psi_pm
 *
 * \sa sasfit_ferrofluid.h, ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_psi_pm(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_rw_psi_pm
 *
 * \sa sasfit_ferrofluid.h, ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_psi_pm_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_rw_psi_pm
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_psi_pm_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_rw_psi_pm ################ */

/* ################ start ff_saturated_ff_chain_rw_psi_mp ################ */
/**
 * \defgroup ff_saturated_ff_chain_rw_psi_mp saturated FF+(Chain, RW) - psi (-+)
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief some brief description of saturated FF+(Chain, RW) - psi (-+) function
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between scattering and magnetization vector</td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_rw_psi_mp
 *
 * \sa sasfit_ferrofluid.h, ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_psi_mp(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_rw_psi_mp
 *
 * \sa sasfit_ferrofluid.h, ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_psi_mp_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_rw_psi_mp
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_psi_mp_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_rw_psi_mp ################ */

/* ################ start ff_saturated_ff_chain_rw_magnetic ################ */
/**
 * \defgroup ff_saturated_ff_chain_rw_magnetic saturated FF+(Chain, RW) - magnetic
 * \ingroup  ff_plugins_ferrofluid
 *
 * \brief \<some brief description of saturated FF+(Chain, RW) - magnetic function\>
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_rw_magnetic
 *
 * \sa sasfit_ferrofluid.h,  ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_magnetic(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_rw_magnetic
 *
 * \sa sasfit_ferrofluid.h,  ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_magnetic_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_rw_magnetic
 *
 * \sa sasfit_ferrofluid.h,  ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_magnetic_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_rw_magnetic ################ */

/* ################ start ff_saturated_ff_chain_rw_cross_term_radial ################ */
/**
 * \defgroup ff_saturated_ff_chain_rw_cross_term_radial saturated FF+(Chain, RW) - cross-term (rad. avg.)
 * \ingroup  ff_plugins_ferrofluid
 *
 * \brief \<some brief description of saturated FF+(Chain, RW) - cross-term (rad. avg.) function\>
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_rw_cross_term_radial
 *
 * \sa sasfit_ferrofluid.h,  ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_cross_term_radial(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_rw_cross_term
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_cross_term_radial_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_rw_cross_term_radial
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_cross_term_radial_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_rw_cross_term_radial ################ */


/* ################ start ff_saturated_ff_chain_rw_cross_term ################ */
/**
 * \defgroup ff_saturated_ff_chain_rw_cross_term saturated FF+(Chain, RW) - cross-term
 * \ingroup  ff_plugins_ferrofluid
 *
 * \brief \<some brief description of saturated FF+(Chain, RW) - cross-term function\>
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between scattering and magnetization vector</td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_rw_cross_term
 *
 * \sa sasfit_ferrofluid.h,  ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_cross_term(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_rw_cross_term
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_cross_term_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_rw_cross_term
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_rw_cross_term_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_rw_cross_term ################ */

/* ################ start ff_saturated_ff_chain_saw_psi ################ */
/**
 * \defgroup ff_saturated_ff_chain_saw_psi saturated FF+(Chain, SAW) - psi
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief \<some brief description of saturated FF+(Chain, SAW) - psi function\>
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between scattering and magnetization vector</td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr><tr>
 *       <td>\b t_brush_const</td>
 *       <td>box width of box with Gaussian tail</td>
 *      </tr><tr>
 *       <td>\b sigma_brush_Gaussian</td>
 *       <td>Gaussian width of box with Gaussian tail </td>
 *      </tr><tr>
 *       <td>\b L_b</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_saw_psi
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_psi(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_saw_psi
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_psi_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_saw_psi
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_psi_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_saw_psi ################ */

/* ################ start ff_saturated_ff_chain_saw_psi_pp ################ */
/**
 * \defgroup ff_saturated_ff_chain_saw_psi_pp saturated FF+(Chain, SAW) - psi (++)
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief some brief description of saturated FF+(Chain, SAW) - psi (++) function
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between scattering and magnetization vector</td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr><tr>
 *       <td>\b t_brush_const</td>
 *       <td>box width of box with Gaussian tail</td>
 *      </tr><tr>
 *       <td>\b sigma_brush_Gaussian</td>
 *       <td>Gaussian width of box with Gaussian tail </td>
 *      </tr><tr>
 *       <td>\b L_b</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_saw_psi_pp
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_psi_pp(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_saw_psi_pp
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_psi_pp_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_saw_psi_pp
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_psi_pp_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_saw_psi_pp ################ */

/* ################ start ff_saturated_ff_chain_saw_psi_mm ################ */
/**
 * \defgroup ff_saturated_ff_chain_saw_psi_mm saturated FF+(Chain, SAW) - psi (--)
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief some brief description of saturated FF+(Chain, SAW) - psi (--) function
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between scattering and magnetization vector</td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr><tr>
 *       <td>\b t_brush_const</td>
 *       <td>box width of box with Gaussian tail</td>
 *      </tr><tr>
 *       <td>\b sigma_brush_Gaussian</td>
 *       <td>Gaussian width of box with Gaussian tail </td>
 *      </tr><tr>
 *       <td>\b L_b</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_saw_psi_mm
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_psi_mm(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_saw_psi_mm
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_psi_mm_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_saw_psi_mm
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_psi_mm_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_saw_psi_mm ################ */

/* ################ start ff_saturated_ff_chain_saw_psi_pm ################ */
/**
 * \defgroup ff_saturated_ff_chain_saw_psi_pm saturated FF+(Chain, SAW) - psi (+-)
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief some brief description of saturated FF+(Chain, SAW) - psi (+-) function
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between scattering and magnetization vector</td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr><tr>
 *       <td>\b t_brush_const</td>
 *       <td>box width of box with Gaussian tail</td>
 *      </tr><tr>
 *       <td>\b sigma_brush_Gaussian</td>
 *       <td>Gaussian width of box with Gaussian tail </td>
 *      </tr><tr>
 *       <td>\b L_b</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_saw_psi_pm
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_psi_pm(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_saw_psi_pm
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_psi_pm_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_saw_psi_pm
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_psi_pm_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_saw_psi_pm ################ */

/* ################ start ff_saturated_ff_chain_saw_psi_mp ################ */
/**
 * \defgroup ff_saturated_ff_chain_saw_psi_mp saturated FF+(Chain, SAW) - psi (-+)
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief some brief description of saturated FF+(Chain, SAW) - psi (-+) function
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between scattering and magnetization vector</td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr><tr>
 *       <td>\b t_brush_const</td>
 *       <td>box width of box with Gaussian tail</td>
 *      </tr><tr>
 *       <td>\b sigma_brush_Gaussian</td>
 *       <td>Gaussian width of box with Gaussian tail </td>
 *      </tr><tr>
 *       <td>\b L_b</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_saw_psi_mp
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_psi_mp(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_saw_psi_mp
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_psi_mp_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_saw_psi_mp
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_psi_mp_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_saw_psi_mp ################ */

/* ################ start ff_saturated_ff_chain_saw_psi_pm ################ */
/**
 * \defgroup ff_saturated_ff_chain_saw_psi_pm saturated FF+(Chain, SAW) - psi (+-)
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief some brief description of saturated FF+(Chain, SAW) - psi (+-) function
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between scattering and magnetization vector</td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr><tr>
 *       <td>\b t_brush_const</td>
 *       <td>box width of box with Gaussian tail</td>
 *      </tr><tr>
 *       <td>\b sigma_brush_Gaussian</td>
 *       <td>Gaussian width of box with Gaussian tail </td>
 *      </tr><tr>
 *       <td>\b L_b</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_saw_psi_pm
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_psi_pm(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_saw_psi_pm
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_psi_pm_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_saw_psi_pm
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_psi_pm_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_saw_psi_pm ################ */

/* ################ start ff_saturated_ff_chain_saw_radial ################ */
/**
 * \defgroup ff_saturated_ff_chain_saw_radial saturated FF+(Chain, SAW) - (rad. avg.)
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief \<some brief description of saturated FF+(Chain, SAW) - (rad. avg.) function\>
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr><tr>
 *       <td>\b t_brush_const</td>
 *       <td>box width of box with Gaussian tail</td>
 *      </tr><tr>
 *       <td>\b sigma_brush_Gaussian</td>
 *       <td>Gaussian width of box with Gaussian tail </td>
 *      </tr><tr>
 *       <td>\b L_b</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_saw_radial
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_radial(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_saw_radial
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_radial_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_saw_radial
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_radial_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_saw_radial ################ */


/* ################ start ff_saturated_ff_chain_saw_magnetic ################ */
/**
 * \defgroup ff_saturated_ff_chain_saw_magnetic saturated FF+(Chain, SAW) - magnetic
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief \<some brief description of saturated FF+(Chain, SAW) - magnetic function\>
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr><tr>
 *       <td>\b t_brush_const</td>
 *       <td>box width of box with Gaussian tail</td>
 *      </tr><tr>
 *       <td>\b sigma_brush_Gaussian</td>
 *       <td>Gaussian width of box with Gaussian tail </td>
 *      </tr><tr>
 *       <td>\b L_b</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_saw_magnetic
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_magnetic(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_saw_magnetic
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_magnetic_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_saw_magnetic
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_magnetic_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_saw_magnetic ################ */

/* ################ start ff_saturated_ff_chain_saw_cross_term ################ */
/**
 * \defgroup ff_saturated_ff_chain_saw_cross_term saturated FF+(Chain, SAW) - cross-term
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief \<some brief description of saturated FF+(Chain, SAW) - cross-term function\>
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between scattering and magnetization vector</td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr><tr>
 *       <td>\b t_brush_const</td>
 *       <td>box width of box with Gaussian tail</td>
 *      </tr><tr>
 *       <td>\b sigma_brush_Gaussian</td>
 *       <td>Gaussian width of box with Gaussian tail </td>
 *      </tr><tr>
 *       <td>\b L_b</td>
 *       <td>ratio of contour and Kuhn length</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_saw_cross_term
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_cross_term(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_saw_cross_term
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_cross_term_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_saw_cross_term
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_cross_term_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_saw_cross_term ################ */

/* ################ start ff_saturated_ff_chain_saw_cross_term_radial ################ */
/**
 * \defgroup ff_saturated_ff_chain_saw_cross_term_radial saturated FF+(Chain, SAW) - cross-term (rad. avg.)
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief \<some brief description of saturated FF+(Chain, SAW) - cross-term (rad. avg.) function\>
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr><tr>
 *       <td>\b t_brush_const</td>
 *       <td>box width of box with Gaussian tail</td>
 *      </tr><tr>
 *       <td>\b sigma_brush_Gaussian</td>
 *       <td>Gaussian width of box with Gaussian tail </td>
 *      </tr><tr>
 *       <td>\b L_b</td>
 *       <td>ratio of contour and Kuhn length</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_saw_cross_term_radial
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_cross_term_radial(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_saw_cross_term_radial
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_cross_term_radial_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_saw_cross_term_radial
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_saw_cross_term_radial_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_saw_cross_term_radial ################ */

/* ################ start ff_saturated_ff_chain_parabolic_psi ################ */
/**
 * \defgroup ff_saturated_ff_chain_parabolic_psi saturated FF+(Chain, parabolic) - psi
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief \<some brief description of saturated FF+(Chain, parabolic) - psi function\>
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between scattering and magnetization vector</td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td>dimensionless persistence length </td>
 *      </tr><tr>
 *       <td>\b a</td>
 *       <td>monomer size</td>
 *      </tr><tr>
 *       <td>\b tau</td>
 *       <td>solvent quality parameter tau=1-Theta/T</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_parabolic_psi
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_psi(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_parabolic_psi
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_psi_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_parabolic_psi
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_psi_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_parabolic_psi ################ */

/* ################ start ff_saturated_ff_chain_parabolic_psi_pp ################ */
/**
 * \defgroup ff_saturated_ff_chain_parabolic_psi_pp saturated FF+(Chain, parabolic) - psi (++)
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief some brief description of saturated FF+(Chain, parabolic) - psi (++) function
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between scattering and magnetization vector</td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td>dimensionless persistence length </td>
 *      </tr><tr>
 *       <td>\b a</td>
 *       <td>monomer size</td>
 *      </tr><tr>
 *       <td>\b tau</td>
 *       <td>solvent quality parameter tau=1-Theta/T</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_parabolic_psi_pp
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_psi_pp(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_parabolic_psi_pp
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_psi_pp_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_parabolic_psi_pp
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_psi_pp_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_parabolic_psi_pp ################ */

/* ################ start ff_saturated_ff_chain_parabolic_psi_mm ################ */
/**
 * \defgroup ff_saturated_ff_chain_parabolic_psi_pp saturated FF+(Chain, parabolic) - psi (--)
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief some brief description of saturated FF+(Chain, parabolic) - psi (--) function
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between scattering and magnetization vector</td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td>dimensionless persistence length </td>
 *      </tr><tr>
 *       <td>\b a</td>
 *       <td>monomer size</td>
 *      </tr><tr>
 *       <td>\b tau</td>
 *       <td>solvent quality parameter tau=1-Theta/T</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_parabolic_psi_mm
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_psi_mm(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_parabolic_psi_mm
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_psi_mm_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_parabolic_psi_mm
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_psi_mm_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_parabolic_psi_mm ################ */

/* ################ start ff_saturated_ff_chain_parabolic_psi_pm ################ */
/**
 * \defgroup ff_saturated_ff_chain_parabolic_psi_pm saturated FF+(Chain, parabolic) - psi (+-)
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief some brief description of saturated FF+(Chain, parabolic) - psi (+-) function
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between scattering and magnetization vector</td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td>dimensionless persistence length </td>
 *      </tr><tr>
 *       <td>\b a</td>
 *       <td>monomer size</td>
 *      </tr><tr>
 *       <td>\b tau</td>
 *       <td>solvent quality parameter tau=1-Theta/T</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_parabolic_psi_pm
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_psi_pm(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_parabolic_psi_pm
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_psi_pm_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_parabolic_psi_pm
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_psi_pm_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_parabolic_psi_pm ################ */

/* ################ start ff_saturated_ff_chain_parabolic_psi_mp ################ */
/**
 * \defgroup ff_saturated_ff_chain_parabolic_psi_mp saturated FF+(Chain, parabolic) - psi (-+)
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief some brief description of saturated FF+(Chain, parabolic) - psi (-+) function
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between scattering and magnetization vector</td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td>dimensionless persistence length </td>
 *      </tr><tr>
 *       <td>\b a</td>
 *       <td>monomer size</td>
 *      </tr><tr>
 *       <td>\b tau</td>
 *       <td>solvent quality parameter tau=1-Theta/T</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_parabolic_psi_mp
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_psi_mp(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_parabolic_psi_mp
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_psi_mp_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_parabolic_psi_mp
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_psi_mp_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_parabolic_psi_mp ################ */

/* ################ start ff_saturated_ff_chain_parabolic_radial ################ */
/**
 * \defgroup ff_saturated_ff_chain_parabolic_radial saturated FF+(Chain, parabolic) - radial
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief \<some brief description of saturated FF+(Chain, parabolic) - radial function\>
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td>dimensionless persistence length </td>
 *      </tr><tr>
 *       <td>\b a</td>
 *       <td>monomer size</td>
 *      </tr><tr>
 *       <td>\b tau</td>
 *       <td>solvent quality parameter tau=1-Theta/T</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_parabolic_radial
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_radial(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_parabolic_radial
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_radial_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_parabolic_radial
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_radial_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_parabolic_radial ################ */


/* ################ start ff_saturated_ff_chain_parabolic_magnetic ################ */
/**
 * \defgroup ff_saturated_ff_chain_parabolic_magnetic saturated FF+(Chain, parabolic) - magnetic
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief \<some brief description of saturated FF+(Chain, parabolic) - magnetic function\>
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td>dimensionless persistence length </td>
 *      </tr><tr>
 *       <td>\b a</td>
 *       <td>monomer size</td>
 *      </tr><tr>
 *       <td>\b tau</td>
 *       <td>solvent quality parameter tau=1-Theta/T</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_parabolic_magnetic
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_magnetic(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_parabolic_magnetic
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_magnetic_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_parabolic_magnetic
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_magnetic_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_parabolic_magnetic ################ */

/* ################ start ff_saturated_ff_chain_parabolic_cross_term ################ */
/**
 * \defgroup ff_saturated_ff_chain_parabolic_cross_term saturated FF+(Chain, parabolic) - cross-term
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief \<some brief description of saturated FF+(Chain, parabolic) - cross-term function\>
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td>angle between scattering and magnetization vector</td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td>dimensionless persistence length </td>
 *      </tr><tr>
 *       <td>\b a</td>
 *       <td>monomer size</td>
 *      </tr><tr>
 *       <td>\b tau</td>
 *       <td>solvent quality parameter tau=1-Theta/T</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_parabolic_cross_term
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_cross_term(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_parabolic_cross_term
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_cross_term_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_parabolic_cross_term
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_cross_term_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_parabolic_cross_term ################ */

/* ################ start ff_saturated_ff_chain_parabolic_cross_term_radial ################ */
/**
 * \defgroup ff_saturated_ff_chain_parabolic_cross_term_radial saturated FF+(Chain, parabolic) - cross-term (rad. avg.)
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief \<some brief description of saturated FF+(Chain, parabolic) - cross-term (rad. avg.) function\>
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td>dimensionless persistence length </td>
 *      </tr><tr>
 *       <td>\b a</td>
 *       <td>monomer size</td>
 *      </tr><tr>
 *       <td>\b tau</td>
 *       <td>solvent quality parameter tau=1-Theta/T</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_saturated_ff_chain_parabolic_cross_term_radial
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_cross_term_radial(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_parabolic_cross_term_radial
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_cross_term_radial_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_saturated_ff_chain_parabolic_cross_term_radial
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_saturated_ff_chain_parabolic_cross_term_radial_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_saturated_ff_chain_parabolic_cross_term_radial ################ */



/* ################ start ff_chain_parabolic_profile ################ */
/**
 * \defgroup ff_chain_parabolic_profile  FF+(Chain, parabolic) - radial profile
 * \ingroup ff_plugins_ferrofluid
 *
 * \brief \<some brief description of FF+(Chain, parabolic) - radial profile function\>
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
 *       <td>\b t_shell</td>
 *       <td>thickness of the solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b nagg</td>
 *       <td>specific aggegation number \n number of chains per surface area</td>
 *      </tr><tr>
 *       <td>\b Vbrush</td>
 *       <td>molecular volume of a single polymer chain</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>nuclear scattering lenth density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>nuclear scattering lenth density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b eta_brush</td>
 *       <td>scattering length density of polymer in corona</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_mag_core</td>
 *       <td>magnetic scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_mag_shell</td>
 *       <td>magnetic scattering length density of solid shell of the core</td>
 *      </tr><tr>
 *       <td>\b R_av</td>
 *       <td>average particle radius, for which alpha is given below</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>potential energy of magnetic moment in applied magnetic field divided by thermal energy</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration of unpertubated polymer chain in corona</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b pol</td>
 *       <td>incident neutron polarisation</td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td>dimensionless persistence length </td>
 *      </tr><tr>
 *       <td>\b a</td>
 *       <td>monomer size</td>
 *      </tr><tr>
 *       <td>\b tau</td>
 *       <td>solvent quality parameter tau=1-Theta/T</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_chain_parabolic_profile
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_chain_parabolic_profile(scalar q, sasfit_param * p);

/**
 * \ingroup ff_chain_parabolic_profile
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_chain_parabolic_profile_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_chain_parabolic_profile
 *
 * \sa sasfit_ferrofluid.h, ff_plugins_ferrofluid
 */
sasfit_ferrofluid_DLLEXP scalar sasfit_ff_chain_parabolic_profile_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_chain_parabolic_profile ################ */

#endif // this file

