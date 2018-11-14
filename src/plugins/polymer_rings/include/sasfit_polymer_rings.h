/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_POLYMER_RINGS_H
#define SASFIT_PLUGIN_POLYMER_RINGS_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_polymer_rings.h
 * Public available functions and descriptions of the polymer_rings plugin.
 */

/**
 * \def sasfit_polymer_rings_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_polymer_rings_EXPORTS)
	#ifdef sasfit_polymer_rings_DLLEXP
	#undef sasfit_polymer_rings_DLLEXP
	#endif
	#define sasfit_polymer_rings_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_polymer_rings_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_polymer_rings_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_ringpolymerCasassa ################ */
/** 
 * \defgroup ff_ringpolymerCasassa RingPolymerCasassa
 * \ingroup ff_polymer_rings
 *
 * \brief \<some brief description of RingPolymerCasassa function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering I(Q=0)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ringpolymerCasassa
 *
 * \sa sasfit_polymer_rings.h, ff_polymer_rings
 */
sasfit_polymer_rings_DLLEXP scalar sasfit_ff_ringpolymerCasassa(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ringpolymerCasassa
 *
 * \sa sasfit_polymer_rings.h, ff_polymer_rings
 */
sasfit_polymer_rings_DLLEXP scalar sasfit_ff_ringpolymerCasassa_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ringpolymerCasassa
 *
 * \sa sasfit_polymer_rings.h, ff_polymer_rings
 */
sasfit_polymer_rings_DLLEXP scalar sasfit_ff_ringpolymerCasassa_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ringpolymerCasassa ################ */

/* ################ start ff_ringpolymerBZ ################ */
/** 
 * \defgroup ff_ringpolymerBZ RingPolymerBZ
 * \ingroup ff_polymer_rings
 *
 * \brief \<some brief description of RingPolymerBZ function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b nu = (1+epsilon)/2</td>
 *       <td>excluded volume parameter</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering I(Q=0)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ringpolymerBZ
 *
 * \sa sasfit_polymer_rings.h, ff_polymer_rings
 */
sasfit_polymer_rings_DLLEXP scalar sasfit_ff_ringpolymerBZ(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ringpolymerBZ
 *
 * \sa sasfit_polymer_rings.h, ff_polymer_rings
 */
sasfit_polymer_rings_DLLEXP scalar sasfit_ff_ringpolymerBZ_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ringpolymerBZ
 *
 * \sa sasfit_polymer_rings.h, ff_polymer_rings
 */
sasfit_polymer_rings_DLLEXP scalar sasfit_ff_ringpolymerBZ_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ringpolymerBZ ################ */

/* ################ start ff_ringpolymerBMB ################ */
/** 
 * \defgroup ff_ringpolymerBMB RingPolymerBMB
 * \ingroup ff_polymer_rings
 *
 * \brief \<some brief description of RingPolymerBMB function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b nu = (1+epsilon)/2</td>
 *       <td>excluded volume parameter</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering I(Q=0)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ringpolymerBMB
 *
 * \sa sasfit_polymer_rings.h, ff_polymer_rings
 */
sasfit_polymer_rings_DLLEXP scalar sasfit_ff_ringpolymerBMB(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ringpolymerBMB
 *
 * \sa sasfit_polymer_rings.h, ff_polymer_rings
 */
sasfit_polymer_rings_DLLEXP scalar sasfit_ff_ringpolymerBMB_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ringpolymerBMB
 *
 * \sa sasfit_polymer_rings.h, ff_polymer_rings
 */
sasfit_polymer_rings_DLLEXP scalar sasfit_ff_ringpolymerBMB_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ringpolymerBMB ################ */

/* ################ start ff_mmemberedtwistedring ################ */
/** 
 * \defgroup ff_mmemberedtwistedring mMemberedTwistedRing
 * \ingroup ff_polymer_rings
 *
 * \brief \<some brief description of mMemberedTwistedRing function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg,1r</td>
 *       <td>radius of gyration of one loop</td>
 *      </tr><tr>
 *       <td>\b m</td>
 *       <td>number of twists</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering I(Q=0)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_mmemberedtwistedring
 *
 * \sa sasfit_polymer_rings.h, ff_polymer_rings
 */
sasfit_polymer_rings_DLLEXP scalar sasfit_ff_mmemberedtwistedring(scalar q, sasfit_param * p);

/**
 * \ingroup ff_mmemberedtwistedring
 *
 * \sa sasfit_polymer_rings.h, ff_polymer_rings
 */
sasfit_polymer_rings_DLLEXP scalar sasfit_ff_mmemberedtwistedring_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_mmemberedtwistedring
 *
 * \sa sasfit_polymer_rings.h, ff_polymer_rings
 */
sasfit_polymer_rings_DLLEXP scalar sasfit_ff_mmemberedtwistedring_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_mmemberedtwistedring ################ */

/* ################ start ff_daisylikering ################ */
/** 
 * \defgroup ff_daisylikering DaisyLikeRing
 * \ingroup ff_polymer_rings
 *
 * \brief \<some brief description of DaisyLikeRing function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg,1r</td>
 *       <td>radius of gyration of one ring</td>
 *      </tr><tr>
 *       <td>\b m</td>
 *       <td>number of rings</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering I(Q=0)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_daisylikering
 *
 * \sa sasfit_polymer_rings.h, ff_polymer_rings
 */
sasfit_polymer_rings_DLLEXP scalar sasfit_ff_daisylikering(scalar q, sasfit_param * p);

/**
 * \ingroup ff_daisylikering
 *
 * \sa sasfit_polymer_rings.h, ff_polymer_rings
 */
sasfit_polymer_rings_DLLEXP scalar sasfit_ff_daisylikering_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_daisylikering
 *
 * \sa sasfit_polymer_rings.h, ff_polymer_rings
 */
sasfit_polymer_rings_DLLEXP scalar sasfit_ff_daisylikering_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_daisylikering ################ */


#endif // this file

