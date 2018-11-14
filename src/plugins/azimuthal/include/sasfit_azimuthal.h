/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_AZIMUTHAL_H
#define SASFIT_PLUGIN_AZIMUTHAL_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_azimuthal.h
 * Public available functions and descriptions of the azimuthal plugin.
 */

/**
 * \def sasfit_azimuthal_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_azimuthal_EXPORTS)
	#ifdef sasfit_azimuthal_DLLEXP
	#undef sasfit_azimuthal_DLLEXP
	#endif
	#define sasfit_azimuthal_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_azimuthal_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_azimuthal_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_a_bsin2_csin4__deg_ ################ */
/** 
 * \defgroup ff_a_bsin2_csin4__deg_ A+Bsin2+Csin4 (deg)
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of A+Bsin2+Csin4 (deg) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>isotropic</td>
 *      </tr><tr>
 *       <td>\b B</td>
 *       <td>anisotropic sin^2(psi) term</td>
 *      </tr><tr>
 *       <td>\b C</td>
 *       <td>anisotropic sin^4(psi) term</td>
 *      </tr><tr>
 *       <td>\b Delta2</td>
 *       <td>offset for sin^2</td>
 *      </tr><tr>
 *       <td>\b Delta4</td>
 *       <td>offset for sin^4</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_a_bsin2_csin4__deg_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_a_bsin2_csin4__deg_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_a_bsin2_csin4__deg_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_a_bsin2_csin4__deg__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_a_bsin2_csin4__deg_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_a_bsin2_csin4__deg__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_a_bsin2_csin4__deg_ ################ */

/* ################ start ff_a_bsin2_csin4__rad_ ################ */
/** 
 * \defgroup ff_a_bsin2_csin4__rad_ A+Bsin2+Csin4 (rad)
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of A+Bsin2+Csin4 (rad) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>isotropic</td>
 *      </tr><tr>
 *       <td>\b B</td>
 *       <td>anisotropic sin^2(psi) term</td>
 *      </tr><tr>
 *       <td>\b C</td>
 *       <td>anisotropic sin^4(psi) term</td>
 *      </tr><tr>
 *       <td>\b Delta2</td>
 *       <td>offset for sin^2</td>
 *      </tr><tr>
 *       <td>\b Delta4</td>
 *       <td>offset for sin^4</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_a_bsin2_csin4__rad_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_a_bsin2_csin4__rad_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_a_bsin2_csin4__rad_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_a_bsin2_csin4__rad__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_a_bsin2_csin4__rad_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_a_bsin2_csin4__rad__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_a_bsin2_csin4__rad_ ################ */

/* ################ start ff_maiersaupe__deg_ ################ */
/** 
 * \defgroup ff_maiersaupe__deg_ MaierSaupe (deg)
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of MaierSaupe (deg) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>isotropic</td>
 *      </tr><tr>
 *       <td>\b B</td>
 *       <td>anosotropic</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>width</td>
 *      </tr><tr>
 *       <td>\b Delta</td>
 *       <td>offset</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_maiersaupe__deg_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_maiersaupe__deg_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_maiersaupe__deg_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_maiersaupe__deg__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_maiersaupe__deg_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_maiersaupe__deg__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_maiersaupe__deg_ ################ */

/* ################ start ff_maiersaupe__rad_ ################ */
/** 
 * \defgroup ff_maiersaupe__rad_ MaierSaupe (rad)
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of MaierSaupe (rad) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>isotropic</td>
 *      </tr><tr>
 *       <td>\b B</td>
 *       <td>anosotropic</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>width</td>
 *      </tr><tr>
 *       <td>\b Delta</td>
 *       <td>offset</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_maiersaupe__rad_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_maiersaupe__rad_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_maiersaupe__rad_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_maiersaupe__rad__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_maiersaupe__rad_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_maiersaupe__rad__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_maiersaupe__rad_ ################ */

/* ################ start ff_elliptically_averaged_deg_ ################ */
/** 
 * \defgroup ff_elliptically_averaged_deg_ elliptically averaged (deg)
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of elliptically averaged (deg) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>first half axes</td>
 *      </tr><tr>
 *       <td>\b B</td>
 *       <td>second half axes</td>
 *      </tr><tr>
 *       <td>\b N</td>
 *       <td>power law</td>
 *      </tr><tr>
 *       <td>\b Delta</td>
 *       <td>offset</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_elliptically_averaged_deg_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_elliptically_averaged_deg_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_elliptically_averaged_deg_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_elliptically_averaged_deg__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_elliptically_averaged_deg_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_elliptically_averaged_deg__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_elliptically_averaged_deg_ ################ */

/* ################ start ff_elliptically_averaged_rad_ ################ */
/** 
 * \defgroup ff_elliptically_averaged_rad_ elliptically averaged (rad)
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of elliptically averaged (rad) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b A</td>
 *       <td>first half axes</td>
 *      </tr><tr>
 *       <td>\b B</td>
 *       <td>second half axes</td>
 *      </tr><tr>
 *       <td>\b N</td>
 *       <td>power law</td>
 *      </tr><tr>
 *       <td>\b Delta</td>
 *       <td>offset</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_elliptically_averaged_rad_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_elliptically_averaged_rad_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_elliptically_averaged_rad_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_elliptically_averaged_rad__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_maiersaupe__rad_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_elliptically_averaged_rad__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_elliptically_averaged_rad_ ################ */

#endif // this file

