/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_T_LINDLEY_Y_H
#define SASFIT_PLUGIN_T_LINDLEY_Y_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_t_lindley_y.h
 * Public available functions and descriptions of the t_lindley_y plugin.
 */

/**
 * \def sasfit_t_lindley_y_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_t_lindley_y_EXPORTS)
	#ifdef sasfit_t_lindley_y_DLLEXP
	#undef sasfit_t_lindley_y_DLLEXP
	#endif
	#define sasfit_t_lindley_y_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_t_lindley_y_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_t_lindley_y_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sd_w_l_e_ ################ */
/**
 * \defgroup sd_w_l_e_ W-L{E}
 * \ingroup sd_plugins_try
 *
 * \brief \<some brief description of W-L{E} function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sd_w_l_e_
 *
 * \sa sasfit_t_lindley_y.h, sd_plugins_try
 */
sasfit_t_lindley_y_DLLEXP scalar sasfit_sd_w_l_e_(scalar q, sasfit_param * p);

/**
 * \ingroup sd_w_l_e_
 *
 * \sa sasfit_t_lindley_y.h, sd_plugins_try
 */
sasfit_t_lindley_y_DLLEXP scalar sasfit_sd_w_l_e__f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_w_l_e_
 *
 * \sa sasfit_t_lindley_y.h, sd_plugins_try
 */
sasfit_t_lindley_y_DLLEXP scalar sasfit_sd_w_l_e__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_w_l_e_ ################ */

/* ################ start sd_n_l_c_ ################ */
/**
 * \defgroup sd_n_l_c_ N-L{C}
 * \ingroup sd_plugins_try
 *
 * \brief \<some brief description of N-L{C} function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sd_n_l_c_
 *
 * \sa sasfit_t_lindley_y.h, sd_plugins_try
 */
sasfit_t_lindley_y_DLLEXP scalar sasfit_sd_n_l_c_(scalar q, sasfit_param * p);

/**
 * \ingroup sd_n_l_c_
 *
 * \sa sasfit_t_lindley_y.h, sd_plugins_try
 */
sasfit_t_lindley_y_DLLEXP scalar sasfit_sd_n_l_c__f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_n_l_c_
 *
 * \sa sasfit_t_lindley_y.h, sd_plugins_try
 */
sasfit_t_lindley_y_DLLEXP scalar sasfit_sd_n_l_c__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_n_l_c_ ################ */

/* ################ start sd_e_l_ll_ ################ */
/**
 * \defgroup sd_e_l_ll_ E-L{LL}
 * \ingroup sd_plugins_try
 *
 * \brief \<some brief description of E-L{LL} function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b theta</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b beta</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sd_e_l_ll_
 *
 * \sa sasfit_t_lindley_y.h, sd_plugins_try
 */
sasfit_t_lindley_y_DLLEXP scalar sasfit_sd_e_l_ll_(scalar q, sasfit_param * p);

/**
 * \ingroup sd_e_l_ll_
 *
 * \sa sasfit_t_lindley_y.h, sd_plugins_try
 */
sasfit_t_lindley_y_DLLEXP scalar sasfit_sd_e_l_ll__f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_e_l_ll_
 *
 * \sa sasfit_t_lindley_y.h, sd_plugins_try
 */
sasfit_t_lindley_y_DLLEXP scalar sasfit_sd_e_l_ll__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_e_l_ll_ ################ */


#endif // this file

