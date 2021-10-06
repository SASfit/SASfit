/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_METALOG_H
#define SASFIT_PLUGIN_METALOG_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_metalog.h
 * Public available functions and descriptions of the metalog plugin.
 */

/**
 * \def sasfit_metalog_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_metalog_EXPORTS)
	#ifdef sasfit_metalog_DLLEXP
	#undef sasfit_metalog_DLLEXP
	#endif
	#define sasfit_metalog_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_metalog_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_metalog_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_metaloglog_sphere ################ */
/**
 * \defgroup ff_metaloglog_sphere metalogLog Sphere
 * \ingroup ff_plugins
 *
 * \brief \<some brief description of metalogLog Sphere function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b bl</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>number, volume, intensity distribution (alpha:0,3,6)</td>
 *      </tr><tr>
 *       <td>\b a1</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a5</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a6</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a7</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a8</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a9</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a10</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_metaloglog_sphere
 *
 * \sa sasfit_metalog.h, sd_plugins
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metaloglog_sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metaloglog_sphere
 *
 * \sa sasfit_metalog.h, sd_plugins
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metaloglog_sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metaloglog_sphere
 *
 * \sa sasfit_metalog.h, sd_plugins
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metaloglog_sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_metaloglog_sphere ################ */


/* ################ start ff_metaloglogit_sphere ################ */
/**
 * \defgroup ff_metaloglogit_sphere metalogLogit Sphere
 * \ingroup ff_plugins
 *
 * \brief \<some brief description of metalogLogit Sphere function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b bl</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>number, volume, intensity distribution (alpha:0,3,6)</td>
 *      </tr><tr>
 *       <td>\b a1</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a5</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a6</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a7</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a8</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a9</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a10</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_metaloglogit_sphere
 *
 * \sa sasfit_metalog.h, sd_plugins
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metaloglogit_sphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metaloglogit_sphere
 *
 * \sa sasfit_metalog.h, sd_plugins
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metaloglogit_sphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_metaloglogit_sphere
 *
 * \sa sasfit_metalog.h, sd_plugins
 */
sasfit_metalog_DLLEXP scalar sasfit_ff_metaloglogit_sphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_metaloglogit_sphere ################ */

/* ################ start sd_metalog_0_inf ################ */
/**
 * \defgroup sd_metalog_0_inf metalog_0_inf
 * \ingroup sd_plugins
 *
 * \brief \<some brief description of metalog_0_inf function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b bl</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>number, volume, intensity distribution (alpha:0,3,6)</td>
 *      </tr><tr>
 *       <td>\b a1</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a5</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a6</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a7</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a8</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a9</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a10</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sd_metalog_0_inf
 *
 * \sa sasfit_metalog.h, sd_plugins
 */
sasfit_metalog_DLLEXP scalar sasfit_sd_metalog_0_inf(scalar q, sasfit_param * p);

/**
 * \ingroup sd_metalog_0_inf
 *
 * \sa sasfit_metalog.h, sd_plugins
 */
sasfit_metalog_DLLEXP scalar sasfit_sd_metalog_0_inf_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_metalog_0_inf
 *
 * \sa sasfit_metalog.h, sd_plugins
 */
sasfit_metalog_DLLEXP scalar sasfit_sd_metalog_0_inf_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_metalog_0_inf ################ */

/* ################ start sd_metalog_0_bu ################ */
/**
 * \defgroup sd_metalog_0_bu metalog_0_bu
 * \ingroup sd_plugins
 *
 * \brief \<some brief description of metalog_0_bu function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>scaling factor</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>number, volume, intensity distribution (alpha:0,3,6)</td>
 *      </tr><tr>
 *       <td>\b a1</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a5</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a6</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a7</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a8</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a9</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a10</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sd_metalog_0_bu
 *
 * \sa sasfit_metalog.h, sd_plugins
 */
sasfit_metalog_DLLEXP scalar sasfit_sd_metalog_0_bu(scalar q, sasfit_param * p);

/**
 * \ingroup sd_metalog_0_bu
 *
 * \sa sasfit_metalog.h, sd_plugins
 */
sasfit_metalog_DLLEXP scalar sasfit_sd_metalog_0_bu_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_metalog_0_bu
 *
 * \sa sasfit_metalog.h, sd_plugins
 */
sasfit_metalog_DLLEXP scalar sasfit_sd_metalog_0_bu_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_metalog_0_bu ################ */

/* ################ start sd_metalog_bl_bu ################ */
/**
 * \defgroup sd_metalog_bl_bu metalog_bl_bu
 * \ingroup sd_plugins
 *
 * \brief \<some brief description of metalog_bl_bu function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b bl</td>
 *       <td>lower bound</td>
 *      </tr><tr>
 *       <td>\b bu</td>
 *       <td>upper bound</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>number, volume, intensity distribution (alpha:0,3,6)</td>
 *      </tr><tr>
 *       <td>\b a1</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a3</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a5</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a6</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a7</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a8</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a9</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b a10</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup sd_metalog_bl_bu
 *
 * \sa sasfit_metalog.h, sd_plugins
 */
sasfit_metalog_DLLEXP scalar sasfit_sd_metalog_bl_bu(scalar q, sasfit_param * p);

/**
 * \ingroup sd_metalog_bl_bu
 *
 * \sa sasfit_metalog.h, sd_plugins
 */
sasfit_metalog_DLLEXP scalar sasfit_sd_metalog_bl_bu_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_metalog_bl_bu
 *
 * \sa sasfit_metalog.h, sd_plugins
 */
sasfit_metalog_DLLEXP scalar sasfit_sd_metalog_bl_bu_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_metalog_bl_bu ################ */


#endif // this file

