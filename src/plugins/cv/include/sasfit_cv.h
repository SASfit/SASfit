/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_CV_H
#define SASFIT_PLUGIN_CV_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_cv.h
 * Public available functions and descriptions of the cv plugin.
 */

/**
 * \def sasfit_cv_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_cv_EXPORTS)
	#ifdef sasfit_cv_DLLEXP
	#undef sasfit_cv_DLLEXP
	#endif
	#define sasfit_cv_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_cv_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_cv_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_cv ################ */
/** 
 * \defgroup ff_cv cv
 * \ingroup ff_sans
 *
 * \brief \< calculates matchpoint in a CV experiment by sqrt(I(q=0))=m*(x-x0) \>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b m</td>
 *       <td>slope</td>
 *      </tr><tr>
 *       <td>\b x0</td>
 *       <td>match point</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_cv
 *
 * \sa sasfit_cv.h, ff_sans
 */
sasfit_cv_DLLEXP scalar sasfit_ff_cv(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cv
 *
 * \sa sasfit_cv.h, ff_sans
 */
sasfit_cv_DLLEXP scalar sasfit_ff_cv_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cv
 *
 * \sa sasfit_cv.h, ff_sans
 */
sasfit_cv_DLLEXP scalar sasfit_ff_cv_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_cv ################ */


#endif // this file

