/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_DAB_1D_2D_H
#define SASFIT_PLUGIN_DAB_1D_2D_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_dab_1d_2d.h
 * Public available functions and descriptions of the dab_1d_2d plugin.
 */

/**
 * \def sasfit_dab_1d_2d_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_dab_1d_2d_EXPORTS)
	#ifdef sasfit_dab_1d_2d_DLLEXP
	#undef sasfit_dab_1d_2d_DLLEXP
	#endif
	#define sasfit_dab_1d_2d_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_dab_1d_2d_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_dab_1d_2d_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_dab_2d ################ */
/**
 * \defgroup ff_dab_2d DAB_2D
 * \ingroup ff_plugins_pcslocalcylindrical
 *
 * \brief \<some brief description of DAB_2D function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b xi</td>
 *       <td>correlation length</td>
 *      </tr><tr>
 *       <td>\b H</td>
 *       <td>Hurst exponent</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_dab_2d
 *
 * \sa sasfit_dab_1d_2d.h, ff_plugins_pcslocalcylindrical
 */
sasfit_dab_1d_2d_DLLEXP scalar sasfit_ff_dab_2d(scalar q, sasfit_param * p);

/**
 * \ingroup ff_dab_2d
 *
 * \sa sasfit_dab_1d_2d.h, ff_plugins_pcslocalcylindrical
 */
sasfit_dab_1d_2d_DLLEXP scalar sasfit_ff_dab_2d_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_dab_2d
 *
 * \sa sasfit_dab_1d_2d.h, ff_plugins_pcslocalcylindrical
 */
sasfit_dab_1d_2d_DLLEXP scalar sasfit_ff_dab_2d_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_dab_2d ################ */

/* ################ start ff_dab_1d ################ */
/**
 * \defgroup ff_dab_1d DAB_1D
 * \ingroup ff_plugins_pcslocalplanar
 *
 * \brief \<some brief description of DAB_1D function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b xi</td>
 *       <td>correlation length</td>
 *      </tr><tr>
 *       <td>\b H</td>
 *       <td>Hurst exponent</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_dab_1d
 *
 * \sa sasfit_dab_1d_2d.h, ff_plugins_pcslocalplanar
 */
sasfit_dab_1d_2d_DLLEXP scalar sasfit_ff_dab_1d(scalar q, sasfit_param * p);

/**
 * \ingroup ff_dab_1d
 *
 * \sa sasfit_dab_1d_2d.h, ff_plugins_pcslocalplanar
 */
sasfit_dab_1d_2d_DLLEXP scalar sasfit_ff_dab_1d_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_dab_1d
 *
 * \sa sasfit_dab_1d_2d.h, ff_plugins_pcslocalplanar
 */
sasfit_dab_1d_2d_DLLEXP scalar sasfit_ff_dab_1d_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_dab_1d ################ */


#endif // this file

