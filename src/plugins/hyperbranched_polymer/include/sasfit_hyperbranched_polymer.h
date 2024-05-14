/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_HYPERBRANCHED_POLYMER_H
#define SASFIT_PLUGIN_HYPERBRANCHED_POLYMER_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_hyperbranched_polymer.h
 * Public available functions and descriptions of the hyperbranched_polymer plugin.
 */

/**
 * \def sasfit_hyperbranched_polymer_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_hyperbranched_polymer_EXPORTS)
	#ifdef sasfit_hyperbranched_polymer_DLLEXP
	#undef sasfit_hyperbranched_polymer_DLLEXP
	#endif
	#define sasfit_hyperbranched_polymer_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_hyperbranched_polymer_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_hyperbranched_polymer_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_fract_hyperbranched_polym ################ */
/**
 * \defgroup ff_fract_hyperbranched_polym fract hyperbranched polym
 * \ingroup ff_polymer_branched
 *
 * \brief \<some brief description of fract hyperbranched polym function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>fractal dimension</td>
 *      </tr><tr>
 *       <td>\b C</td>
 *       <td> 1/C is the number of branching points per macromolecule</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_fract_hyperbranched_polym
 *
 * \sa sasfit_hyperbranched_polymer.h, ff_polymer_branched
 */
sasfit_hyperbranched_polymer_DLLEXP scalar sasfit_ff_fract_hyperbranched_polym(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fract_hyperbranched_polym
 *
 * \sa sasfit_hyperbranched_polymer.h, ff_polymer_branched
 */
sasfit_hyperbranched_polymer_DLLEXP scalar sasfit_ff_fract_hyperbranched_polym_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fract_hyperbranched_polym
 *
 * \sa sasfit_hyperbranched_polymer.h, ff_polymer_branched
 */
sasfit_hyperbranched_polymer_DLLEXP scalar sasfit_ff_fract_hyperbranched_polym_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_fract_hyperbranched_polym ################ */


/* ################ start ff_f_funct_homo_polym ################ */
/**
 * \defgroup ff_f_funct_homo_polym f-funct. homopolymer
 * \ingroup ff_polymer_branched
 *
 * \brief \<some brief description of f-functional homopolymer\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b sigma</td>
 *       <td>sigma^2: mean-square distance between two units directly connected by a link</td>
 *      </tr><tr>
 *       <td>\b f</td>
 *       <td>functionality</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td> fraction of functionalities which have reacted</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr><tr>
 *       <td>\b type</td>
 *       <td>type: 0 Zimm approx, 1:includes local structure at large q, 2: 3: </td>
 *      </tr></table>
 */

/**
 * \ingroup ff_f_funct_homo_polym
 *
 * \sa sasfit_hyperbranched_polymer.h, ff_polymer_branched
 */
sasfit_hyperbranched_polymer_DLLEXP scalar sasfit_ff_f_funct_homo_polym(scalar q, sasfit_param * p);

/**
 * \ingroup ff_f_funct_homo_polym
 *
 * \sa sasfit_hyperbranched_polymer.h, ff_polymer_branched
 */
sasfit_hyperbranched_polymer_DLLEXP scalar sasfit_ff_f_funct_homo_polym_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_f_funct_homo_polym
 *
 * \sa sasfit_hyperbranched_polymer.h, ff_polymer_branched
 */
sasfit_hyperbranched_polymer_DLLEXP scalar sasfit_ff_f_funct_homo_polym_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_f_funct_homo_polym ################ */


/* ################ start ff_comb_regular ################ */
/**
 * \defgroup ff_comb_regular comb (regular)
 * \ingroup ff_polymer_branched
 *
 * \brief \<some brief description of a regular comb \>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration</td>
 *      </tr><tr>
 *       <td>\b zeta</td>
 *       <td>functionality</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td> density of side chains</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_comb_regular
 *
 * \sa sasfit_hyperbranched_polymer.h, ff_polymer_branched
 */
sasfit_hyperbranched_polymer_DLLEXP scalar sasfit_ff_comb_regular(scalar q, sasfit_param * p);

/**
 * \ingroup ff_comb_regular
 *
 * \sa sasfit_hyperbranched_polymer.h, ff_polymer_branched
 */
sasfit_hyperbranched_polymer_DLLEXP scalar sasfit_ff_comb_regular_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_comb_regular
 *
 * \sa sasfit_hyperbranched_polymer.h, ff_polymer_branched
 */
sasfit_hyperbranched_polymer_DLLEXP scalar sasfit_ff_comb_regular_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_comb_regular ################ */


/* ################ start ff_comb_random ################ */
/**
 * \defgroup ff_comb_random comb (random)
 * \ingroup ff_polymer_branched
 *
 * \brief \<some brief description of a random comb \>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>radius of gyration of a linear chain with the same number of polymer units</td>
 *      </tr><tr>
 *       <td>\b zeta</td>
 *       <td>functionality</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td> density of side chains</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_comb_random
 *
 * \sa sasfit_hyperbranched_polymer.h, ff_polymer_branched
 */
sasfit_hyperbranched_polymer_DLLEXP scalar sasfit_ff_comb_random(scalar q, sasfit_param * p);

/**
 * \ingroup ff_comb_random
 *
 * \sa sasfit_hyperbranched_polymer.h, ff_polymer_branched
 */
sasfit_hyperbranched_polymer_DLLEXP scalar sasfit_ff_comb_random_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_comb_random
 *
 * \sa sasfit_hyperbranched_polymer.h, ff_polymer_branched
 */
sasfit_hyperbranched_polymer_DLLEXP scalar sasfit_ff_comb_random_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_comb_random ################ */


/* ################ start ff_comb_hetero ################ */
/**
 * \defgroup ff_comb_hetero comb (heterogeneous)
 * \ingroup ff_polymer_branched
 *
 * \brief \<some brief description of a heterogeneous comb \>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>radius of gyration of a linear chain with the same number of polymer units</td>
 *      </tr><tr>
 *       <td>\b zeta</td>
 *       <td>functionality</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td> density of side chains</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_comb_hetero
 *
 * \sa sasfit_hyperbranched_polymer.h, ff_polymer_branched
 */
sasfit_hyperbranched_polymer_DLLEXP scalar sasfit_ff_comb_hetero(scalar q, sasfit_param * p);

/**
 * \ingroup ff_comb_hetero
 *
 * \sa sasfit_hyperbranched_polymer.h, ff_polymer_branched
 */
sasfit_hyperbranched_polymer_DLLEXP scalar sasfit_ff_comb_hetero_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_comb_hetero
 *
 * \sa sasfit_hyperbranched_polymer.h, ff_polymer_branched
 */
sasfit_hyperbranched_polymer_DLLEXP scalar sasfit_ff_comb_hetero_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_comb_hetero ################ */


/* ################ start ff_ideal_dendrimer ################ */
/**
 * \defgroup ff_ideal_dendrimer ideal dendrimer
 * \ingroup ff_polymer_branched
 *
 * \brief \<some brief description of an ideal_dendrimer of generation g \>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
 *       <td>radius of gyration of a linear chain with the same number of polymer units</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b g</td>
 *       <td> generation of tri-dendrimer</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ideal_dendrimer
 *
 * \sa sasfit_hyperbranched_polymer.h, ff_polymer_branched
 */
sasfit_hyperbranched_polymer_DLLEXP scalar sasfit_ff_ideal_dendrimer(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ideal_dendrimer
 *
 * \sa sasfit_hyperbranched_polymer.h, ff_polymer_branched
 */
sasfit_hyperbranched_polymer_DLLEXP scalar sasfit_ff_ideal_dendrimer_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ideal_dendrimer
 *
 * \sa sasfit_hyperbranched_polymer.h, ff_polymer_branched
 */
sasfit_hyperbranched_polymer_DLLEXP scalar sasfit_ff_ideal_dendrimer_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ideal_dendrimer ################ */

#endif // this file

