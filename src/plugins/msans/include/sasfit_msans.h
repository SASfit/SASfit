/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_MSANS_H
#define SASFIT_PLUGIN_MSANS_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_msans.h
 * Public available functions and descriptions of the msans plugin.
 */

/**
 * \def sasfit_msans_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_msans_EXPORTS)
	#ifdef sasfit_msans_DLLEXP
	#undef sasfit_msans_DLLEXP
	#endif
	#define sasfit_msans_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_msans_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_msans_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_msas_spheres ################ */
/**
 * \defgroup ff_msas_spheres MSAS Spheres
 * \ingroup ff_msas
 *
 * \brief \<some brief description of MSAS Spheres function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius of spheres</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Sigma_t</td>
 *       <td>total scattering per volume</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>sample thickness in reciprocal units of the intensity, e.g. [cm]</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>wave length in reciprocal units of the scattering vector</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_msas_spheres
 *
 * \sa sasfit_msans.h, ff_msas
 */
sasfit_msans_DLLEXP scalar sasfit_ff_msas_spheres(scalar q, sasfit_param * p);

/**
 * \ingroup ff_msas_spheres
 *
 * \sa sasfit_msans.h, ff_msas
 */
sasfit_msans_DLLEXP scalar sasfit_ff_msas_spheres_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_msas_spheres
 *
 * \sa sasfit_msans.h, ff_msas
 */
sasfit_msans_DLLEXP scalar sasfit_ff_msas_spheres_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_msas_spheres ################ */

/* ################ start ff_msas_polydisp_spheres ################ */
/**
 * \defgroup ff_msas_polydisp_spheres MSAS polidisp. Spheres
 * \ingroup ff_msas
 *
 * \brief \<some brief description of MSAS polydisp. Spheres function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b mu</td>
 *       <td>mean radius of spheres</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width of the LogNorm size distribution</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Sigma_t</td>
 *       <td>total scattering per volume</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>sample thickness in reciprocal units of the intensity, e.g. [cm]</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>wave length in reciprocal units of the scattering vector</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_msas_polydisp_spheres
 *
 * \sa sasfit_msans.h, ff_msas
 */
sasfit_msans_DLLEXP scalar sasfit_ff_msas_polydisp_spheres(scalar q, sasfit_param * p);

/**
 * \ingroup ff_msas_polydisp_spheres
 *
 * \sa sasfit_msans.h, ff_msas
 */
sasfit_msans_DLLEXP scalar sasfit_ff_msas_polydisp_spheres_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_msas_polydisp_spheres
 *
 * \sa sasfit_msans.h, ff_msas
 */
sasfit_msans_DLLEXP scalar sasfit_ff_msas_polydisp_spheres_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_msas_polydisp_spheres ################ */

/* ################ start ff_msas_gdab ################ */
/**
 * \defgroup ff_msas_gdab MSAS gDAB
 * \ingroup ff_msas
 *
 * \brief \<some brief description of MSAS gDAB function\>
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
 *       <td>Hurst_exponeny</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Sigma_t</td>
 *       <td>total scattering per volume</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>sample thickness in reciprocal units of the intensity, e.g. [cm]</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>wave length in reciprocal units of the scattering vector</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_msas_gdab
 *
 * \sa sasfit_msans.h, ff_msas
 */
sasfit_msans_DLLEXP scalar sasfit_ff_msas_gdab(scalar q, sasfit_param * p);

/**
 * \ingroup ff_msas_gdab
 *
 * \sa sasfit_msans.h, ff_msas
 */
sasfit_msans_DLLEXP scalar sasfit_ff_msas_gdab_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_msas_gdab
 *
 * \sa sasfit_msans.h, ff_msas
 */
sasfit_msans_DLLEXP scalar sasfit_ff_msas_gdab_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_msas_gdab ################ */

/* ################ start ff_msas_ggc ################ */
/**
 * \defgroup ff_msas_ggc MSAS gGc
 * \ingroup ff_msas
 *
 * \brief \<some brief description of MSAS gGc function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration</td>
 *      </tr><tr>
 *       <td>\b nu</td>
 *       <td>Flory exponent</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b Sigma_t</td>
 *       <td>total scattering per volume</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>sample thickness in reciprocal units of the intensity, e.g. [cm]</td>
 *      </tr><tr>
 *       <td>\b lambda</td>
 *       <td>wave length in reciprocal units of the scattering vector</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_msas_ggc
 *
 * \sa sasfit_msans.h, ff_msas
 */
sasfit_msans_DLLEXP scalar sasfit_ff_msas_ggc(scalar q, sasfit_param * p);

/**
 * \ingroup ff_msas_ggc
 *
 * \sa sasfit_msans.h, ff_msas
 */
sasfit_msans_DLLEXP scalar sasfit_ff_msas_ggc_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_msas_ggc
 *
 * \sa sasfit_msans.h, ff_msas
 */
sasfit_msans_DLLEXP scalar sasfit_ff_msas_ggc_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_msas_ggc ################ */


#endif // this file

