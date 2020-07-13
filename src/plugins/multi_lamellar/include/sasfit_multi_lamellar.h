/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_MULTI_LAMELLAR_H
#define SASFIT_PLUGIN_MULTI_LAMELLAR_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_multi_lamellar.h
 * Public available functions and descriptions of the multi_lamellar plugin.
 */

/**
 * \def sasfit_multi_lamellar_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_multi_lamellar_EXPORTS)
	#ifdef sasfit_multi_lamellar_DLLEXP
	#undef sasfit_multi_lamellar_DLLEXP
	#endif
	#define sasfit_multi_lamellar_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_multi_lamellar_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_multi_lamellar_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq_perfect_ordered ################ */
/**
 * \defgroup sq_perfect_ordered Perfect Finite Stack
 * \ingroup sq_plugins_multi_lamellar
 *
 * \brief \<some brief description of Perfect Finite Stack function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>total number of layers in stack</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>stacking separation</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Nu</td>
 *       <td>scaling constant for additional diffuse scattering term</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_perfect_ordered
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_perfect_ordered(scalar q, sasfit_param * p);

/**
 * \ingroup sq_perfect_ordered
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_perfect_ordered_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_perfect_ordered
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_perfect_ordered_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_perfect_ordered ################ */

/* ################ start sq_perfect_ordered_sum ################ */
/**
 * \defgroup sq_perfect_ordered_sum Perfect Finite Stack (polydisp.,sum.)
 * \ingroup sq_plugins_multi_lamellar
 *
 * \brief \<some brief description of Perfect Finite Stack (polydisp.,sum.) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>total number of layers in stack</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>stacking separation</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Nu</td>
 *       <td>scaling constant for additional diffuse scattering term</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_perfect_ordered_sum
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_perfect_ordered_sum(scalar q, sasfit_param * p);

/**
 * \ingroup sq_perfect_ordered_sum
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_perfect_ordered_sum_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_perfect_ordered_sum
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_perfect_ordered_sum_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_perfect_ordered_sum ################ */

/* ################ start sq_perfect_ordered_sd ################ */
/**
 * \defgroup sq_perfect_ordered_sd Perfect Finite Stack (polydisp.,int.)
 * \ingroup sq_plugins_multi_lamellar
 *
 * \brief \<some brief description of Perfect Finite Stack (polydisp.,int.) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>total number of layers in stack</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>stacking separation</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Nu</td>
 *       <td>scaling constant for additional diffuse scattering term</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_perfect_ordered_sd
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_perfect_ordered_sd(scalar q, sasfit_param * p);

/**
 * \ingroup sq_perfect_ordered_sd
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_perfect_ordered_sd_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_perfect_ordered_sd
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_perfect_ordered_sd_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_perfect_ordered_sd ################ */


/* ################ start sq_thermaldisorder ################ */
/**
 * \defgroup sq_thermaldisorder Thermal Disorder
 * \ingroup sq_plugins_multi_lamellar
 *
 * \brief \<some brief description of Thermal Disorder function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>total number of layers in stack</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>stacking distance</td>
 *      </tr><tr>
 *       <td>\b Delta</td>
 *       <td>Debye-Waller temperature factor</td>
 *      </tr><tr>
 *       <td>\b Nu</td>
 *       <td>scaling constant for additional diffuse scattering term</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_thermaldisorder
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_thermaldisorder(scalar q, sasfit_param * p);

/**
 * \ingroup sq_thermaldisorder
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_thermaldisorder_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_thermaldisorder
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_thermaldisorder_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_thermaldisorder ################ */

/* ################ start sq_thermaldisorder_sum ################ */
/**
 * \defgroup sq_thermaldisorder_sum Thermal Disorder (polydisp.,sum.)
 * \ingroup sq_plugins_multi_lamellar
 *
 * \brief \<some brief description of Thermal Disorder (polydisp.,sum.) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>total number of layers in stack</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>stacking distance</td>
 *      </tr><tr>
 *       <td>\b Delta</td>
 *       <td>Debye-Waller temperature factor</td>
 *      </tr><tr>
 *       <td>\b Nu</td>
 *       <td>scaling constant for additional diffuse scattering term</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_thermaldisorder_sum
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_thermaldisorder_sum(scalar q, sasfit_param * p);

/**
 * \ingroup sq_thermaldisorder_sum
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_thermaldisorder_sum_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_thermaldisorder_sum
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_thermaldisorder_sum_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_thermaldisorder_sum ################ */

/* ################ start sq_thermaldisorder_sd ################ */
/**
 * \defgroup sq_thermaldisorder_sd Thermal Disorder (polydisp.,int.)
 * \ingroup sq_plugins_multi_lamellar
 *
 * \brief \<some brief description of Thermal Disorder (polydisp.,int.) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>total number of layers in stack</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>stacking distance</td>
 *      </tr><tr>
 *       <td>\b Delta</td>
 *       <td>Debye-Waller temperature factor</td>
 *      </tr><tr>
 *       <td>\b Nu</td>
 *       <td>scaling constant for additional diffuse scattering term</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_thermaldisorder_sd
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_thermaldisorder_sd(scalar q, sasfit_param * p);

/**
 * \ingroup sq_thermaldisorder_sd
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_thermaldisorder_sd_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_thermaldisorder_sd
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_thermaldisorder_sd_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_thermaldisorder_sd ################ */

/* ################ start sq_paracrystalline ################ */
/**
 * \defgroup sq_paracrystalline Paracrystalline
 * \ingroup sq_plugins_multi_lamellar
 *
 * \brief \<some brief description of Paracrystalline function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>total number of layers in stack</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>stacking separation</td>
 *      </tr><tr>
 *       <td>\b Delta</td>
 *       <td>stacking disorder parameter</td>
 *      </tr><tr>
 *       <td>\b Nu</td>
 *       <td>scaling constant for additional diffuse scattering term</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_paracrystalline
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_paracrystalline(scalar q, sasfit_param * p);

/**
 * \ingroup sq_paracrystalline
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_paracrystalline_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_paracrystalline
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_paracrystalline_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_paracrystalline ################ */

/* ################ start sq_paracrystalline_sum ################ */
/**
 * \defgroup sq_paracrystalline_sum Paracrystalline (polydisp.,sum.)
 * \ingroup sq_plugins_multi_lamellar
 *
 * \brief \<some brief description of Paracrystalline (polydisp.,sum.) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>total number of layers in stack</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>stacking separation</td>
 *      </tr><tr>
 *       <td>\b Delta</td>
 *       <td>stacking disorder parameter</td>
 *      </tr><tr>
 *       <td>\b Nu</td>
 *       <td>scaling constant for additional diffuse scattering term</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_paracrystalline_sum
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_paracrystalline_sum(scalar q, sasfit_param * p);

/**
 * \ingroup sq_paracrystalline_sum
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_paracrystalline_sum_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_paracrystalline_sum
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_paracrystalline_sum_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_paracrystalline_sum ################ */

/* ################ start sq_paracrystalline_sd ################ */
/**
 * \defgroup sq_paracrystalline_sd Paracrystalline (polydisp.,int.)
 * \ingroup sq_plugins_multi_lamellar
 *
 * \brief \<some brief description of Paracrystalline (polydisp.,int.) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>total number of layers in stack</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>stacking separation</td>
 *      </tr><tr>
 *       <td>\b Delta</td>
 *       <td>stacking disorder parameter</td>
 *      </tr><tr>
 *       <td>\b Nu</td>
 *       <td>scaling constant for additional diffuse scattering term</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_paracrystalline_sd
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_paracrystalline_sd(scalar q, sasfit_param * p);

/**
 * \ingroup sq_paracrystalline_sd
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_paracrystalline_sd_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_paracrystalline_sd
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_paracrystalline_sd_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_paracrystalline_sd ################ */

/* ################ start sq_modifiedcaille ################ */
/**
 * \defgroup sq_modifiedcaille Modified Caille
 * \ingroup sq_plugins_multi_lamellar
 *
 * \brief \<some brief description of Modified Caille function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>total number of layers in stack</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>stacking separation</td>
 *      </tr><tr>
 *       <td>\b eta_1</td>
 *       <td>Caillé parameter as a measure for bilayer bending fluctuation</td>
 *      </tr><tr>
 *       <td>\b Nu</td>
 *       <td>scaling constant for additional diffuse scattering term</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_modifiedcaille
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_modifiedcaille(scalar q, sasfit_param * p);

/**
 * \ingroup sq_modifiedcaille
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_modifiedcaille_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_modifiedcaille
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_modifiedcaille_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_modifiedcaille ################ */

/* ################ start sq_modifiedcaille_sum ################ */
/**
 * \defgroup sq_modifiedcaille_sum Modified Caille (polydisp.,sum.)
 * \ingroup sq_plugins_multi_lamellar
 *
 * \brief \<some brief description of Modified Caille (polydisp.,sum.) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>total number of layers in stack</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>stacking separation</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>Caillé parameter as a measure for bilayer bending fluctuation</td>
 *      </tr><tr>
 *       <td>\b Nu</td>
 *       <td>scaling constant for additional diffuse scattering term</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_modifiedcaille_sum
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_modifiedcaille_sum(scalar q, sasfit_param * p);

/**
 * \ingroup sq_modifiedcaille_sum
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_modifiedcaille_sum_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_modifiedcaille_sum
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_modifiedcaille_sum_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_modifiedcaille_sum ################ */

/* ################ start sq_modifiedcaille_sd ################ */
/**
 * \defgroup sq_modifiedcaille_sd Modified Caille (polydisp.,int.)
 * \ingroup sq_plugins_multi_lamellar
 *
 * \brief \<some brief description of Modified Caille (polydisp.,int.) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>total number of layers in stack</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>stacking separation</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>Caillé parameter as a measure for bilayer bending fluctuation</td>
 *      </tr><tr>
 *       <td>\b Nu</td>
 *       <td>scaling constant for additional diffuse scattering term</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_modifiedcaille_sd
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_modifiedcaille_sd(scalar q, sasfit_param * p);

/**
 * \ingroup sq_modifiedcaille_sd
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_modifiedcaille_sd_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_modifiedcaille_sd
 *
 * \sa sasfit_multi_lamellar.h, sq_plugins_multi_lamellar
 */
sasfit_multi_lamellar_DLLEXP scalar sasfit_sq_modifiedcaille_sd_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_modifiedcaille_sd ################ */

#endif // this file

