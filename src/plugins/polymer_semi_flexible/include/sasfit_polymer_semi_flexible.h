/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_POLYMER_SEMI_FLEXIBLE_H
#define SASFIT_PLUGIN_POLYMER_SEMI_FLEXIBLE_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_polymer_semi_flexible.h
 * Public available functions and descriptions of the polymer_semi_flexible plugin.
 */

/**
 * \def sasfit_polymer_semi_flexible_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_polymer_semi_flexible_EXPORTS)
	#ifdef sasfit_polymer_semi_flexible_DLLEXP
	#undef sasfit_polymer_semi_flexible_DLLEXP
	#endif
	#define sasfit_polymer_semi_flexible_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_polymer_semi_flexible_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_polymer_semi_flexible_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_worms_ps1_ ################ */
/**
 * \defgroup ff_worms_ps1_ Worms(PS1)
 * \ingroup ff_polymer_worm
 *
 * \brief \<some brief description of Worms(PS1) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
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
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_worms_ps1_
 *
 * \sa sasfit_polymer_semi_flexible.h, ff_polymer_worm
 */
sasfit_polymer_semi_flexible_DLLEXP scalar sasfit_ff_worms_ps1_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_worms_ps1_
 *
 * \sa sasfit_polymer_semi_flexible.h, ff_polymer_worm
 */
sasfit_polymer_semi_flexible_DLLEXP scalar sasfit_ff_worms_ps1__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_worms_ps1_
 *
 * \sa sasfit_polymer_semi_flexible.h, ff_polymer_worm
 */
sasfit_polymer_semi_flexible_DLLEXP scalar sasfit_ff_worms_ps1__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_worms_ps1_ ################ */

/* ################ start ff_worms_ps2_ ################ */
/**
 * \defgroup ff_worms_ps2_ Worms(PS2)
 * \ingroup ff_polymer_worm
 *
 * \brief \<some brief description of Worms(PS2) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b lb</td>
 *       <td>Kuhn length</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>contour length</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_worms_ps2_
 *
 * \sa sasfit_polymer_semi_flexible.h, ff_polymer_worm
 */
sasfit_polymer_semi_flexible_DLLEXP scalar sasfit_ff_worms_ps2_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_worms_ps2_
 *
 * \sa sasfit_polymer_semi_flexible.h, ff_polymer_worm
 */
sasfit_polymer_semi_flexible_DLLEXP scalar sasfit_ff_worms_ps2__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_worms_ps2_
 *
 * \sa sasfit_polymer_semi_flexible.h, ff_polymer_worm
 */
sasfit_polymer_semi_flexible_DLLEXP scalar sasfit_ff_worms_ps2__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_worms_ps2_ ################ */

/* ################ start ff_worms_ps3_ ################ */
/**
 * \defgroup ff_worms_ps3_ Worms(PS3)
 * \ingroup ff_polymer_worm
 *
 * \brief \<some brief description of Worms(PS3) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
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
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_worms_ps3_
 *
 * \sa sasfit_polymer_semi_flexible.h, ff_polymer_worm
 */
sasfit_polymer_semi_flexible_DLLEXP scalar sasfit_ff_worms_ps3_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_worms_ps3_
 *
 * \sa sasfit_polymer_semi_flexible.h, ff_polymer_worm
 */
sasfit_polymer_semi_flexible_DLLEXP scalar sasfit_ff_worms_ps3__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_worms_ps3_
 *
 * \sa sasfit_polymer_semi_flexible.h, ff_polymer_worm
 */
sasfit_polymer_semi_flexible_DLLEXP scalar sasfit_ff_worms_ps3__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_worms_ps3_ ################ */

/* ################ start ff_kholodenko_worm ################ */
/**
 * \defgroup ff_kholodenko_worm Kholodenko worm
 * \ingroup ff_polymer_worm
 *
 * \brief \<some brief description of Kholodenko worm function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b lb</td>
 *       <td>Kuhn length</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>contour length</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_kholodenko_worm
 *
 * \sa sasfit_polymer_semi_flexible.h, ff_polymer_worm
 */
sasfit_polymer_semi_flexible_DLLEXP scalar sasfit_ff_kholodenko_worm(scalar q, sasfit_param * p);

/**
 * \ingroup ff_kholodenko_worm
 *
 * \sa sasfit_polymer_semi_flexible.h, ff_polymer_worm
 */
sasfit_polymer_semi_flexible_DLLEXP scalar sasfit_ff_kholodenko_worm_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_kholodenko_worm
 *
 * \sa sasfit_polymer_semi_flexible.h, ff_polymer_worm
 */
sasfit_polymer_semi_flexible_DLLEXP scalar sasfit_ff_kholodenko_worm_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_kholodenko_worm ################ */

/* ################ start ff_freely_joined_chain_of_rods ################ */
/**
 * \defgroup ff_freely_joined_chain_of_rods freely joined chain of rods
 * \ingroup ff_polymer_worm
 *
 * \brief \<some brief description of freely joined chain of rods function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b lb</td>
 *       <td>Kuhn length</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>contour length</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_freely_joined_chain_of_rods
 *
 * \sa sasfit_polymer_semi_flexible.h, ff_polymer_worm
 */
sasfit_polymer_semi_flexible_DLLEXP scalar sasfit_ff_freely_joined_chain_of_rods(scalar q, sasfit_param * p);

/**
 * \ingroup ff_freely_joined_chain_of_rods
 *
 * \sa sasfit_polymer_semi_flexible.h, ff_polymer_worm
 */
sasfit_polymer_semi_flexible_DLLEXP scalar sasfit_ff_freely_joined_chain_of_rods_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_freely_joined_chain_of_rods
 *
 * \sa sasfit_polymer_semi_flexible.h, ff_polymer_worm
 */
sasfit_polymer_semi_flexible_DLLEXP scalar sasfit_ff_freely_joined_chain_of_rods_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_freely_joined_chain_of_rods ################ */

/* ################ start ff_koyama_worm ################ */
/**
 * \defgroup ff_koyama_worm Koyama worm
 * \ingroup ff_polymer_worm
 *
 * \brief \<some brief description of Koyama worm function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b lb</td>
 *       <td>Kuhn Length</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>contour length</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_koyama_worm
 *
 * \sa sasfit_polymer_semi_flexible.h, ff_polymer_worm
 */
sasfit_polymer_semi_flexible_DLLEXP scalar sasfit_ff_koyama_worm(scalar q, sasfit_param * p);

/**
 * \ingroup ff_koyama_worm
 *
 * \sa sasfit_polymer_semi_flexible.h, ff_polymer_worm
 */
sasfit_polymer_semi_flexible_DLLEXP scalar sasfit_ff_koyama_worm_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_koyama_worm
 *
 * \sa sasfit_polymer_semi_flexible.h, ff_polymer_worm
 */
sasfit_polymer_semi_flexible_DLLEXP scalar sasfit_ff_koyama_worm_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_koyama_worm ################ */


#endif // this file

