/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
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
 *       <td>anisotropic sin^2-term</td>
 *      </tr><tr>
 *       <td>\b C</td>
 *       <td>anisotropic sin^4-term</td>
 *      </tr><tr>
 *       <td>\b Delta</td>
 *       <td>offset for direction of psi=0 direction</td>
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
 *       <td>anisotropic sin^2-term</td>
 *      </tr><tr>
 *       <td>\b C</td>
 *       <td>anisotropic sin^4-term</td>
 *      </tr><tr>
 *       <td>\b Delta</td>
 *       <td>offset for direction of psi=0 direction</td>
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
 *       <td>\b I0</td>
 *       <td>flat background</td>
 *      </tr><tr>
 *       <td>\b A</td>
 *       <td>anisotropic amplitude</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>width</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>reference direction  relative to horizontal direction in degree</td>
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
 *       <td>\b I0</td>
 *       <td>flat background</td>
 *      </tr><tr>
 *       <td>\b A</td>
 *       <td>anisotropic amplitude</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>width</td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>reference direction relative to horizontal direction in radian</td>
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

/* ################ start ff_affine_shrinkage_deg ################ */
/**
 * \defgroup ff_affine_shrinkage_deg affine shrinkage (deg)
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of affine shrinkage (deg) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I0</td>
 *       <td>flat background</td>
 *      </tr><tr>
 *       <td>\b A</td>
 *       <td>anisotropic amplitude</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>shrinkage factor 0<lambda<infinity </td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>direction of shrinkage relative to horizontal direction in degree</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_affine_shrinkage_deg
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_affine_shrinkage_deg(scalar q, sasfit_param * p);

/**
 * \ingroup ff_affine_shrinkage_deg
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_affine_shrinkage_deg_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_affine_shrinkage_deg
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_affine_shrinkage_deg_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_affine_shrinkage_deg ################ */

/* ################ start ff_affine_shrinkage_rad ################ */
/**
 * \defgroup ff_affine_shrinkage_rad affine shrinkage (rad)
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of affine shrinkage (rad) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I0</td>
 *       <td>flat background</td>
 *      </tr><tr>
 *       <td>\b A</td>
 *       <td>anisotropic amplitude</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>shrinkage factor 0<lambda<infinity </td>
 *      </tr><tr>
 *       <td>\b delta</td>
 *       <td>direction of shrinkage relative to horizontal direction in radian</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_affine_shrinkage_rad
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_affine_shrinkage_rad(scalar q, sasfit_param * p);

/**
 * \ingroup ff_affine_shrinkage_rad
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_affine_shrinkage_rad_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_affine_shrinkage_rad
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_affine_shrinkage_rad_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_affine_shrinkage_rad ################ */

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
 *       <td>\b C</td>
 *       <td>constant background</td>
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
 *       <td>\b C</td>
 *       <td>constant background</td>
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
 * \ingroup ff_elliptically_averaged_rad_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_elliptically_averaged_rad__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_elliptically_averaged_rad_ ################ */

/* ################ start ff_longstructures__kratky_ms_deg_ ################ */
/**
 * \defgroup ff_longstructures__kratky_ms_deg_ azimuthal (long cyl.,MS,K,deg)
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of azimuthal (long cyl.,MS,K,deg) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I0</td>
 *       <td>flat background</td>
 *      </tr><tr>
 *       <td>\b A</td>
 *       <td>anisotropic amplitude</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>width of orientation distribution</td>
 *      </tr><tr>
 *       <td>\b Delta</td>
 *       <td>offset in deg</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_longstructures__kratky_ms_deg_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_longstructures__kratky_ms_deg_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_longstructures__kratky_ms_deg_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_longstructures__kratky_ms_deg__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_longstructures__kratky_ms_deg_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_longstructures__kratky_ms_deg__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_longstructures__kratky_ms_deg_ ################ */

/* ################ start ff_longstructures__kratky_ms_rad_ ################ */
/**
 * \defgroup ff_longstructures__kratky_ms_rad_ azimuthal (long cyl.,MS,K,rad)
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of azimuthal (long cyl.,MS,K,rad) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I0</td>
 *       <td>flat background</td>
 *      </tr><tr>
 *       <td>\b A</td>
 *       <td>anisotropic amplitude</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>width of orientation distribution</td>
 *      </tr><tr>
 *       <td>\b Delta</td>
 *       <td>offset in rad</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_longstructures__kratky_ms_rad_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_longstructures__kratky_ms_rad_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_longstructures__kratky_ms_rad_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_longstructures__kratky_ms_rad__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_longstructures__kratky_ms_rad_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_longstructures__kratky_ms_rad__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_longstructures__kratky_ms_rad_ ################ */

/* ################ start ff_longstructures__kratky_onsager_deg_ ################ */
/**
 * \defgroup ff_longstructures__kratky_onsager_deg_ azimuthal (long cyl.,Onsager,K,deg)
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of azimuthal (long cyl.,Onsager,K,deg) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I0</td>
 *       <td>flat background</td>
 *      </tr><tr>
 *       <td>\b A</td>
 *       <td>anisotropic amplitude</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>width of orientation distribution</td>
 *      </tr><tr>
 *       <td>\b Delta</td>
 *       <td>offset in deg</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_longstructures__kratky_onsager_deg_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_longstructures__kratky_onsager_deg_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_longstructures__kratky_onsager_deg_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_longstructures__kratky_onsager_deg__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_longstructures__kratky_onsager_deg_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_longstructures__kratky_onsager_deg__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_longstructures__kratky_onsager_deg_ ################ */

/* ################ start ff_longstructures__kratky_onsager_rad_ ################ */
/**
 * \defgroup ff_longstructures__kratky_onsager_rad_ azimuthal (long cyl.,Onsager,K,rad)
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of azimuthal (long cyl.,Onsager,K,rad) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I0</td>
 *       <td>flat background</td>
 *      </tr><tr>
 *       <td>\b A</td>
 *       <td>anisotropic amplitude</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>width of orientation distribution</td>
 *      </tr><tr>
 *       <td>\b Delta</td>
 *       <td>offset in rad</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_longstructures__kratky_onsager_rad_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_longstructures__kratky_onsager_rad_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_longstructures__kratky_onsager_rad_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_longstructures__kratky_onsager_rad__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_longstructures__kratky_onsager_rad_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_longstructures__kratky_onsager_rad__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_longstructures__kratky_onsager_rad_ ################ */


/* ################ start ff_longstructures__ln_ms_deg_ ################ */
/**
 * \defgroup ff_longstructures__ln_ms_deg_ azimuthal (long cyl.,MS,LN,deg)
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of azimuthal (long cyl.,MS,LN,deg) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I0</td>
 *       <td>flat background</td>
 *      </tr><tr>
 *       <td>\b A</td>
 *       <td>anisotropic amplitude</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>width of orientation distribution</td>
 *      </tr><tr>
 *       <td>\b Delta</td>
 *       <td>offset in deg</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_longstructures__ln_ms_deg_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_longstructures__ln_ms_deg_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_longstructures__ln_ms_deg_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_longstructures__ln_ms_deg__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_longstructures__ln_ms_deg_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_longstructures__ln_ms_deg__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_longstructures__ln_ms_deg_ ################ */

/* ################ start ff_longstructures__ln_ms_rad_ ################ */
/**
 * \defgroup ff_longstructures__ln_ms_rad_ azimuthal (long cyl.,MS,LN,rad)
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of azimuthal (long cyl.,MS,LN,rad) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I0</td>
 *       <td>flat background</td>
 *      </tr><tr>
 *       <td>\b A</td>
 *       <td>anisotropic amplitude</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>width of orientation distribution</td>
 *      </tr><tr>
 *       <td>\b Delta</td>
 *       <td>offset in rad</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_longstructures__ln_ms_rad_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_longstructures__ln_ms_rad_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_longstructures__ln_ms_rad_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_longstructures__ln_ms_rad__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_longstructures__ln_ms_rad_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_longstructures__ln_ms_rad__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_longstructures__ln_ms_rad_ ################ */

/* ################ start ff_longstructures__ln_onsager_deg_ ################ */
/**
 * \defgroup ff_longstructures__ln_onsager_deg_ azimuthal (long cyl.,Onsager,LN,deg)
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of azimuthal (long cyl.,Onsager,LN,deg) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I0</td>
 *       <td>flat background</td>
 *      </tr><tr>
 *       <td>\b A</td>
 *       <td>anisotropic amplitude</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>width of orientation distribution</td>
 *      </tr><tr>
 *       <td>\b Delta</td>
 *       <td>offset in deg</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_longstructures__ln_onsager_deg_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_longstructures__ln_onsager_deg_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_longstructures__ln_onsager_deg_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_longstructures__ln_onsager_deg__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_longstructures__ln_onsager_deg_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_longstructures__ln_onsager_deg__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_longstructures__ln_onsager_deg_ ################ */

/* ################ start ff_longstructures__ln_onsager_rad_ ################ */
/**
 * \defgroup ff_longstructures__ln_onsager_rad_ azimuthal (long cyl.,Onsager,LN,rad)
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of azimuthal (long cyl.,Onsager,LN,rad) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I0</td>
 *       <td>flat background</td>
 *      </tr><tr>
 *       <td>\b A</td>
 *       <td>anisotropic amplitude</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>width of orientation distribution</td>
 *      </tr><tr>
 *       <td>\b Delta</td>
 *       <td>offset in rad</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_longstructures__ln_onsager_rad_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_longstructures__ln_onsager_rad_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_longstructures__ln_onsager_rad_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_longstructures__ln_onsager_rad__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_longstructures__ln_onsager_rad_
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_longstructures__ln_onsager_rad__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_longstructures__ln_onsager_rad_ ################ */

/* ################ start ff_azimuthal_sheared_cylinders_maier_saupe ################ */
/**
 * \defgroup ff_azimuthal_sheared_cylinders_maier_saupe Sheared Cylinders (Maier-Saupe) azimuthal
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of Sheared Cylinders (Maier-Saupe) azimuthal function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>cylinder length</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b Q</td>
 *       <td>scattering vector</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width parameter of lognormal size distribution of radii</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>orientation distribution parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>not used</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_azimuthal_sheared_cylinders_maier_saupe
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_cylinders_maier_saupe(scalar q, sasfit_param * p);

/**
 * \ingroup ff_azimuthal_sheared_cylinders_maier_saupe
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_cylinders_maier_saupe_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_azimuthal_sheared_cylinders_maier_saupe
 *
* \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_cylinders_maier_saupe_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_azimuthal_sheared_cylinders_maier_saupe ################ */

/* ################ start ff_azimuthal_sheared_cylinders_heavyside ################ */
/**
 * \defgroup ff_azimuthal_sheared_cylinders_heavyside Sheared Cylinders (Heavyside) azimuthal
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of Sheared Cylinders (Heavyside) azimuthal function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>cylinder length</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b Q</td>
 *       <td>scattering vector</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width parameter of lognormal size distribution of radii</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>orientation distribution parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>not used</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_azimuthal_sheared_cylinders_heavyside
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_cylinders_heavyside(scalar q, sasfit_param * p);

/**
 * \ingroup ff_azimuthal_sheared_cylinders_heavyside
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_cylinders_heavyside_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_azimuthal_sheared_cylinders_heavyside
 *
* \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_cylinders_heavyside_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_azimuthal_sheared_cylinders_heavyside ################ */

/* ################ start ff_azimuthal_sheared_cylinders_boltzmann ################ */
/**
 * \defgroup ff_azimuthal_sheared_cylinders_boltzmann Sheared Cylinders (Boltzmann) azimuthal
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of Sheared Cylinders (Boltzmann) azimuthal function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>cylinder length</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b Q</td>
 *       <td>scattering vector</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width parameter of lognormal size distribution of radii</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>orientation distribution parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>not used</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_azimuthal_sheared_cylinders_boltzmann
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_cylinders_boltzmann(scalar q, sasfit_param * p);

/**
 * \ingroup ff_azimuthal_sheared_cylinders_boltzmann
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_cylinders_boltzmann_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_azimuthal_sheared_cylinders_boltzmann
 *
* \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_cylinders_boltzmann_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_azimuthal_sheared_cylinders_boltzmann ################ */

/* ################ start ff_azimuthal_sheared_cylinders_gauss ################ */
/**
 * \defgroup ff_azimuthal_sheared_cylinders_gauss Sheared Cylinders (Gauss) azimuthal
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of Sheared Cylinders (Gauss) azimuthal function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>cylinder length</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b Q</td>
 *       <td>scattering vector</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width parameter of lognormal size distribution of radii</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>orientation distribution parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>not used</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_azimuthal_sheared_cylinders_gauss
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_cylinders_gauss(scalar q, sasfit_param * p);

/**
 * \ingroup ff_azimuthal_sheared_cylinders_gauss
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_cylinders_gauss_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_azimuthal_sheared_cylinders_gauss
 *
* \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_cylinders_gauss_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_azimuthal_sheared_cylinders_gauss ################ */

/* ################ start ff_azimuthal_sheared_cylinders_hayterpenfold ################ */
/**
 * \defgroup ff_azimuthal_sheared_cylinders_hayterpenfold Sheared Cylinders (HayterPenfold) azimuthal
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of Sheared Cylinders (HayterPenfold) azimuthal function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>cylinder length</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b Q</td>
 *       <td>scattering vector</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width parameter of lognormal size distribution of radii</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>orientation distribution parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>not used</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_azimuthal_sheared_cylinders_hayterpenfold
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_cylinders_hayterpenfold(scalar q, sasfit_param * p);

/**
 * \ingroup ff_azimuthal_sheared_cylinders_hayterpenfold
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_cylinders_hayterpenfold_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_azimuthal_sheared_cylinders_hayterpenfold
 *
* \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_cylinders_hayterpenfold_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_azimuthal_sheared_cylinders_hayterpenfold ################ */

/* ################ start ff_azimuthal_sheared_cylinders_onsager ################ */
/**
 * \defgroup ff_azimuthal_sheared_cylinders_onsager Sheared Cylinders (Onsager) azimuthal
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of Sheared Cylinders (Onsager) azimuthal function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>cylinder length</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b Q</td>
 *       <td>scattering vector</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width parameter of lognormal size distribution of radii</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>orientation distribution parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>not used</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_azimuthal_sheared_cylinders_onsager
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_cylinders_onsager(scalar q, sasfit_param * p);

/**
 * \ingroup ff_azimuthal_sheared_cylinders_onsager
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_cylinders_onsager_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_azimuthal_sheared_cylinders_onsager
 *
* \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_cylinders_onsager_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_azimuthal_sheared_cylinders_onsager ################ */


/* ################ start ff_azimuthal_sheared_spheroid_maier_saupe ################ */
/**
 * \defgroup ff_azimuthal_sheared_spheroid_maier_saupe Sheared Spheroids (Maier-Saupe) azimuthal
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of Sheared Spheroid (Maier-Saupe) azimuthal function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_equatorial</td>
 *       <td>equatorial semi-axes</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b R_polar</td>
 *       <td>polar semin-axis</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b Q</td>
 *       <td>scattering vector</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width parameter of lognormal size distribution of radii</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>orientation distribution parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>not used</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_azimuthal_sheared_spheroid_maier_saupe
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_spheroid_maier_saupe(scalar q, sasfit_param * p);

/**
 * \ingroup ff_azimuthal_sheared_spheroid_maier_saupe
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_spheroid_maier_saupe_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_azimuthal_sheared_spheroid_maier_saupe
 *
* \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_spheroid_maier_saupe_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_azimuthal_sheared_spheroid_maier_saupe ################ */

/* ################ start ff_azimuthal_sheared_spheroid_heavyside ################ */
/**
 * \defgroup ff_azimuthal_sheared_spheroid_heavyside Sheared Spheroid (Heavyside) azimuthal
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of Sheared Spheroid (Heavyside) azimuthal function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_equatorial</td>
 *       <td>equatorial semi-axes</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b R_polar</td>
 *       <td>polar semin-axis</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b Q</td>
 *       <td>scattering vector</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width parameter of lognormal size distribution of radii</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>orientation distribution parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>not used</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_azimuthal_sheared_spheroid_heavyside
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_spheroid_heavyside(scalar q, sasfit_param * p);

/**
 * \ingroup ff_azimuthal_sheared_spheroid_heavyside
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_spheroid_heavyside_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_azimuthal_sheared_spheroid_heavyside
 *
* \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_spheroid_heavyside_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_azimuthal_sheared_spheroid_heavyside ################ */

/* ################ start ff_azimuthal_sheared_spheroid_boltzmann ################ */
/**
 * \defgroup ff_azimuthal_sheared_spheroid_boltzmann Sheared Spheroid (Boltzmann) azimuthal
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of Sheared Spheroid (Boltzmann) azimuthal function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_equatorial</td>
 *       <td>equatorial semi-axes</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b R_polar</td>
 *       <td>polar semin-axis</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b Q</td>
 *       <td>scattering vector</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width parameter of lognormal size distribution of radii</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>orientation distribution parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>not used</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_azimuthal_sheared_spheroid_boltzmann
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_spheroid_boltzmann(scalar q, sasfit_param * p);

/**
 * \ingroup ff_azimuthal_sheared_spheroid_boltzmann
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_spheroid_boltzmann_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_azimuthal_sheared_spheroid_boltzmann
 *
* \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_spheroid_boltzmann_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_azimuthal_sheared_spheroid_boltzmann ################ */

/* ################ start ff_azimuthal_sheared_spheroid_gauss ################ */
/**
 * \defgroup ff_azimuthal_sheared_spheroid_gauss Sheared Spheroid (Gauss) azimuthal
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of Sheared Spheroid (Gauss) azimuthal function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_equatorial</td>
 *       <td>equatorial semi-axes</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b R_polar</td>
 *       <td>polar semin-axis</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b Q</td>
 *       <td>scattering vector</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width parameter of lognormal size distribution of radii</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>orientation distribution parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>not used</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_azimuthal_sheared_spheroid_gauss
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_spheroid_gauss(scalar q, sasfit_param * p);

/**
 * \ingroup ff_azimuthal_sheared_spheroid_gauss
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_spheroid_gauss_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_azimuthal_sheared_spheroid_gauss
 *
* \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_spheroid_gauss_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_azimuthal_sheared_spheroid_gauss ################ */

/* ################ start ff_azimuthal_sheared_spheroid_hayterpenfold ################ */
/**
 * \defgroup ff_azimuthal_sheared_spheroid_hayterpenfold Sheared Spheroid (HayterPenfold) azimuthal
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of Sheared Spheroid (HayterPenfold) azimuthal function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_equatorial</td>
 *       <td>equatorial semi-axes</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b R_polar</td>
 *       <td>polar semin-axis</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b Q</td>
 *       <td>scattering vector</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width parameter of lognormal size distribution of radii</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>orientation distribution parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>not used</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_azimuthal_sheared_spheroid_hayterpenfold
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_spheroid_hayterpenfold(scalar q, sasfit_param * p);

/**
 * \ingroup ff_azimuthal_sheared_spheroid_hayterpenfold
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_spheroid_hayterpenfold_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_azimuthal_sheared_spheroid_hayterpenfold
 *
* \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_spheroid_hayterpenfold_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_azimuthal_sheared_spheroid_hayterpenfold ################ */

/* ################ start ff_azimuthal_sheared_spheroid_onsager ################ */
/**
 * \defgroup ff_azimuthal_sheared_spheroid_onsager Sheared Spheroid (Onsager) azimuthal
 * \ingroup ff_azimuthal
 *
 * \brief \<some brief description of Sheared Spheroid (Onsager) azimuthal function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_equatorial</td>
 *       <td>equatorial semi-axes</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b R_polar</td>
 *       <td>polar semin-axis</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b Q</td>
 *       <td>scattering vector</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width parameter of lognormal size distribution of radii</td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td>orientation distribution parameter</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>not used</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_azimuthal_sheared_cylinders_onsager
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_spheroid_onsager(scalar q, sasfit_param * p);

/**
 * \ingroup ff_azimuthal_sheared_spheroid_onsager
 *
 * \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_spheroid_onsager_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_azimuthal_sheared_cylinders_onsager
 *
* \sa sasfit_azimuthal.h, ff_azimuthal
 */
sasfit_azimuthal_DLLEXP scalar sasfit_ff_azimuthal_sheared_spheroid_onsager_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_azimuthal_sheared_spheroid_onsager ################ */


#endif // this file

