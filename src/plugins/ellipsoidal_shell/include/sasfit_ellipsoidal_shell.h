/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_ELLIPSOIDAL_SHELL_H
#define SASFIT_PLUGIN_ELLIPSOIDAL_SHELL_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_ellipsoidal_shell.h
 * Public available functions and descriptions of the ellipsoidal_shell plugin.
 */

/**
 * \def sasfit_ellipsoidal_shell_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_ellipsoidal_shell_EXPORTS)
	#ifdef sasfit_ellipsoidal_shell_DLLEXP
	#undef sasfit_ellipsoidal_shell_DLLEXP
	#endif
	#define sasfit_ellipsoidal_shell_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_ellipsoidal_shell_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_ellipsoidal_shell_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_ellipsoidal_shell_0 ################ */
/**
 * \defgroup ff_ellipsoidal_shell_0 ellipsoidal shell
 * \ingroup ff_spheroid_obj
 *
 * \brief \<some brief description of ellipsoidal shell function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *       <td>\b R_polar</td>
 *       <td>length of polar semi axis</td>
 *      </tr><tr>
 *       <td>\b R_equatorial</td>
 *       <td>length of equatorial semi axes</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of compact core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of compact shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b x_core</td>
 *       <td>volume fraction of solvent in core</td>
 *      </tr><tr>
 *       <td>\b x_shell</td>
 *       <td>volume fraction of solvent in shell</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ellipsoidal_shell_0
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_ellipsoidal_shell_0(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellipsoidal_shell_0
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_ellipsoidal_shell_0_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellipsoidal_shell_0
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_ellipsoidal_shell_0_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ellipsoidal_shell_0 ################ */

/* ################ start ff_ellipsoidal_shell_0t ################ */
/**
 * \defgroup ff_ellipsoidal_shell_0t ellipsoidal shell (t)
 * \ingroup ff_spheroid_obj
 *
 * \brief \<some brief description of ellipsoidal shell 0t function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *       <td>\b R_polar</td>
 *       <td>length of polar semi axis</td>
 *      </tr><tr>
 *       <td>\b R_equatorial</td>
 *       <td>length of equatorial semi axes</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width of LogNormal size distribution</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of compact core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of compact shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b x_core</td>
 *       <td>volume fraction of solvent in core</td>
 *      </tr><tr>
 *       <td>\b x_shell</td>
 *       <td>volume fraction of solvent in shell</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ellipsoidal_shell_0t
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_ellipsoidal_shell_0t(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellipsoidal_shell_0t
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_ellipsoidal_shell_0t_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellipsoidal_shell_0t
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_ellipsoidal_shell_0t_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ellipsoidal_shell_0t ################ */

/* ################ start ff_ellipsoidal_shell_rp ################ */
/**
 * \defgroup ff_ellipsoidal_shell_rp ellipsoidal shell (Rp)
 * \ingroup ff_spheroid_obj
 *
 * \brief \<some brief description of ellipsoidal shell (Rp) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *       <td>\b R_polar</td>
 *       <td>length of polar semi axis</td>
 *      </tr><tr>
 *       <td>\b R_equatorial</td>
 *       <td>length of equatorial semi axes</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width of LogNormal size distribution</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of compact core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of compact shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b x_core</td>
 *       <td>volume fraction of solvent in core</td>
 *      </tr><tr>
 *       <td>\b x_shell</td>
 *       <td>volume fraction of solvent in shell</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ellipsoidal_shell_rp
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_ellipsoidal_shell_rp(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellipsoidal_shell_rp
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_ellipsoidal_shell_rp_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellipsoidal_shell_rp
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_ellipsoidal_shell_rp_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ellipsoidal_shell_rp ################ */

/* ################ start ff_ellipsoidal_shell_rp_t ################ */
/**
 * \defgroup ff_ellipsoidal_shell_rp_t ellipsoidal shell (Rp t)
 * \ingroup ff_spheroid_obj
 *
 * \brief \<some brief description of ellipsoidal shell (Rp t) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *       <td>\b R_polar</td>
 *       <td>length of polar semi axis</td>
 *      </tr><tr>
 *       <td>\b R_equatorial</td>
 *       <td>length of equatorial semi axes</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width of LogNormal size distribution</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of compact core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of compact shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b x_core</td>
 *       <td>volume fraction of solvent in core</td>
 *      </tr><tr>
 *       <td>\b x_shell</td>
 *       <td>volume fraction of solvent in shell</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ellipsoidal_shell_rp_t
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_ellipsoidal_shell_rp_t(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellipsoidal_shell_rp_t
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_ellipsoidal_shell_rp_t_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellipsoidal_shell_rp_t
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_ellipsoidal_shell_rp_t_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ellipsoidal_shell_rp_t ################ */

/* ################ start ff_ellipsoidal_shell_re ################ */
/**
 * \defgroup ff_ellipsoidal_shell_re ellipsoidal shell (Re)
 * \ingroup ff_spheroid_obj
 *
 * \brief \<some brief description of ellipsoidal shell (Re) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *       <td>\b R_polar</td>
 *       <td>length of polar semi axis</td>
 *      </tr><tr>
 *       <td>\b R_equatorial</td>
 *       <td>length of equatorial semi axes</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width of LogNormal size distribution</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of compact core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of compact shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b x_core</td>
 *       <td>volume fraction of solvent in core</td>
 *      </tr><tr>
 *       <td>\b x_shell</td>
 *       <td>volume fraction of solvent in shell</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ellipsoidal_shell_re
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_ellipsoidal_shell_re(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellipsoidal_shell_re
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_ellipsoidal_shell_re_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellipsoidal_shell_re
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_ellipsoidal_shell_re_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ellipsoidal_shell_re ################ */

/* ################ start ff_ellipsoidal_shell_re_t ################ */
/**
 * \defgroup ff_ellipsoidal_shell_re_t ellipsoidal shell (Re t)
 * \ingroup ff_spheroid_obj
 *
 * \brief \<some brief description of ellipsoidal shell (Re t) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *       <td>\b R_polar</td>
 *       <td>length of polar semi axis</td>
 *      </tr><tr>
 *       <td>\b R_equatorial</td>
 *       <td>length of equatorial semi axes</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width of LogNormal size distribution</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of compact core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of compact shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b x_core</td>
 *       <td>volume fraction of solvent in core</td>
 *      </tr><tr>
 *       <td>\b x_shell</td>
 *       <td>volume fraction of solvent in shell</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ellipsoidal_shell_re_t
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_ellipsoidal_shell_re_t(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellipsoidal_shell_re_t
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_ellipsoidal_shell_re_t_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellipsoidal_shell_re_t
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_ellipsoidal_shell_re_t_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ellipsoidal_shell_re_t ################ */

/* ################ start ff_ellipsoidal_shell_2 ################ */
/**
 * \defgroup ff_ellipsoidal_shell_2 ellipsoidal shell (Re Rp)
 * \ingroup ff_spheroid_obj
 *
 * \brief \<some brief description of ellipsoidal shell (Re Rp) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *       <td>\b R_polar</td>
 *       <td>length of polar semi axis</td>
 *      </tr><tr>
 *       <td>\b R_equatorial</td>
 *       <td>length of equatorial semi axes</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width of LogNormal size distribution</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of compact core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of compact shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b x_core</td>
 *       <td>volume fraction of solvent in core</td>
 *      </tr><tr>
 *       <td>\b x_shell</td>
 *       <td>volume fraction of solvent in shell</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ellipsoidal_shell_2
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_ellipsoidal_shell_2(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellipsoidal_shell_2
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_ellipsoidal_shell_2_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellipsoidal_shell_2
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_ellipsoidal_shell_2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ellipsoidal_shell_2 ################ */

/* ################ start ff_ellipsoidal_shell_2t ################ */
/**
 * \defgroup ff_ellipsoidal_shell_2t ellipsoidal shell (Re Rp t)
 * \ingroup ff_spheroid_obj
 *
 * \brief \<some brief description of ellipsoidal shell (Re Rp t) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_polar</td>
 *       <td>length of polar semi axis</td>
 *      </tr><tr>
 *       <td>\b R_equatorial</td>
 *       <td>length of equatorial semi axes</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>width of LogNormal size distribution</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of compact core</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of compact shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b x_core</td>
 *       <td>volume fraction of solvent in core</td>
 *      </tr><tr>
 *       <td>\b x_shell</td>
 *       <td>volume fraction of solvent in shell</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ellipsoidal_shell_2t
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_ellipsoidal_shell_2t(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellipsoidal_shell_2t
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_ellipsoidal_shell_2t_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ellipsoidal_shell_2t
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_ellipsoidal_shell_2t_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ellipsoidal_shell_2t ################ */

/* ################ start ff_spheroid_R ################ */
/**
 * \defgroup ff_spheroid_R spheroid R
 * \ingroup ff_spheroid_obj
 *
 * \brief \<some brief description of spheroid R function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *       <td>\b R_polar</td>
 *       <td>length of polar semi axis</td>
 *      </tr><tr>
 *       <td>\b R_equatorial</td>
 *       <td>length of equatorial semi axes</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of spheroid</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_spheroid_R
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_spheroid_R(scalar q, sasfit_param * p);

/**
 * \ingroup ff_spheroid_R
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_spheroid_R_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_spheroid_R
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_spheroid_R_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_spheroid_R ################ */

/* ################ start ff_spheroid_V ################ */
/**
 * \defgroup ff_spheroid_V spheroid V
 * \ingroup ff_spheroid_obj
 *
 * \brief \<some brief description of spheroid V function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *       <td>\b V</td>
 *       <td>volume of ellipsoid</td>
 *      </tr><tr>
 *       <td>\b R_equatorial</td>
 *       <td>length of equatorial semi axes</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of spheroid</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_spheroid_V
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_spheroid_V(scalar q, sasfit_param * p);

/**
 * \ingroup ff_spheroid_V
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_spheroid_V_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_spheroid_V
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_spheroid_V_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_spheroid_V ################ */

/* ################ start ff_spheroid_nu ################ */
/**
 * \defgroup ff_spheroid_nu spheroid nu
 * \ingroup ff_spheroid_obj
 *
 * \brief \<some brief description of spheroid nu function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *       <td>\b nu</td>
 *       <td>nu assymetry factor,  nu*R_equatorial = length of polar semi axis</td>
 *      </tr><tr>
 *       <td>\b R_equatorial</td>
 *       <td>length of equatorial semi axes</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of spheroid</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_spheroid_nu
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_spheroid_nu(scalar q, sasfit_param * p);

/**
 * \ingroup ff_spheroid_nu
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_spheroid_nu_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_spheroid_nu
 *
 * \sa sasfit_ellipsoidal_shell.h, ff_spheroid_obj
 */
sasfit_ellipsoidal_shell_DLLEXP scalar sasfit_ff_spheroid_nu_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_spheroid_nu ################ */

#endif // this file

