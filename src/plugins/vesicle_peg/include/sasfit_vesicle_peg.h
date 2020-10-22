/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_VESICLE_PEG_H
#define SASFIT_PLUGIN_VESICLE_PEG_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_vesicle_peg.h
 * Public available functions and descriptions of the vesicle_peg plugin.
 */

/**
 * \def sasfit_vesicle_peg_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_vesicle_peg_EXPORTS)
	#ifdef sasfit_vesicle_peg_DLLEXP
	#undef sasfit_vesicle_peg_DLLEXP
	#endif
	#define sasfit_vesicle_peg_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_vesicle_peg_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_vesicle_peg_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_vesicle__pegulated_ ################ */
/** 
 * \defgroup ff_vesicle__pegulated_ Vesicle (pegulated)
 * \ingroup ff_devel
 *
 * \brief \<some brief description of Vesicle (pegulated) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b n_PEG</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b V_PEG</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta_PEG</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b V_head</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta_head</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b V_tail</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta_tail</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_vesicle__pegulated_
 *
 * \sa sasfit_vesicle_peg.h, ff_devel
 */
sasfit_vesicle_peg_DLLEXP scalar sasfit_ff_vesicle__pegulated_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_vesicle__pegulated_
 *
 * \sa sasfit_vesicle_peg.h, ff_devel
 */
sasfit_vesicle_peg_DLLEXP scalar sasfit_ff_vesicle__pegulated__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_vesicle__pegulated_
 *
 * \sa sasfit_vesicle_peg.h, ff_devel
 */
sasfit_vesicle_peg_DLLEXP scalar sasfit_ff_vesicle__pegulated__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_vesicle__pegulated_ ################ */


#endif // this file

