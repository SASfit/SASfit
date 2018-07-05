/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_HARD_SPHERES_H
#define SASFIT_PLUGIN_HARD_SPHERES_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_hard_spheres.h
 * Public available functions and descriptions of the hard_spheres plugin.
 */

/**
 * \def sasfit_hard_spheres_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_hard_spheres_EXPORTS)
	#ifdef sasfit_hard_spheres_DLLEXP
	#undef sasfit_hard_spheres_DLLEXP
	#endif
	#define sasfit_hard_spheres_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_hard_spheres_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_hard_spheres_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq_hard_sphere__lhr_ ################ */
/**
 * \defgroup sq_hard_sphere__lhr_ Hard Sphere (LHR)
 * \ingroup sq_plugins_hard_sphere
 *
 * \brief \<some brief description of Hard Sphere (LHR) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>volume fraction</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_hard_sphere__lhr_
 *
 * \sa sasfit_hard_spheres.h, sq_plugins_hard_sphere
 */
sasfit_hard_spheres_DLLEXP scalar sasfit_sq_hard_sphere__lhr_(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hard_sphere__lhr_
 *
 * \sa sasfit_hard_spheres.h, sq_plugins_hard_sphere
 */
sasfit_hard_spheres_DLLEXP scalar sasfit_sq_hard_sphere__lhr__f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hard_sphere__lhr_
 *
 * \sa sasfit_hard_spheres.h, sq_plugins_hard_sphere
 */
sasfit_hard_spheres_DLLEXP scalar sasfit_sq_hard_sphere__lhr__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_hard_sphere__lhr_ ################ */

/* ################ start sq_hard_sphere__mv_ ################ */
/**
 * \defgroup sq_hard_sphere__mv_ Hard Sphere (MV)
 * \ingroup sq_plugins_hard_sphere
 *
 * \brief \<some brief description of Hard Sphere (MV) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>volume fraction</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_hard_sphere__mv_
 *
 * \sa sasfit_hard_spheres.h, sq_plugins_hard_sphere
 */
sasfit_hard_spheres_DLLEXP scalar sasfit_sq_hard_sphere__mv_(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hard_sphere__mv_
 *
 * \sa sasfit_hard_spheres.h, sq_plugins_hard_sphere
 */
sasfit_hard_spheres_DLLEXP scalar sasfit_sq_hard_sphere__mv__f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hard_sphere__mv_
 *
 * \sa sasfit_hard_spheres.h, sq_plugins_hard_sphere
 */
sasfit_hard_spheres_DLLEXP scalar sasfit_sq_hard_sphere__mv__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_hard_sphere__mv_ ################ */

/* ################ start sq_hard_sphere__4_3_ ################ */
/**
 * \defgroup sq_hard_sphere__4_3_ Hard Sphere (4,3)
 * \ingroup sq_plugins_hard_sphere
 *
 * \brief \<some brief description of Hard Sphere (4,3) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>volume fraction</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_hard_sphere__4_3_
 *
 * \sa sasfit_hard_spheres.h, sq_plugins_hard_sphere
 */
sasfit_hard_spheres_DLLEXP scalar sasfit_sq_hard_sphere__4_3_(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hard_sphere__4_3_
 *
 * \sa sasfit_hard_spheres.h, sq_plugins_hard_sphere
 */
sasfit_hard_spheres_DLLEXP scalar sasfit_sq_hard_sphere__4_3__f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hard_sphere__4_3_
 *
 * \sa sasfit_hard_spheres.h, sq_plugins_hard_sphere
 */
sasfit_hard_spheres_DLLEXP scalar sasfit_sq_hard_sphere__4_3__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_hard_sphere__4_3_ ################ */

/* ################ start sq_hard_sphere__cs_ ################ */
/**
 * \defgroup sq_hard_sphere__cs_ Hard Sphere (CS)
 * \ingroup sq_plugins_hard_sphere
 *
 * \brief \<some brief description of Hard Sphere (CS) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>volume fraction</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_hard_sphere__cs_
 *
 * \sa sasfit_hard_spheres.h, sq_plugins_hard_sphere
 */
sasfit_hard_spheres_DLLEXP scalar sasfit_sq_hard_sphere__cs_(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hard_sphere__cs_
 *
 * \sa sasfit_hard_spheres.h, sq_plugins_hard_sphere
 */
sasfit_hard_spheres_DLLEXP scalar sasfit_sq_hard_sphere__cs__f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hard_sphere__cs_
 *
 * \sa sasfit_hard_spheres.h, sq_plugins_hard_sphere
 */
sasfit_hard_spheres_DLLEXP scalar sasfit_sq_hard_sphere__cs__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_hard_sphere__cs_ ################ */

/* ################ start sq_hard_sphere__gh_ ################ */
/**
 * \defgroup sq_hard_sphere__gh_ Hard Sphere (GH)
 * \ingroup sq_plugins_hard_sphere
 *
 * \brief \<some brief description of Hard Sphere (GH) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>volume fraction</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_hard_sphere__gh_
 *
 * \sa sasfit_hard_spheres.h, sq_plugins_hard_sphere
 */
sasfit_hard_spheres_DLLEXP scalar sasfit_sq_hard_sphere__gh_(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hard_sphere__gh_
 *
 * \sa sasfit_hard_spheres.h, sq_plugins_hard_sphere
 */
sasfit_hard_spheres_DLLEXP scalar sasfit_sq_hard_sphere__gh__f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hard_sphere__gh_
 *
 * \sa sasfit_hard_spheres.h, sq_plugins_hard_sphere
 */
sasfit_hard_spheres_DLLEXP scalar sasfit_sq_hard_sphere__gh__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_hard_sphere__gh_ ################ */

/* ################ start sq_hard_sphere__py_ ################ */
/**
 * \defgroup sq_hard_sphere__py_ Hard Sphere (PY)
 * \ingroup sq_plugins_hard_sphere
 *
 * \brief \<some brief description of Hard Sphere (PY) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>volume fraction</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_hard_sphere__py_
 *
 * \sa sasfit_hard_spheres.h, sq_plugins_hard_sphere
 */
sasfit_hard_spheres_DLLEXP scalar sasfit_sq_hard_sphere__py_(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hard_sphere__py_
 *
 * \sa sasfit_hard_spheres.h, sq_plugins_hard_sphere
 */
sasfit_hard_spheres_DLLEXP scalar sasfit_sq_hard_sphere__py__f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_hard_sphere__py_
 *
 * \sa sasfit_hard_spheres.h, sq_plugins_hard_sphere
 */
sasfit_hard_spheres_DLLEXP scalar sasfit_sq_hard_sphere__py__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_hard_sphere__py_ ################ */


#endif // this file

