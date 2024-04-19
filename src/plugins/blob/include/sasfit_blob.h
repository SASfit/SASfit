/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_BLOB_H
#define SASFIT_PLUGIN_BLOB_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_blob.h
 * Public available functions and descriptions of the blob plugin.
 */

/**
 * \def sasfit_blob_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_blob_EXPORTS)
	#ifdef sasfit_blob_DLLEXP
	#undef sasfit_blob_DLLEXP
	#endif
	#define sasfit_blob_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_blob_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_blob_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_blob__star_ ################ */
/**
 * \defgroup ff_blob__star_ blob (star)
 * \ingroup ff_non-particulate
 *
 * \brief \<some brief description of blob (star) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td>radius of guration</td>
 *      </tr><tr>
 *       <td>\b nu</td>
 *       <td>Flory-Huggins parameter (1/3<nu<1)</td>
 *      </tr><tr>
 *       <td>\b f</td>
 *       <td>number of arms</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>scaling parameter</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_blob__star_
 *
 * \sa sasfit_blob.h, ff_non-particulate
 */
sasfit_blob_DLLEXP scalar sasfit_ff_blob__star_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_blob__star_
 *
 * \sa sasfit_blob.h, ff_non-particulate
 */
sasfit_blob_DLLEXP scalar sasfit_ff_blob__star__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_blob__star_
 *
 * \sa sasfit_blob.h, ff_non-particulate
 */
sasfit_blob_DLLEXP scalar sasfit_ff_blob__star__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_blob__star_ ################ */

/* ################ start ff_blob__dendrimer_ ################ */
/**
 * \defgroup ff_blob__dendrimer_ blob (dendrimer)
 * \ingroup ff_non-particulate
 *
 * \brief \<some brief description of blob (dendrimer) function\>
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
 *       <td>\b nu</td>
 *       <td>Flory–Huggins parameter (1/3<nu<1)</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>effective blob radius</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>scaling parameter</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_blob__dendrimer_
 *
 * \sa sasfit_blob.h, ff_non-particulate
 */
sasfit_blob_DLLEXP scalar sasfit_ff_blob__dendrimer_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_blob__dendrimer_
 *
 * \sa sasfit_blob.h, ff_non-particulate
 */
sasfit_blob_DLLEXP scalar sasfit_ff_blob__dendrimer__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_blob__dendrimer_
 *
 * \sa sasfit_blob.h, ff_non-particulate
 */
sasfit_blob_DLLEXP scalar sasfit_ff_blob__dendrimer__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_blob__dendrimer_ ################ */


#endif // this file

