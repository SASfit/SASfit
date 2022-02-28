/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_JOHNSONDISTRIBUTION_H
#define SASFIT_PLUGIN_JOHNSONDISTRIBUTION_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_johnsondistribution.h
 * Public available functions and descriptions of the johnsondistribution plugin.
 */

/**
 * \def sasfit_johnsondistribution_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_johnsondistribution_EXPORTS)
	#ifdef sasfit_johnsondistribution_DLLEXP
	#undef sasfit_johnsondistribution_DLLEXP
	#endif
	#define sasfit_johnsondistribution_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_johnsondistribution_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_johnsondistribution_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sd_johnson_sn ################ */
/**
 * \defgroup sd_johnson_sn Johnson SN
 * \ingroup sd_plugins_quantile_unbounded
 *
 * \brief \<some brief description of Johnson SN function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b l</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b u</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>number, volume, intensity distribution (alpha:0,3,6)</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sd_johnson_sn
 *
 * \sa sasfit_johnsondistribution.h, sd_plugins_quantile_unbounded
 */
sasfit_johnsondistribution_DLLEXP scalar sasfit_sd_johnson_sn(scalar q, sasfit_param * p);

/**
 * \ingroup sd_johnson_sn
 *
 * \sa sasfit_johnsondistribution.h, sd_plugins_quantile_unbounded
 */
sasfit_johnsondistribution_DLLEXP scalar sasfit_sd_johnson_sn_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_johnson_sn
 *
 * \sa sasfit_johnsondistribution.h, sd_plugins_quantile_unbounded
 */
sasfit_johnsondistribution_DLLEXP scalar sasfit_sd_johnson_sn_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_johnson_sn ################ */

/* ################ start sd_johnson_sb ################ */
/**
 * \defgroup sd_johnson_sb Johnson SB
 * \ingroup sd_plugins_quantile_bounded
 *
 * \brief \< some brief description of Johnson SB function \>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>number, volume, intensity distribution (alpha:0,3,6)</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>lower bound: xi</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>upper bound: xi+lambda</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sd_johnson_sb
 *
 * \sa sasfit_johnsondistribution.h, sd_plugins_quantile_bounded
 */
sasfit_johnsondistribution_DLLEXP scalar sasfit_sd_johnson_sb(scalar x, sasfit_param * p);

/**
 * \ingroup sd_johnson_sb
 *
 * \sa sasfit_johnsondistribution.h, sd_plugins_quantile_bounded
 */
sasfit_johnsondistribution_DLLEXP scalar sasfit_sd_johnson_sb_f(scalar x, sasfit_param * p);

/**
 * \ingroup sd_johnson_sb
 *
 * \sa sasfit_johnsondistribution.h, sd_plugins_quantile_bounded
 */
sasfit_johnsondistribution_DLLEXP scalar sasfit_sd_johnson_sb_v(scalar y, sasfit_param * p, int dist);
/* ################ stop sd_johnson_sb ################ */

/* ################ start sd_johnson_su ################ */
/**
 * \defgroup sd_johnson_su Johnson SU
 * \ingroup sd_plugins_quantile_unbounded
 *
 * \brief \<some brief description of Johnson SU function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b l</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b u</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>number, volume, intensity distribution (alpha:0,3,6)</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sd_johnson_su
 *
 * \sa sasfit_johnsondistribution.h, sd_plugins_quantile_unbounded
 */
sasfit_johnsondistribution_DLLEXP scalar sasfit_sd_johnson_su(scalar q, sasfit_param * p);

/**
 * \ingroup sd_johnson_su
 *
 * \sa sasfit_johnsondistribution.h, sd_plugins_quantile_unbounded
 */
sasfit_johnsondistribution_DLLEXP scalar sasfit_sd_johnson_su_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_johnson_su
 *
 * \sa sasfit_johnsondistribution.h, sd_plugins_quantile_unbounded
 */
sasfit_johnsondistribution_DLLEXP scalar sasfit_sd_johnson_su_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_johnson_su ################ */

/* ################ start sd_johnson_sl ################ */
/**
 * \defgroup sd_johnson_sl Johnson SL
 * \ingroup sd_plugins_quantile_semibounded
 *
 * \brief \<some brief description of Johnson SL function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b l</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b u</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>number, volume, intensity distribution (alpha:0,3,6)</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sd_johnson_sl
 *
 * \sa sasfit_johnsondistribution.h, sd_plugins_quantile_semibounded
 */
sasfit_johnsondistribution_DLLEXP scalar sasfit_sd_johnson_sl(scalar q, sasfit_param * p);

/**
 * \ingroup sd_johnson_sl
 *
 * \sa sasfit_johnsondistribution.h, sd_plugins_quantile_semibounded
 */
sasfit_johnsondistribution_DLLEXP scalar sasfit_sd_johnson_sl_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_johnson_sl
 *
 * \sa sasfit_johnsondistribution.h, sd_plugins_quantile_semibounded
 */
sasfit_johnsondistribution_DLLEXP scalar sasfit_sd_johnson_sl_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_johnson_sl ################ */


#endif // this file

