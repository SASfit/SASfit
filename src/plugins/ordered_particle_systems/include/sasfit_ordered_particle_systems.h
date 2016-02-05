/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_ORDERED_PARTICLE_SYSTEMS_H
#define SASFIT_PLUGIN_ORDERED_PARTICLE_SYSTEMS_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_ordered_particle_systems.h
 * Public available functions and descriptions of the ordered_particle_systems plugin.
 */

/**
 * \def sasfit_ordered_particle_systems_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_ordered_particle_systems_EXPORTS)
	#ifdef sasfit_ordered_particle_systems_DLLEXP
	#undef sasfit_ordered_particle_systems_DLLEXP
	#endif
	#define sasfit_ordered_particle_systems_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_ordered_particle_systems_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_ordered_particle_systems_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq_sc_iso_lorentzian ################ */
/**
 * \defgroup sq_sc_iso_lorentzian SC-iso-Lorentzian
 * \ingroup sq_plugins_SC
 *
 * \brief \<some brief description of SC function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b cL</td>
 *       <td>scaling constant for Z0</td>
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
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_sc_iso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_SC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_sc_iso_lorentzian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sc_iso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_SC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_sc_iso_lorentzian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sc_iso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_SC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_sc_iso_lorentzian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_sc_iso_lorentzian ################ */

/* ################ start sq_fcc_iso_lorentzian ################ */
/**
 * \defgroup sq_fcc_iso_lorentzian FCC-iso-Lorentzian
 * \ingroup sq_plugins_FCC
 *
 * \brief \<some brief description of FCC function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b cL</td>
 *       <td>scaling constant for Z0</td>
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
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_fcc_iso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_FCC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_fcc_iso_lorentzian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_fcc_iso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_FCC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_fcc_iso_lorentzian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_fcc_iso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_FCC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_fcc_iso_lorentzian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_fcc_iso_lorentzian ################ */


/* ################ start sq_fcc_iso_gaussian ################ */
/**
 * \defgroup sq_fcc_iso_lorentzian FCC-iso-Gaussian
 * \ingroup sq_plugins_FCC
 *
 * \brief \<some brief description of FCC function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b cL</td>
 *       <td>scaling constant for Z0</td>
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
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_fcc_iso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_FCC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_fcc_iso_gaussian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_fcc_iso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_FCC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_fcc_iso_gaussian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_fcc_iso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_FCC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_fcc_iso_gaussian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_fcc_iso_gaussian ################ */


/* ################ start sq_hcp_iso_lorentzian ################ */
/**
 * \defgroup sq_hcp_iso_lorentzian HCP-iso-Lorentzian
 * \ingroup sq_plugins_HCP
 *
 * \brief \<some brief description of HCP function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b cL</td>
 *       <td>scaling constant for Z0</td>
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
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_hcp_iso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HCP
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hcp_iso_lorentzian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hcp_iso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HCP
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hcp_iso_lorentzian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hcp_iso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HCP
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hcp_iso_lorentzian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_hcp_iso_lorentzian ################ */

/* ################ start sq_hcp_iso_gaussian ################ */
/**
 * \defgroup sq_hcp_iso_gaussian HCP-iso-Gaussian
 * \ingroup sq_plugins_HCP
 *
 * \brief \<some brief description of HCP function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b cL</td>
 *       <td>scaling constant for Z0</td>
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
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_hcp_iso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HCP
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hcp_iso_gaussian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hcp_iso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HCP
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hcp_iso_gaussian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hcp_iso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HCP
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hcp_iso_gaussian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_hcp_iso_gaussian ################ */

/* ################ start sq_bcc_iso_lorentzian ################ */
/**
 * \defgroup sq_bcc_iso_lorentzian BCC-iso-Lorentzian
 * \ingroup sq_plugins_BCC
 *
 * \brief \<some brief description of BCC function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b cL</td>
 *       <td>scaling constant for Z0</td>
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
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_bcc_iso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_BCC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_bcc_iso_lorentzian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_bcc_iso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_BCC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_bcc_iso_lorentzian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_bcc_iso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_BCC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_bcc_iso_lorentzian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_bcc_iso_lorentzian ################ */

/* ################ start sq_bcc_iso_gaussian ################ */
/**
 * \defgroup sq_bcc_iso_lorentzian BCC-iso-Gaussian
 * \ingroup sq_plugins_BCC
 *
 * \brief \<some brief description of BCC function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b cL</td>
 *       <td>scaling constant for Z0</td>
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
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_bcc_iso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_BCC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_bcc_iso_gaussian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_bcc_iso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_BCC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_bcc_iso_gaussian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_bcc_iso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_BCC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_bcc_iso_gaussian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_bcc_iso_gaussian ################ */

/* ################ start sq_hex_iso_lorentzian ################ */
/**
 * \defgroup sq_hex_iso_lorentzian HEX-iso-Lorentzian
 * \ingroup sq_plugins_HEX
 *
 * \brief \<some brief description of HEX function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b cL</td>
 *       <td>scaling constant for Z0</td>
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
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_hex_iso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HEX
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hex_iso_lorentzian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hex_iso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HEX
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hex_iso_lorentzian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hex_iso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HEX
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hex_iso_lorentzian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_hex_iso_lorentzian ################ */

/* ################ start sq_hex_iso_gaussian ################ */
/**
 * \defgroup sq_hex_iso_gaussian HEX-iso-Gaussian
 * \ingroup sq_plugins_HEX
 *
 * \brief \<some brief description of HEX function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b cL</td>
 *       <td>scaling constant for Z0</td>
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
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_hex_iso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HEX
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hex_iso_gaussian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hex_iso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HEX
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hex_iso_gaussian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hex_iso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HEX
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hex_iso_gaussian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_hex_iso_gaussian ################ */

/* ################ start sq_sc_iso_gaussian ################ */
/**
 * \defgroup sq_sc_iso_gaussian SC-iso-Gaussian
 * \ingroup sq_plugins_SC
 *
 * \brief \<some brief description of SC function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b cL</td>
 *       <td>scaling constant for Z0</td>
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
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_sc_iso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_SC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_sc_iso_gaussian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sc_iso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_SC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_sc_iso_gaussian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sc_iso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_SC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_sc_iso_gaussian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_sc_iso_gaussian ################ */

/* ################ start sq_lam_iso_lorentzian ################ */
/**
 * \defgroup sq_lam_iso_lorentzian LAM-iso-Lorentzian
 * \ingroup sq_plugins_LAM
 *
 * \brief \<some brief description of LAM function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b cL</td>
 *       <td>scaling constant for Z0</td>
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
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_lam_iso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_LAM
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_lam_iso_lorentzian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_lam_iso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_LAM
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_lam_iso_lorentzian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_lam_iso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_LAM
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_lam_iso_lorentzian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_lam_iso_lorentzian ################ */


/* ################ start sq_lam_iso_gaussian ################ */
/**
 * \defgroup sq_lam_iso_gaussian LAM-iso-Gaussian
 * \ingroup sq_plugins_LAM
 *
 * \brief \<some brief description of LAM function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b cL</td>
 *       <td>scaling constant for Z0</td>
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
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_lam_iso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_LAM
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_lam_iso_gaussian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_lam_iso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_LAM
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_lam_iso_gaussian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_lam_iso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_LAM
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_lam_iso_gaussian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_lam_iso_gaussian ################ */


/* ################ start sq_crec_iso_lorentzian ################ */
/**
 * \defgroup sq_crec_iso_lorentzian CREC-iso-Lorentzian
 * \ingroup sq_plugins_CREC
 *
 * \brief \<some brief description of CREC function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b b</td>
 *       <td>lattice constant b</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b cL</td>
 *       <td>scaling constant for Z0</td>
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
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_crec_iso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_CREC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_crec_iso_lorentzian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_crec_iso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_CREC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_crec_iso_lorentzian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_crec_iso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_CREC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_crec_iso_lorentzian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_lam_iso_lorentzian ################ */


/* ################ start sq_crec_iso_gaussian ################ */
/**
 * \defgroup sq_crec_iso_gaussian CREC-iso-Gaussian
 * \ingroup sq_plugins_CREC
 *
 * \brief \<some brief description of CREC function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant a</td>
 *      </tr><tr>
 *       <td>\b b</td>
 *       <td>lattice constant b</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b cL</td>
 *       <td>scaling constant for Z0</td>
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
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_crec_iso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_CREC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_crec_iso_gaussian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_crec_iso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_CREC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_crec_iso_gaussian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_crec_iso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_CREC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_crec_iso_gaussian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_crec_iso_gaussian ################ */


/* ################ start sq_sq_iso_lorentzian ################ */
/**
 * \defgroup sq_sq_iso_lorentzian SQ-iso-Lorentzian
 * \ingroup sq_plugins_SQ
 *
 * \brief \<some brief description of SQ function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b cL</td>
 *       <td>scaling constant for Z0</td>
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
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_sq_iso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_SQ
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_sq_iso_lorentzian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sq_iso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_SQ
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_sq_iso_lorentzian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sq_iso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_SQ
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_sq_iso_lorentzian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_lam_iso_lorentzian ################ */


/* ################ start sq_sq_iso_gaussian ################ */
/**
 * \defgroup sq_sq_iso_gaussian SQ-iso-Gaussian
 * \ingroup sq_plugins_SQ
 *
 * \brief \<some brief description of SQ function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant a</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b cL</td>
 *       <td>scaling constant for Z0</td>
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
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_sq_iso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_SQ
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_sq_iso_gaussian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sq_iso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_SQ
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_sq_iso_gaussian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sq_iso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_SQ
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_sq_iso_gaussian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_sq_iso_gaussian ################ */













/* ################ start sq_sc_aniso_lorentzian ################ */
/**
 * \defgroup sq_sc_aniso_lorentzian SC-aniso-Lorentzian
 * \ingroup sq_plugins_SC
 *
 * \brief \<some brief description of SC function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta_q</td>
 *       <td>radial peak width</td>
 *      </tr><tr>
 *       <td>\b delta_psi</td>
 *       <td>azimuthal peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b yaw</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b pitch</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b roll</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Psi (deg)</td>
 *       <td>azimuthal angle in detector plane in degree (Psi = 0: horizontal)</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>wavelength in reciprocal units of q</td>
 *      </tr><tr>
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_sc_aniso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_SC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_sc_aniso_lorentzian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sc_aniso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_SC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_sc_aniso_lorentzian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sc_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_SC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_sc_aniso_lorentzian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_sc_aniso_gaussian ################ */

/* ################ start sq_sc_aniso_gaussian ################ */
/**
 * \defgroup sq_sc_aniso_gaussian SC-aniso-Gaussian
 * \ingroup sq_plugins_SC
 *
 * \brief \<some brief description of SC function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta_q</td>
 *       <td>radial peak width</td>
 *      </tr><tr>
 *       <td>\b delta_psi</td>
 *       <td>azimuthal peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b yaw</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b pitch</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b roll</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Psi (deg)</td>
 *       <td>azimuthal angle in detector plane in degree (Psi = 0: horizontal)</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>wavelength in reciprocal units of q</td>
 *      </tr><tr>
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_sc_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_SC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_sc_aniso_gaussian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sc_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_SC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_sc_aniso_gaussian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sc_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_SC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_sc_aniso_gaussian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_sc_aniso_gaussian ################ */


/* ################ start sq__hcp_aniso_lorentzian ################ */
/**
 * \defgroup sq_hcp_aniso_lorentzian HCP-aniso-Lorentzian
 * \ingroup sq_plugins_HCP
 *
 * \brief \<some brief description of HCP function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta_q</td>
 *       <td>radial peak width</td>
 *      </tr><tr>
 *       <td>\b delta_psi</td>
 *       <td>azimuthal peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b yaw</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b pitch</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b roll</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Psi (deg)</td>
 *       <td>azimuthal angle in detector plane in degree (Psi = 0: horizontal)</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>wavelength in reciprocal units of q</td>
 *      </tr><tr>
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_hcp_aniso_lorentzian
 *
 * \sa sasfit_ordered_particle_systepms.h, sq_plugins_HCP
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hcp_aniso_lorentzian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hcp_aniso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HCP
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_hcp_aniso_lorentzian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hcp_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HCP
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hcp_aniso_lorentzian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_hcp_aniso_gaussian ################ */

/* ################ start sq_hcp_aniso_gaussian ################ */
/**
 * \defgroup sq_hcp_aniso_gaussian HCP_aniso-Gaussian
 * \ingroup sq_plugins_HCP
 *
 * \brief \<some brief description of HCP function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta_q</td>
 *       <td>radial peak width</td>
 *      </tr><tr>
 *       <td>\b delta_psi</td>
 *       <td>azimuthal peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b yaw</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b pitch</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b roll</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Psi (deg)</td>
 *       <td>azimuthal angle in detector plane in degree (Psi = 0: horizontal)</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>wavelength in reciprocal units of q</td>
 *      </tr><tr>
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_hcp_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HCP
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hcp_aniso_gaussian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hcp_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HCP
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hcp_aniso_gaussian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hcp_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HCP
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hcp_aniso_gaussian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_hcp_aniso_gaussian ################ */

/* ################ start sq_sq_aniso_lorentzian ################ */
/**
 * \defgroup sq_sq_aniso_lorentzian SQ-aniso-Lorentzian
 * \ingroup sq_plugins_SQ
 *
 * \brief \<some brief description of SQ function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta_q</td>
 *       <td>radial peak width</td>
 *      </tr><tr>
 *       <td>\b delta_psi</td>
 *       <td>azimuthal peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b yaw</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b pitch</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b roll</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Psi (deg)</td>
 *       <td>azimuthal angle in detector plane in degree (Psi = 0: horizontal)</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>wavelength in reciprocal units of q</td>
 *      </tr><tr>
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_sq_aniso_lorentzian
 *
 * \sa sasfit_ordered_particle_systepms.h, sq_plugins_SQ
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_sq_aniso_lorentzian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sq_aniso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_SQ
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_sq_aniso_lorentzian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sq_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_SQ
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_sq_aniso_lorentzian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_sq_aniso_gaussian ################ */

/* ################ start sq_sq_aniso_gaussian ################ */
/**
 * \defgroup sq_sq_aniso_gaussian SQ_aniso-Gaussian
 * \ingroup sq_plugins_SQ
 *
 * \brief \<some brief description of SQ function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta_q</td>
 *       <td>radial peak width</td>
 *      </tr><tr>
 *       <td>\b delta_psi</td>
 *       <td>azimuthal peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b yaw</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b pitch</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b roll</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Psi (deg)</td>
 *       <td>azimuthal angle in detector plane in degree (Psi = 0: horizontal)</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>wavelength in reciprocal units of q</td>
 *      </tr><tr>
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_sq_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_SQ
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_sq_aniso_gaussian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sq_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_SQ
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_sq_aniso_gaussian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_sq_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_SQ
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_sq_aniso_gaussian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_sq_aniso_gaussian ################ */


/* ################ start sq_crec_aniso_lorentzian ################ */
/**
 * \defgroup sq_crec_aniso_lorentzian CREC-aniso-Lorentzian
 * \ingroup sq_plugins_CREC
 *
 * \brief \<some brief description of CREC function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta_q</td>
 *       <td>radial peak width</td>
 *      </tr><tr>
 *       <td>\b delta_psi</td>
 *       <td>azimuthal peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b yaw</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b pitch</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b roll</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Psi (deg)</td>
 *       <td>azimuthal angle in detector plane in degree (Psi = 0: horizontal)</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>wavelength in reciprocal units of q</td>
 *      </tr><tr>
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_crec_aniso_lorentzian
 *
 * \sa sasfit_ordered_particle_systepms.h, sq_plugins_CREC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_crec_aniso_lorentzian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_crec_aniso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_CREC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_crec_aniso_lorentzian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_crec_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_CREC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_crec_aniso_lorentzian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_crec_aniso_gaussian ################ */

/* ################ start sq_crec_aniso_gaussian ################ */
/**
 * \defgroup sq_crec_aniso_gaussian CREC_aniso-Gaussian
 * \ingroup sq_plugins_CREC
 *
 * \brief \<some brief description of CREC function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta_q</td>
 *       <td>radial peak width</td>
 *      </tr><tr>
 *       <td>\b delta_psi</td>
 *       <td>azimuthal peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b yaw</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b pitch</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b roll</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Psi (deg)</td>
 *       <td>azimuthal angle in detector plane in degree (Psi = 0: horizontal)</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>wavelength in reciprocal units of q</td>
 *      </tr><tr>
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_crec_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_CREC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_crec_aniso_gaussian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_crec_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_CREC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_crec_aniso_gaussian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_crec_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_CREC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_crec_aniso_gaussian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_crec_aniso_gaussian ################ */


/* ################ start sq_lam_aniso_lorentzian ################ */
/**
 * \defgroup sq_lam_aniso_lorentzian LAM-aniso-Lorentzian
 * \ingroup sq_plugins_LAM
 *
 * \brief \<some brief description of LAM function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta_q</td>
 *       <td>radial peak width</td>
 *      </tr><tr>
 *       <td>\b delta_psi</td>
 *       <td>azimuthal peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b yaw</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b pitch</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b roll</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Psi (deg)</td>
 *       <td>azimuthal angle in detector plane in degree (Psi = 0: horizontal)</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>wavelength in reciprocal units of q</td>
 *      </tr><tr>
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_lam_aniso_lorentzian
 *
 * \sa sasfit_ordered_particle_systepms.h, sq_plugins_LAM
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_lam_aniso_lorentzian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_lam_aniso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_LAM
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_lam_aniso_lorentzian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_lam_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_LAM
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_lam_aniso_lorentzian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_lam_aniso_gaussian ################ */

/* ################ start sq_lam_aniso_gaussian ################ */
/**
 * \defgroup sq_lam_aniso_gaussian LAM_aniso-Gaussian
 * \ingroup sq_plugins_LAM
 *
 * \brief \<some brief description of LAM function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta_q</td>
 *       <td>radial peak width</td>
 *      </tr><tr>
 *       <td>\b delta_psi</td>
 *       <td>azimuthal peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b yaw</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b pitch</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b roll</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Psi (deg)</td>
 *       <td>azimuthal angle in detector plane in degree (Psi = 0: horizontal)</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>wavelength in reciprocal units of q</td>
 *      </tr><tr>
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_lam_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_LAM
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_lam_aniso_gaussian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_lam_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_LAM
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_lam_aniso_gaussian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_lam_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_LAM
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_lam_aniso_gaussian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_lam_aniso_gaussian ################ */


/* ################ start sq_hex_aniso_lorentzian ################ */
/**
 * \defgroup sq_hex_aniso_lorentzian HEX-aniso-Lorentzian
 * \ingroup sq_plugins_HEX
 *
 * \brief \<some brief description of HEX function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta_q</td>
 *       <td>radial peak width</td>
 *      </tr><tr>
 *       <td>\b delta_psi</td>
 *       <td>azimuthal peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b yaw</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b pitch</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b roll</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Psi (deg)</td>
 *       <td>azimuthal angle in detector plane in degree (Psi = 0: horizontal)</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>wavelength in reciprocal units of q</td>
 *      </tr><tr>
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_hex_aniso_lorentzian
 *
 * \sa sasfit_ordered_particle_systepms.h, sq_plugins_HEX
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hex_aniso_lorentzian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hex_aniso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HEX
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hex_aniso_lorentzian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hex_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HEX
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hex_aniso_lorentzian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_hex_aniso_gaussian ################ */

/* ################ start sq_hex_aniso_gaussian ################ */
/**
 * \defgroup sq_hex_aniso_gaussian HEX_aniso-Gaussian
 * \ingroup sq_plugins_HEX
 *
 * \brief \<some brief description of HEX function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta_q</td>
 *       <td>radial peak width</td>
 *      </tr><tr>
 *       <td>\b delta_psi</td>
 *       <td>azimuthal peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b yaw</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b pitch</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b roll</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Psi (deg)</td>
 *       <td>azimuthal angle in detector plane in degree (Psi = 0: horizontal)</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>wavelength in reciprocal units of q</td>
 *      </tr><tr>
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_hex_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HEX
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hex_aniso_gaussian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hex_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HEX
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hex_aniso_gaussian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hex_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HEX
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hex_aniso_gaussian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_hex_aniso_gaussian ################ */


/* ################ start sq_hcp_aniso_lorentzian ################ */
/**
 * \defgroup sq_hcp_aniso_lorentzian HCP-aniso-Lorentzian
 * \ingroup sq_plugins_HCP
 *
 * \brief \<some brief description of HCP function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta_q</td>
 *       <td>radial peak width</td>
 *      </tr><tr>
 *       <td>\b delta_psi</td>
 *       <td>azimuthal peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b yaw</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b pitch</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b roll</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Psi (deg)</td>
 *       <td>azimuthal angle in detector plane in degree (Psi = 0: horizontal)</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>wavelength in reciprocal units of q</td>
 *      </tr><tr>
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_hcp_aniso_lorentzian
 *
 * \sa sasfit_ordered_particle_systepms.h, sq_plugins_HCP
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hcp_aniso_lorentzian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hcp_aniso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HCP
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hcp_aniso_lorentzian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hcp_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HCP
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hcp_aniso_lorentzian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_hcp_aniso_gaussian ################ */

/* ################ start sq_hcp_aniso_gaussian ################ */
/**
 * \defgroup sq_hcp_aniso_gaussian HCP_aniso-Gaussian
 * \ingroup sq_plugins_HCP
 *
 * \brief \<some brief description of HCP function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta_q</td>
 *       <td>radial peak width</td>
 *      </tr><tr>
 *       <td>\b delta_psi</td>
 *       <td>azimuthal peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b yaw</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b pitch</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b roll</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Psi (deg)</td>
 *       <td>azimuthal angle in detector plane in degree (Psi = 0: horizontal)</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>wavelength in reciprocal units of q</td>
 *      </tr><tr>
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_hcp_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HCP
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hcp_aniso_gaussian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hcp_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HCP
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hcp_aniso_gaussian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hcp_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_HCP
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_hcp_aniso_gaussian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_hcp_aniso_gaussian ################ */

/* ################ start sq_fcc_aniso_lorentzian ################ */
/**
 * \defgroup sq_fcc_aniso_lorentzian FCC-aniso-Lorentzian
 * \ingroup sq_plugins_FCC
 *
 * \brief \<some brief description of FCC function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta_q</td>
 *       <td>radial peak width</td>
 *      </tr><tr>
 *       <td>\b delta_psi</td>
 *       <td>azimuthal peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b yaw</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b pitch</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b roll</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Psi (deg)</td>
 *       <td>azimuthal angle in detector plane in degree (Psi = 0: horizontal)</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>wavelength in reciprocal units of q</td>
 *      </tr><tr>
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_fcc_aniso_lorentzian
 *
 * \sa sasfit_ordered_particle_systepms.h, sq_plugins_FCC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_fcc_aniso_lorentzian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_fcc_aniso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_FCC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_fcc_aniso_lorentzian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_fcc_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_FCC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_fcc_aniso_lorentzian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_fcc_aniso_lorentzian ################ */

/* ################ start sq_fcc_aniso_gaussian ################ */
/**
 * \defgroup sq_fcc_aniso_gaussian  FCC_aniso-Gaussian
 * \ingroup sq_plugins_FCC
 *
 * \brief \<some brief description of FCC function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta_q</td>
 *       <td>radial peak width</td>
 *      </tr><tr>
 *       <td>\b delta_psi</td>
 *       <td>azimuthal peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b yaw</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b pitch</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b roll</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Psi (deg)</td>
 *       <td>azimuthal angle in detector plane in degree (Psi = 0: horizontal)</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>wavelength in reciprocal units of q</td>
 *      </tr><tr>
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_fcc_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_FCC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_fcc_aniso_gaussian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_fcc_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_FCC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_fcc_aniso_gaussian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_fcc_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_FCC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_fcc_aniso_gaussian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_fcc_aniso_gaussian ################ */


/* ################ start sq_bcc_aniso_lorentzian ################ */
/**
 * \defgroup sq_bcc_aniso_lorentzian BCC-aniso-Lorentzian
 * \ingroup sq_plugins_BCC
 *
 * \brief \<some brief description of BCC function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta_q</td>
 *       <td>radial peak width</td>
 *      </tr><tr>
 *       <td>\b delta_psi</td>
 *       <td>azimuthal peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b yaw</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b pitch</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b roll</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Psi (deg)</td>
 *       <td>azimuthal angle in detector plane in degree (Psi = 0: horizontal)</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>wavelength in reciprocal units of q</td>
 *      </tr><tr>
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_bcc_aniso_lorentzian
 *
 * \sa sasfit_ordered_particle_systepms.h, sq_plugins_BCC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_bcc_aniso_lorentzian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_bcc_aniso_lorentzian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_BCC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_bcc_aniso_lorentzian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_bcc_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_BCC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_bcc_aniso_lorentzian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_bcc_aniso_lorentzian ################ */

/* ################ start sq_bcc_aniso_gaussian ################ */
/**
 * \defgroup sq_bcc_aniso_gaussian  BCC_aniso-Gaussian
 * \ingroup sq_plugins_BCC
 *
 * \brief \<some brief description of BCC function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>lattice constant</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b sigma_a</td>
 *       <td>peak Gaussian lattice point distribution</td>
 *      </tr><tr>
 *       <td>\b delta_q</td>
 *       <td>radial peak width</td>
 *      </tr><tr>
 *       <td>\b delta_psi</td>
 *       <td>azimuthal peak width</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b yaw</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b pitch</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b roll</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Psi (deg)</td>
 *       <td>azimuthal angle in detector plane in degree (Psi = 0: horizontal)</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>wavelength in reciprocal units of q</td>
 *      </tr><tr>
 *       <td>\b max(hkl)</td>
 *       <td>maximum considered values for hkl</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_bcc_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_BCC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_bcc_aniso_gaussian(scalar q, sasfit_param * p);

/**
 * \ingroup sq_bcc_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_BCC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_bcc_aniso_gaussian_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_bcc_aniso_gaussian
 *
 * \sa sasfit_ordered_particle_systems.h, sq_plugins_BCC
 */
sasfit_ordered_particle_systems_DLLEXP scalar sasfit_sq_bcc_aniso_gaussian_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_bcc_aniso_gaussian ################ */

#endif // this file

