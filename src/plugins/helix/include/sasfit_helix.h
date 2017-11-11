/*
 * Author(s) of this file:
 *  Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *  10. Oct. 2017
 */

#ifndef SASFIT_PLUGIN_HELIX_H
#define SASFIT_PLUGIN_HELIX_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_helix.h
 * Public available functions and descriptions of the heliix plugin.
 */

/**
 * \def sasfit_helix_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_helix_EXPORTS)
	#ifdef sasfit_helix_DLLEXP
	#undef sasfit_helix_DLLEXP
	#endif
	#define sasfit_helix_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_helix_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_helix_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_fan_helix ################ */
/** 
 * \defgroup ff_fan_helix fanlike helix
 * \ingroup ff_plugins_helix
 *
 * \brief \<some brief description of fanlike helix function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>external helix radius</td>
 *      </tr><tr>
 *       <td>\b a</td>
 *       <td>0<a<1, aR : inner helix radius</td>
 *      </tr><tr>
 *       <td>\b omega</td>
 *       <td>angular of the sector of material</td>
 *      </tr><tr>
 *       <td>\b phi</td>
 *       <td>angle between the two sectors of a double helix</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b P</td>
 *       <td>helix period</td>
 *      </tr><tr>
 *       <td>\b H</td>
 *       <td>helix length</td>
 *      </tr><tr>
 *       <td>\b eta_h</td>
 *       <td>scattering length density of helix</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_fan_helix
 *
 * \sa sasfit_helix.h, ff_plugins_helix
 */
sasfit_helix_DLLEXP scalar sasfit_ff_fan_helix(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fan_helix
 *
 * \sa sasfit_helix.h, ff_plugins_helix
 */
sasfit_helix_DLLEXP scalar sasfit_ff_fan_helix_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fan_helix
 *
 * \sa sasfit_helix.h, ff_plugins_helix
 */
sasfit_helix_DLLEXP scalar sasfit_ff_fan_helix_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_fan_helix ################ */

/* ################ start ff_round_helix ################ */
/** 
 * \defgroup ff_round_helix  helix with round XS
 * \ingroup ff_plugins_helix
 *
 * \brief \<some brief description of "helix with round XS" function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b r1</td>
 *       <td>radius of first strand thickness</td>
 *      </tr><tr>
 *       <td>\b delta_1</td>
 *       <td>distance of first strand to center axis</td>
 *      </tr><tr>
 *       <td>\b r2</td>
 *       <td>radius of second strand thickness</td>
 *      </tr><tr>
 *       <td>\b delta_2</td>
 *       <td>distance of second strand to center axis</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>angle between the two strands</td>
 *      </tr><tr>
 *       <td>\b P</td>
 *       <td>helix period</td>
 *      </tr><tr>
 *       <td>\b H</td>
 *       <td>helix length</td>
 *      </tr><tr>
 *       <td>\b eta_1</td>
 *       <td>scattering length density of first strand</td>
 *      </tr><tr>
 *       <td>\b eta_2</td>
 *       <td>scattering length density of second strand</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_round_helix
 *
 * \sa sasfit_helix.h, ff_plugins_helix
 */
sasfit_helix_DLLEXP scalar sasfit_ff_round_helix(scalar q, sasfit_param * p);

/**
 * \ingroup ff_round_helix
 *
 * \sa sasfit_helix.h, ff_plugins_helix
 */
sasfit_helix_DLLEXP scalar sasfit_ff_round_helix_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_round_helix
 *
 * \sa sasfit_helix.h, ff_plugins_helix
 */
sasfit_helix_DLLEXP scalar sasfit_ff_round_helix_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_round_helix ################ */

/* ################ start ff_beads_helix ################ */
/** 
 * \defgroup ff_beads_helix beads helix
 * \ingroup ff_plugins_helix
 *
 * \brief \<some brief description of beads helix function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius of monomer units/beads</td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td>mean diameter of the heli</td>
 *      </tr><tr>
 *       <td>\b n</td>
 *       <td>number of monomers per turn</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b P</td>
 *       <td>helix period</td>
 *      </tr><tr>
 *       <td>\b H</td>
 *       <td>helix length</td>
 *      </tr><tr>
 *       <td>\b eta_b</td>
 *       <td>scattering length density of beads</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_beads_helix
 *
 * \sa sasfit_helix.h, ff_plugins_helix
 */
sasfit_helix_DLLEXP scalar sasfit_ff_beads_helix(scalar q, sasfit_param * p);

/**
 * \ingroup ff_beads_helix
 *
 * \sa sasfit_helix.h, ff_plugins_helix
 */
sasfit_helix_DLLEXP scalar sasfit_ff_beads_helix_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_beads_helix
 *
 * \sa sasfit_helix.h, ff_plugins_helix
 */
sasfit_helix_DLLEXP scalar sasfit_ff_beads_helix_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_beads_helix ################ */

/* ################ start ff_superhelices_straight ################ */
/** 
 * \defgroup ff_superhelices_straight straight superhelix
 * \ingroup ff_plugins_helix
 *
 * \brief \< some brief description of straight superhelix function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R1</td>
 *       <td>radius of monomer units/beads</td>
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
 *       <td>\b P</td>
 *       <td>helix period</td>
 *      </tr><tr>
 *       <td>\b H</td>
 *       <td>helix length</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_superhelices_straight
 *
 * \sa sasfit_helix.h, ff_plugins_helix
 */
sasfit_helix_DLLEXP scalar sasfit_ff_superhelices_straight(scalar q, sasfit_param * p);

/**
 * \ingroup ff_superhelices_straight
 *
 * \sa sasfit_helix.h, ff_plugins_helix
 */
sasfit_helix_DLLEXP scalar sasfit_ff_superhelices_straight_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_superhelices_straight
 *
 * \sa sasfit_helix.h, ff_plugins_helix
 */
sasfit_helix_DLLEXP scalar sasfit_ff_superhelices_straight_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_superhelices_straight ################ */

/* ################ start ff_superhelices_coiled ################ */
/** 
 * \defgroup ff_superhelices_coiled coiled superhelix
 * \ingroup ff_plugins_helix
 *
 * \brief \< some brief description of coiled superhelix function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R1</td>
 *       <td>radius primary turns</td>
 *      </tr><tr>
 *       <td>\b R2</td>
 *       <td>radius of secondary turns</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b N</td>
 *       <td>number of primary turns per secondary turn</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b P</td>
 *       <td>helix period</td>
 *      </tr><tr>
 *       <td>\b turns</td>
 *       <td>number of secondary turns</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_superhelices_coiled
 *
 * \sa sasfit_helix.h, ff_plugins_helix
 */
sasfit_helix_DLLEXP scalar sasfit_ff_superhelices_coiled(scalar q, sasfit_param * p);

/**
 * \ingroup ff_superhelices_coiled
 *
 * \sa sasfit_helix.h, ff_plugins_helix
 */
sasfit_helix_DLLEXP scalar sasfit_ff_superhelices_coiled_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_superhelices_coiled
 *
 * \sa sasfit_helix.h, ff_plugins_helix
 */
sasfit_helix_DLLEXP scalar sasfit_ff_superhelices_coiled_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_superhelices_coiled ################ */

#endif // this file

