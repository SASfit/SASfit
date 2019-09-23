/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_STOCHASTIC_MODELS_H
#define SASFIT_PLUGIN_STOCHASTIC_MODELS_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_stochastic_models.h
 * Public available functions and descriptions of the stochastic_models plugin.
 */

/**
 * \def sasfit_stochastic_models_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_stochastic_models_EXPORTS)
	#ifdef sasfit_stochastic_models_DLLEXP
	#undef sasfit_stochastic_models_DLLEXP
	#endif
	#define sasfit_stochastic_models_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_stochastic_models_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_stochastic_models_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_clipped_random_waves_1 ################ */
/**
 * \defgroup ff_clipped_random_waves_1 CRW 1
 * \ingroup ff_stochastic
 *
 * \brief \<some brief description of CRW 1 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b scale</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b phi_1</td>
 *       <td>volume fraction of solid phase</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>periodicicity</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>correlation length</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_clipped_random_waves_1
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_1(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_1
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_1_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_1
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_1_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_clipped_random_waves_1 ################ */


/* ################ start ff_clipped_random_waves_2 ################ */
/**
 * \defgroup ff_clipped_random_waves_2 CRW 2
 * \ingroup ff_stochastic
 *
 * \brief \<some brief description of CRW 2 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b scale</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b phi_1</td>
 *       <td>volume fraction of solid phase</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>periodicity</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>correlation length</td>
 *      </tr><tr>
 *       <td>\b rc</td>
 *       <td>cut-off length</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_clipped_random_waves_2
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_2(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_2
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_2_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_2
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_clipped_random_waves_2 ################ */

/* ################ start ff_clipped_random_waves_3 ################ */
/**
 * \defgroup ff_clipped_random_waves_3 CRW 3
 * \ingroup ff_stochastic
 *
 * \brief \<some brief description of CRW 3 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b scale</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b phi_1</td>
 *       <td>volume fraction of solid phase</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>periodicity</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>correlation length</td>
 *      </tr><tr>
 *       <td>\b rc</td>
 *       <td>cut-off length</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_clipped_random_waves_3
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_3(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_3
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_3_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_3
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_3_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_clipped_random_waves_3 ################ */

/* ################ start ff_clipped_random_waves_4 ################ */
/**
 * \defgroup ff_clipped_random_waves_4 CRW 4
 * \ingroup ff_stochastic
 *
 * \brief \<some brief description of CRW 4 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b scale</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b phi_1</td>
 *       <td>volume fraction of solid phase</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>periodicity</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>correlation length</td>
 *      </tr><tr>
 *       <td>\b rc</td>
 *       <td>cut-off length</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_clipped_random_waves_4
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_4(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_4
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_4_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_4
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_4_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_clipped_random_waves_4 ################ */

/* ################ start ff_clipped_random_waves_5 ################ */
/**
 * \defgroup ff_clipped_random_waves_5 CRW 5
 * \ingroup ff_stochastic
 *
 * \brief \<some brief description of CRW 5 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b scale</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b phi_1</td>
 *       <td>volume fraction of solid phase</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>periodicity</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>correlation length</td>
 *      </tr><tr>
 *       <td>\b rc</td>
 *       <td>cut-off length</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_clipped_random_waves_5
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_5(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_5
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_5_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_5
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_5_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_clipped_random_waves_5 ################ */

/* ################ start ff_clipped_random_waves_twocut_1 ################ */
/**
 * \defgroup ff_clipped_random_waves_twocut_1 CRW 1 (two-cut))
 * \ingroup ff_stochastic
 *
 * \brief \<some brief description of CRW 1 (two-cut) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b scale</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b phi_1</td>
 *       <td>volume fraction of solid phase</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>periodicicity</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>correlation length</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_clipped_random_waves_twocut_1
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_1(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_twocut_1
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_1_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_twocut_1
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_1_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_clipped_random_waves_twocut_1 ################ */


/* ################ start ff_clipped_random_waves_twocut_2 ################ */
/**
 * \defgroup ff_clipped_random_waves_twocut_2 CRW 2 (two-cut)
 * \ingroup ff_stochastic
 *
 * \brief \<some brief description of CRW 2 (two-cut) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b scale</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b phi_1</td>
 *       <td>volume fraction of solid phase</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>periodicity</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>correlation length</td>
 *      </tr><tr>
 *       <td>\b rc</td>
 *       <td>cut-off length</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_clipped_random_waves_twocut_2
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_2(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_twocut_2
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_2_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_twocut_2
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_clipped_random_waves_twocut_2 ################ */

/* ################ start ff_clipped_random_waves_twocut_3 ################ */
/**
 * \defgroup ff_clipped_random_waves_twocut_3 CRW 3 (two-cut)
 * \ingroup ff_stochastic
 *
 * \brief \<some brief description of CRW 3 (two-cut) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b scale</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b phi_1</td>
 *       <td>volume fraction of solid phase</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>periodicity</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>correlation length</td>
 *      </tr><tr>
 *       <td>\b rc</td>
 *       <td>cut-off length</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_clipped_random_waves_twocut_3
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_3(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_twocut_3
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_3_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_twocut_3
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_3_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_clipped_random_waves_twocut_3 ################ */

/* ################ start ff_clipped_random_waves_twocut_4 ################ */
/**
 * \defgroup ff_clipped_random_waves_twocut_4 CRW 4 (two-cut)
 * \ingroup ff_stochastic
 *
 * \brief \<some brief description of CRW 4 (two-cut) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b scale</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b phi_1</td>
 *       <td>volume fraction of solid phase</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>periodicity</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>correlation length</td>
 *      </tr><tr>
 *       <td>\b rc</td>
 *       <td>cut-off length</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_clipped_random_waves_twocut_4
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_4(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_twocut_4
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_4_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_twocut_4
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_4_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_clipped_random_waves_twocut_4 ################ */

/* ################ start ff_clipped_random_waves_twocut_5 ################ */
/**
 * \defgroup ff_clipped_random_waves_twocut_5 CRW 5 (two-cut)
 * \ingroup ff_stochastic
 *
 * \brief \<some brief description of CRW 5 (two-cut) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b scale</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b phi_1</td>
 *       <td>volume fraction of solid phase</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>periodicity</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>correlation length</td>
 *      </tr><tr>
 *       <td>\b rc</td>
 *       <td>cut-off length</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_clipped_random_waves_twocut_5
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_5(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_twocut_5
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_5_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_twocut_5
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_5_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_clipped_random_waves_twocut_5 ################ */

/* ################ start ff_clipped_random_waves_twocut_intersection_1 ################ */
/**
 * \defgroup ff_clipped_random_waves_twocut_intersection_1 CRW 1 (two-cut intersection)
 * \ingroup ff_stochastic
 *
 * \brief \<some brief description of CRW 1 (two-cut intersection) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b scale</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b phi_1</td>
 *       <td>volume fraction of solid phase</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>periodicicity</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>correlation length</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_clipped_random_waves_twocut_intersection_1
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_intersection_1(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_twocut_intersection_1
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_intersection_1_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_twocut_intersection_1
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_intersection_1_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_clipped_random_waves_twocut_intersection_1 ################ */


/* ################ start ff_clipped_random_waves_twocut_intersection_2 ################ */
/**
 * \defgroup ff_clipped_random_waves_twocut_intersection_2 CRW 2 (two-cut intersection)
 * \ingroup ff_stochastic
 *
 * \brief \<some brief description of CRW 2 (two-cut intersection) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b scale</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b phi_1</td>
 *       <td>volume fraction of solid phase</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>periodicity</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>correlation length</td>
 *      </tr><tr>
 *       <td>\b rc</td>
 *       <td>cut-off length</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_clipped_random_waves_twocut_intersection_2
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_intersection_2(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_twocut_intersection_2
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_intersection_2_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_twocut_intersection_2
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_intersection_2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_clipped_random_waves_twocut_intersection_2 ################ */

/* ################ start ff_clipped_random_waves_twocut_intersection_3 ################ */
/**
 * \defgroup ff_clipped_random_waves_twocut_intersection_3 CRW 3 (two-cut intersection)
 * \ingroup ff_stochastic
 *
 * \brief \<some brief description of CRW 3 (two-cut intersection) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b scale</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b phi_1</td>
 *       <td>volume fraction of solid phase</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>periodicity</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>correlation length</td>
 *      </tr><tr>
 *       <td>\b rc</td>
 *       <td>cut-off length</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_clipped_random_waves_twocut_intersection_3
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_intersection_3(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_twocut_intersection_3
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_intersection_3_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_twocut_intersection_3
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_intersection_3_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_clipped_random_waves_twocut_intersection_3 ################ */

/* ################ start ff_clipped_random_waves_twocut_intersection_4 ################ */
/**
 * \defgroup ff_clipped_random_waves_twocut_intersection_4 CRW 4 (two-cut intersection)
 * \ingroup ff_stochastic
 *
 * \brief \<some brief description of CRW 4 (two-cut intersection) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b scale</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b phi_1</td>
 *       <td>volume fraction of solid phase</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>periodicity</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>correlation length</td>
 *      </tr><tr>
 *       <td>\b rc</td>
 *       <td>cut-off length</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_clipped_random_waves_twocut_intersection_4
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_intersection_4(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_twocut_intersection_4
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_intersection_4_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_twocut_intersection_4
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_intersection_4_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_clipped_random_waves_twocut_intersection_4 ################ */

/* ################ start ff_clipped_random_waves_twocut_intersection_5 ################ */
/**
 * \defgroup ff_clipped_random_waves_twocut_intersection_5 CRW 5 (two-cut intersection)
 * \ingroup ff_stochastic
 *
 * \brief \<some brief description of CRW 5 (two-cut intersection) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b scale</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b phi_1</td>
 *       <td>volume fraction of solid phase</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>periodicity</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>correlation length</td>
 *      </tr><tr>
 *       <td>\b rc</td>
 *       <td>cut-off length</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_clipped_random_waves_twocut_intersection_5
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_intersection_5(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_twocut_intersection_5
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_intersection_5_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_clipped_random_waves_twocut_intersection_5
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_clipped_random_waves_twocut_intersection_5_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_clipped_random_waves_twocut_intersection_5 ################ */


/* ################ start ff_boolean_intersection_model ################ */
/**
 * \defgroup ff_boolean_intersection_model boolean (intersection model)
 * \ingroup ff_stochastic
 *
 * \brief \<some brief description of boolean (intersection model) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Scale</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b phi_0</td>
 *       <td>volume fraction of pores</td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td>median of LogNorm distribution</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width of LogNorm distribution</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_boolean__intersection_model_
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_boolean_intersection_model(scalar q, sasfit_param * p);

/**
 * \ingroup ff_boolean_intersection_model
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_boolean_intersection_model_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_boolean_intersection_model
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_boolean_intersection_model_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_boolean_intersection_model ################ */

/* ################ start ff_boolean_union_model ################ */
/**
 * \defgroup ff_boolean_union_model boolean (union model)
 * \ingroup ff_stochastic
 *
 * \brief \<some brief description of boolean (union model) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Scale</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b phi_1</td>
 *       <td>volume fraction of pores</td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td>median of LogNorm distribution</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width of LogNorm distribution</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_boolean_union_model
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_boolean_union_model(scalar q, sasfit_param * p);

/**
 * \ingroup ff_boolean_union_model
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_boolean_union_model_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_boolean_union_model
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_boolean_union_model_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_boolean_union_model ################ */

/* ################ start ff_dead_leaves_model ################ */
/**
 * \defgroup ff_dead_leaves_model dead leaves model
 * \ingroup ff_stochastic
 *
 * \brief \<some brief description of deadf leaves model function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Scale</td>
 *       <td>scale parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b mu</td>
 *       <td>median of LogNorm distribution</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width of LogNorm distribution</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_dead_leaves_model
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_dead_leaves_model(scalar q, sasfit_param * p);

/**
 * \ingroup ff_dead_leaves_model
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_dead_leaves_model_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_dead_leaves_model
 *
 * \sa sasfit_stochastic_models.h, ff_stochastic
 */
sasfit_stochastic_models_DLLEXP scalar sasfit_ff_dead_leaves_model_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_dead_leaves_model ################ */

#endif // this file

