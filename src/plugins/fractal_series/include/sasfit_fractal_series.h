/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_FRACTAL_SERIES_H
#define SASFIT_PLUGIN_FRACTAL_SERIES_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_fractal_series.h
 * Public available functions and descriptions of the fractal_series plugin.
 */

/**
 * \def sasfit_fractal_series_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_fractal_series_EXPORTS)
	#ifdef sasfit_fractal_series_DLLEXP
	#undef sasfit_fractal_series_DLLEXP
	#endif
	#define sasfit_fractal_series_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_fractal_series_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_fractal_series_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_fractal_series1 ################ */
/**
 * \defgroup ff_fractal_series1 fractal series1
 * \ingroup ff_fractals
 *
 * \brief \<some brief description of fractal series1 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>number density</td>
 *      </tr><tr>
 *       <td>\b Rg_min</td>
 *       <td>minimum radius of gyration</td>
 *      </tr><tr>
 *       <td>\b fd1</td>
 *       <td>fractal dimension</td>
 *      </tr><tr>
 *       <td>\b Rg_max</td>
 *       <td>maximum radius of gyration</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_fractal_series1
 *
 * \sa sasfit_fractal_series.h, ff_fractals
 */
sasfit_fractal_series_DLLEXP scalar sasfit_ff_fractal_series1(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fractal_series1
 *
 * \sa sasfit_fractal_series.h, ff_fractals
 */
sasfit_fractal_series_DLLEXP scalar sasfit_ff_fractal_series1_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fractal_series1
 *
 * \sa sasfit_fractal_series.h, ff_fractals
 */
sasfit_fractal_series_DLLEXP scalar sasfit_ff_fractal_series1_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_fractal_series1 ################ */

/* ################ start sd_fractal_series1 ################ */
/**
 * \defgroup sd_fractal_series1 size distr.: fractal series1
 * \ingroup ff_fractals
 *
 * \brief \<some brief description of fractal series1 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>number density</td>
 *      </tr><tr>
 *       <td>\b xmin</td>
 *       <td>minimum size</td>
 *      </tr><tr>
 *       <td>\b fd1</td>
 *       <td>fractal dimension</td>
 *      </tr><tr>
 *       <td>\b xmax</td>
 *       <td>maximum size</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_fractal_series1
 *
 * \sa sasfit_fractal_series.h, ff_fractals
 */
sasfit_fractal_series_DLLEXP scalar sasfit_sd_fractal_series1(scalar q, sasfit_param * p);

/**
 * \ingroup sd_fractal_series1
 *
 * \sa sasfit_fractal_series.h, ff_fractals
 */
sasfit_fractal_series_DLLEXP scalar sasfit_sd_fractal_series1_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_fractal_series1
 *
 * \sa sasfit_fractal_series.h, ff_fractals
 */
sasfit_fractal_series_DLLEXP scalar sasfit_sd_fractal_series1_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_fractal_series1 ################ */

/* ################ start ff_fractal_series2 ################ */
/**
 * \defgroup ff_fractal_series2 fractal series2
 * \ingroup ff_fractals
 *
 * \brief \<some brief description of fractal series2 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>number density</td>
 *      </tr><tr>
 *       <td>\b xmin</td>
 *       <td>minimum size</td>
 *      </tr><tr>
 *       <td>\b fd1</td>
 *       <td>first fractal dimension</td>
 *      </tr><tr>
 *       <td>\b x12</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b fd2</td>
 *       <td>second fractal dimension</td>
 *      </tr><tr>
 *       <td>\b xmax</td>
 *       <td>maximum size</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_fractal_series2
 *
 * \sa sasfit_fractal_series.h, ff_fractals
 */
sasfit_fractal_series_DLLEXP scalar sasfit_ff_fractal_series2(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fractal_series2
 *
 * \sa sasfit_fractal_series.h, ff_fractals
 */
sasfit_fractal_series_DLLEXP scalar sasfit_ff_fractal_series2_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fractal_series2
 *
 * \sa sasfit_fractal_series.h, ff_fractals
 */
sasfit_fractal_series_DLLEXP scalar sasfit_ff_fractal_series2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_fractal_series2 ################ */

/* ################ start sd_fractal_series2 ################ */
/**
 * \defgroup sd_fractal_series2 size distr.: fractal series2
 * \ingroup ff_fractals
 *
 * \brief \<some brief description of fractal series2 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>number density</td>
 *      </tr><tr>
 *       <td>\b xmin</td>
 *       <td>minimum size</td>
 *      </tr><tr>
 *       <td>\b fd1</td>
 *       <td>first fractal dimension</td>
 *      </tr><tr>
 *       <td>\b x12</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b fd2</td>
 *       <td>second fractal dimension</td>
 *      </tr><tr>
 *       <td>\b xmax</td>
 *       <td>maximum size</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_fractal_series2
 *
 * \sa sasfit_fractal_series.h, ff_fractals
 */
sasfit_fractal_series_DLLEXP scalar sasfit_sd_fractal_series2(scalar q, sasfit_param * p);

/**
 * \ingroup sd_fractal_series2
 *
 * \sa sasfit_fractal_series.h, ff_fractals
 */
sasfit_fractal_series_DLLEXP scalar sasfit_sd_fractal_series2_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_fractal_series2
 *
 * \sa sasfit_fractal_series.h, ff_fractals
 */
sasfit_fractal_series_DLLEXP scalar sasfit_sd_fractal_series2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_fractal_series2 ################ */

/* ################ start ff_fractal_series3 ################ */
/**
 * \defgroup ff_fractal_series3 fractal series3
 * \ingroup ff_fractals
 *
 * \brief \<some brief description of fractal series3 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>number density</td>
 *      </tr><tr>
 *       <td>\b xmin</td>
 *       <td>minimum size</td>
 *      </tr><tr>
 *       <td>\b fd1</td>
 *       <td>first fractal dimension</td>
 *      </tr><tr>
 *       <td>\b x12</td>
 *       <td>first intermediate size where fractal dimension changes</td>
 *      </tr><tr>
 *       <td>\b fd2</td>
 *       <td>second fractal dimension</td>
 *      </tr><tr>
 *       <td>\b x23</td>
 *       <td>second intermediat size  where fractal dimension changes</td>
 *      </tr><tr>
 *       <td>\b fd3</td>
 *       <td>third fractal dimension</td>
 *      </tr><tr>
 *       <td>\b xmax</td>
 *       <td>maximum size</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_fractal_series3
 *
 * \sa sasfit_fractal_series.h, ff_fractals
 */
sasfit_fractal_series_DLLEXP scalar sasfit_ff_fractal_series3(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fractal_series3
 *
 * \sa sasfit_fractal_series.h, ff_fractals
 */
sasfit_fractal_series_DLLEXP scalar sasfit_ff_fractal_series3_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fractal_series3
 *
 * \sa sasfit_fractal_series.h, ff_fractals
 */
sasfit_fractal_series_DLLEXP scalar sasfit_ff_fractal_series3_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_fractal_series3 ################ */

/* ################ start sd_fractal_series3 ################ */
/**
 * \defgroup sd_fractal_series3 size distr.: fractal series3
 * \ingroup ff_fractals
 *
 * \brief \<some brief description of fractal series3 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b N</td>
 *       <td>number density</td>
 *      </tr><tr>
 *       <td>\b xmin</td>
 *       <td>minimum size</td>
 *      </tr><tr>
 *       <td>\b fd1</td>
 *       <td>first fractal dimension</td>
 *      </tr><tr>
 *       <td>\b x12</td>
 *       <td>first intermediate size where fractal dimension changes</td>
 *      </tr><tr>
 *       <td>\b fd2</td>
 *       <td>second fractal dimension</td>
 *      </tr><tr>
 *       <td>\b x23</td>
 *       <td>second intermediat size  where fractal dimension changes</td>
 *      </tr><tr>
 *       <td>\b fd3</td>
 *       <td>third fractal dimension</td>
 *      </tr><tr>
 *       <td>\b xmax</td>
 *       <td>maximum size</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_fractal_series3
 *
 * \sa sasfit_fractal_series.h, ff_plugins
 */
sasfit_fractal_series_DLLEXP scalar sasfit_sd_fractal_series3(scalar q, sasfit_param * p);

/**
 * \ingroup sd_fractal_series3
 *
 * \sa sasfit_fractal_series.h, ff_fractals
 */
sasfit_fractal_series_DLLEXP scalar sasfit_sd_fractal_series3_f(scalar q, sasfit_param * p);

/**
 * \ingroup sd_fractal_series3
 *
 * \sa sasfit_fractal_series.h, ff_fractals
 */
sasfit_fractal_series_DLLEXP scalar sasfit_sd_fractal_series3_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sd_fractal_series3 ################ */

#endif // this file

