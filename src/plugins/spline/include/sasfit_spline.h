/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_SPLINE_H
#define SASFIT_PLUGIN_SPLINE_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_spline.h
 * Public available functions and descriptions of the spline plugin.
 */

/**
 * \def sasfit_spline_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_spline_EXPORTS)
	#ifdef sasfit_spline_DLLEXP
	#undef sasfit_spline_DLLEXP
	#endif
	#define sasfit_spline_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_spline_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_spline_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_akima8 ################ */
/**
 * \defgroup ff_akima8 Akima8 (FF)
 * \ingroup ff_user2
 *
 * \brief \<some brief description of Akima8 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b xmin</td>
 *       <td>y(xmin)=0</td>
 *      </tr><tr>
 *       <td>\b xmax</td>
 *       <td>y(xmax)=0</td>
 *      </tr><tr>
 *       <td>\b y1</td>
 *       <td>y1=y(1*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y2</td>
 *       <td>y2=y(2*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y3</td>
 *       <td>y3=y(3*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y4</td>
 *       <td>y4=y(4*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y5</td>
 *       <td>y5=y(5*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y6</td>
 *       <td>y6=y(6*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y7</td>
 *       <td>y7=y(7*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y8</td>
 *       <td>y8=y(8*(xmax-xmin)/9)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_akima8
 *
 * \sa sasfit_splines.h, ff_user2
 */
sasfit_spline_DLLEXP scalar sasfit_ff_akima8(scalar q, sasfit_param * p);

/**
 * \ingroup ff_akima8
 *
 * \sa sasfit_splines.h, ff_user2
 */
sasfit_spline_DLLEXP scalar sasfit_ff_akima8_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_akima8
 *
 * \sa sasfit_splines.h, ff_user2
 */
sasfit_spline_DLLEXP scalar sasfit_ff_akima8_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_akima8 ################ */

/* ################ start ff_cspline8 ################ */
/**
 * \defgroup ff_cspline8 CSpline8 (FF)
 * \ingroup ff_user2
 *
 * \brief \<some brief description of CSpline8 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b xmin</td>
 *       <td>y(xmin)=0</td>
 *      </tr><tr>
 *       <td>\b xmax</td>
 *       <td>y(xmax)=0</td>
 *      </tr><tr>
 *       <td>\b y1</td>
 *       <td>y1=y(1*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y2</td>
 *       <td>y2=y(2*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y3</td>
 *       <td>y3=y(3*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y4</td>
 *       <td>y4=y(4*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y5</td>
 *       <td>y5=y(5*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y6</td>
 *       <td>y6=y(6*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y7</td>
 *       <td>y7=y(7*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y8</td>
 *       <td>y8=y(8*(xmax-xmin)/9)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_cspline8
 *
 * \sa sasfit_splines.h, ff_user2
 */
sasfit_spline_DLLEXP scalar sasfit_ff_cspline8(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cspline8
 *
 * \sa sasfit_splines.h, ff_user2
 */
sasfit_spline_DLLEXP scalar sasfit_ff_cspline8_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_cspline8
 *
 * \sa sasfit_splines.h, ff_user2
 */
sasfit_spline_DLLEXP scalar sasfit_ff_cspline8_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_cspline8 ################ */

/* ################ start ff_steffen8 ################ */
/**
 * \defgroup ff_steffen8 Steffen8 (FF)
 * \ingroup ff_user2
 *
 * \brief \<some brief description of Steffen8 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b xmin</td>
 *       <td>y(xmin)=0</td>
 *      </tr><tr>
 *       <td>\b xmax</td>
 *       <td>y(xmax)=0</td>
 *      </tr><tr>
 *       <td>\b y1</td>
 *       <td>y1=y(1*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y2</td>
 *       <td>y2=y(2*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y3</td>
 *       <td>y3=y(3*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y4</td>
 *       <td>y4=y(4*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y5</td>
 *       <td>y5=y(5*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y6</td>
 *       <td>y6=y(6*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y7</td>
 *       <td>y7=y(7*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y8</td>
 *       <td>y8=y(8*(xmax-xmin)/9)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_steffen8
 *
 * \sa sasfit_splines.h, ff_user2
 */
sasfit_spline_DLLEXP scalar sasfit_ff_steffen8(scalar q, sasfit_param * p);

/**
 * \ingroup ff_steffen8
 *
 * \sa sasfit_splines.h, ff_user2
 */
sasfit_spline_DLLEXP scalar sasfit_ff_steffen8_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_steffen8
 *
 * \sa sasfit_splines.h, ff_user2
 */
sasfit_spline_DLLEXP scalar sasfit_ff_steffen8_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_steffen8 ################ */

/* ################ start sd_akima8 ################ */
/**
 * \defgroup sd_akima8 Akima8 (SD)
 * \ingroup sd_plugins
 *
 * \brief \<some brief description of Akima8 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b xmin</td>
 *       <td>y(xmin)=0</td>
 *      </tr><tr>
 *       <td>\b xmax</td>
 *       <td>y(xmax)=0</td>
 *      </tr><tr>
 *       <td>\b y1</td>
 *       <td>y1=y(1*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y2</td>
 *       <td>y2=y(2*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y3</td>
 *       <td>y3=y(3*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y4</td>
 *       <td>y4=y(4*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y5</td>
 *       <td>y5=y(5*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y6</td>
 *       <td>y6=y(6*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y7</td>
 *       <td>y7=y(7*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y8</td>
 *       <td>y8=y(8*(xmax-xmin)/9)</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_akima8
 *
 * \sa sasfit_spline.h, sd_plugins
 */
sasfit_spline_DLLEXP scalar sasfit_sd_akima8(scalar x, sasfit_param * p);

/**
 * \ingroup sd_akima8
 *
 * \sa sasfit_spline.h, sd_plugins
 */
sasfit_spline_DLLEXP scalar sasfit_sd_akima8_f(scalar x, sasfit_param * p);

/**
 * \ingroup sd_akima8
 *
 * \sa sasfit_spline.h, sd_plugins
 */
sasfit_spline_DLLEXP scalar sasfit_sd_akima8_v(scalar x, sasfit_param * p, int dist);
/* ################ stop sd_akima8 ################ */

/* ################ start sd_cspline8 ################ */
/**
 * \defgroup sd_cspline8 CSpline8 (SD)
 * \ingroup sd_plugins
 *
 * \brief \<some brief description of CSpline8 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b xmin</td>
 *       <td>y(xmin)=0</td>
 *      </tr><tr>
 *       <td>\b xmax</td>
 *       <td>y(xmax)=0</td>
 *      </tr><tr>
 *       <td>\b y1</td>
 *       <td>y1=y(1*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y2</td>
 *       <td>y2=y(2*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y3</td>
 *       <td>y3=y(3*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y4</td>
 *       <td>y4=y(4*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y5</td>
 *       <td>y5=y(5*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y6</td>
 *       <td>y6=y(6*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y7</td>
 *       <td>y7=y(7*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y8</td>
 *       <td>y8=y(8*(xmax-xmin)/9)</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_cspline8
 *
 * \sa sasfit_spline.h, sd_plugins
 */
sasfit_spline_DLLEXP scalar sasfit_sd_cspline8(scalar x, sasfit_param * p);

/**
 * \ingroup sd_cspline8
 *
 * \sa sasfit_spline.h, sd_plugins
 */
sasfit_spline_DLLEXP scalar sasfit_sd_cspline8_f(scalar x, sasfit_param * p);

/**
 * \ingroup sd_spline8
 *
 * \sa sasfit_spline.h, sd_plugins
 */
sasfit_spline_DLLEXP scalar sasfit_sd_cspline8_v(scalar x, sasfit_param * p, int dist);
/* ################ stop sd_spline8 ################ */

/* ################ start sd_steffen8 ################ */
/**
 * \defgroup sd_steffen8 Steffen8 (SD)
 * \ingroup sd_plugins
 *
 * \brief \<some brief description of Steffen8 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b xmin</td>
 *       <td>y(xmin)=0</td>
 *      </tr><tr>
 *       <td>\b xmax</td>
 *       <td>y(xmax)=0</td>
 *      </tr><tr>
 *       <td>\b y1</td>
 *       <td>y1=y(1*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y2</td>
 *       <td>y2=y(2*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y3</td>
 *       <td>y3=y(3*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y4</td>
 *       <td>y4=y(4*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y5</td>
 *       <td>y5=y(5*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y6</td>
 *       <td>y6=y(6*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y7</td>
 *       <td>y7=y(7*(xmax-xmin)/9)</td>
 *      </tr><tr>
 *       <td>\b y8</td>
 *       <td>y8=y(8*(xmax-xmin)/9)</td>
 *      </tr></table>
 */

/**
 * \ingroup sd_steffen8
 *
 * \sa sasfit_spline.h, sd_plugins
 */
sasfit_spline_DLLEXP scalar sasfit_sd_steffen8(scalar x, sasfit_param * p);

/**
 * \ingroup sd_steffen8
 *
 * \sa sasfit_spline.h, sd_plugins
 */
sasfit_spline_DLLEXP scalar sasfit_sd_steffen8_f(scalar x, sasfit_param * p);

/**
 * \ingroup sd_steffen8
 *
 * \sa sasfit_spline.h, sd_plugins
 */
sasfit_spline_DLLEXP scalar sasfit_sd_steffen8_v(scalar x, sasfit_param * p, int dist);
/* ################ stop sd_steffen8 ################ */

#endif // this file

