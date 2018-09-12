/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_POLYMER_STARS_H
#define SASFIT_PLUGIN_POLYMER_STARS_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_polymer_stars.h
 * Public available functions and descriptions of the polymer_stars plugin.
 */

/**
 * \def sasfit_polymer_stars_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_polymer_stars_EXPORTS)
	#ifdef sasfit_polymer_stars_DLLEXP
	#undef sasfit_polymer_stars_DLLEXP
	#endif
	#define sasfit_polymer_stars_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_polymer_stars_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_polymer_stars_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_benoitstar ################ */
/**
 * \defgroup ff_benoitstar BenoitStar
 * \ingroup ff_polymer_stars
 *
 * \brief \<some brief description of BenoitStar function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b f</td>
 *       <td>number of branches</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_benoitstar
 *
 * \sa sasfit_polymer_stars.h, ff_polymer_stars
 */
sasfit_polymer_stars_DLLEXP scalar sasfit_ff_benoitstar(scalar q, sasfit_param * p);

/**
 * \ingroup ff_benoitstar
 *
 * \sa sasfit_polymer_stars.h, ff_polymer_stars
 */
sasfit_polymer_stars_DLLEXP scalar sasfit_ff_benoitstar_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_benoitstar
 *
 * \sa sasfit_polymer_stars.h, ff_polymer_stars
 */
sasfit_polymer_stars_DLLEXP scalar sasfit_ff_benoitstar_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_benoitstar ################ */

/* ################ start ff_polydispersestar ################ */
/**
 * \defgroup ff_polydispersestar PolydisperseStar
 * \ingroup ff_polymer_stars
 *
 * \brief \<some brief description of PolydisperseStar function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I0</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b f</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_polydispersestar
 *
 * \sa sasfit_polymer_stars.h, ff_polymer_stars
 */
sasfit_polymer_stars_DLLEXP scalar sasfit_ff_polydispersestar(scalar q, sasfit_param * p);

/**
 * \ingroup ff_polydispersestar
 *
 * \sa sasfit_polymer_stars.h, ff_polymer_stars
 */
sasfit_polymer_stars_DLLEXP scalar sasfit_ff_polydispersestar_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_polydispersestar
 *
 * \sa sasfit_polymer_stars.h, ff_polymer_stars
 */
sasfit_polymer_stars_DLLEXP scalar sasfit_ff_polydispersestar_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_polydispersestar ################ */

/* ################ start ff_gauss_exvol_star ################ */
/**
 * \defgroup ff_gauss_exvol_star star of polymer arms with excl. vol.
 * \ingroup ff_polymer_stars
 *
 * \brief \<some brief description of "star of polymer arms with excl. vol." function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration</td>
 *      </tr><tr>
 *       <td>\b nu</td>
 *       <td>Flory exponent</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b f</td>
 *       <td>number of branches</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_gauss_exvol_star
 *
 * \sa sasfit_polymer_stars.h, ff_polymer_stars
 */
sasfit_polymer_stars_DLLEXP scalar sasfit_ff_gauss_exvol_star(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gauss_exvol_star
 *
 * \sa sasfit_polymer_stars.h, ff_polymer_stars
 */
sasfit_polymer_stars_DLLEXP scalar sasfit_ff_gauss_exvol_star_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_gauss_exvol_star
 *
 * \sa sasfit_polymer_stars.h, ff_polymer_stars
 */
sasfit_polymer_stars_DLLEXP scalar sasfit_ff_gauss_exvol_star_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_gauss_exvol_star ################ */


/* ################ start ff_dozierstar ################ */
/**
 * \defgroup ff_dozierstar DozierStar
 * \ingroup ff_polymer_stars
 *
 * \brief \<some brief description of DozierStar function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>scaling factor between overall and blob scattering</td>
 *      </tr><tr>
 *       <td>\b nu</td>
 *       <td>nu: Flory exponent g(r) = r^(1/nu-3), 0.6 in good solvent, .5 in theta solvent</td>
 *      </tr><tr>
 *       <td>\b f</td>
 *       <td>f: number of arms, correlation length xi inside star or equivalently the radius of the outermost blobs can be calculated by xi=2Rg/sqrt(f)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_dozierstar
 *
 * \sa sasfit_polymer_stars.h, ff_polymer_stars
 */
sasfit_polymer_stars_DLLEXP scalar sasfit_ff_dozierstar(scalar q, sasfit_param * p);

/**
 * \ingroup ff_dozierstar
 *
 * \sa sasfit_polymer_stars.h, ff_polymer_stars
 */
sasfit_polymer_stars_DLLEXP scalar sasfit_ff_dozierstar_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_dozierstar
 *
 * \sa sasfit_polymer_stars.h, ff_polymer_stars
 */
sasfit_polymer_stars_DLLEXP scalar sasfit_ff_dozierstar_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_dozierstar ################ */

/* ################ start ff_dozierstar2 ################ */
/**
 * \defgroup ff_dozierstar2 DozierStar2
 * \ingroup ff_polymer_stars
 *
 * \brief \<some brief description of DozierStar2 function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>radius of gyration</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b nu</td>
 *       <td>nu: Flory exponent g(r) = r^(1/nu-3), 0.6 in good solvent, .5 in theta solvent</td>
 *      </tr><tr>
 *       <td>\b f</td>
 *       <td>f: number of arms, correlation length xi inside star or equivalently the radius of the outermost blobs can be calculated by xi=2Rg/sqrt(f)</td>
 *      </tr><tr>
 *      </tr></table>
 */

/**
 * \ingroup ff_dozierstar2
 *
 * \sa sasfit_polymer_stars.h, ff_polymer_stars
 */
sasfit_polymer_stars_DLLEXP scalar sasfit_ff_dozierstar2(scalar q, sasfit_param * p);

/**
 * \ingroup ff_dozierstar2
 *
 * \sa sasfit_polymer_stars.h, ff_polymer_stars
 */
sasfit_polymer_stars_DLLEXP scalar sasfit_ff_dozierstar2_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_dozierstar2
 *
 * \sa sasfit_polymer_stars.h, ff_polymer_stars
 */
sasfit_polymer_stars_DLLEXP scalar sasfit_ff_dozierstar2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_dozierstar2 ################ */

/* ################ start ff_broken_rods_star ################ */
/**
 * \defgroup ff_broken_rods_star star polymer with arms of rigid rods
 * \ingroup ff_polymer_stars
 *
 * \brief \<some brief description of star polymer with arms of rigid rods\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>length of a single stiff chain in the star</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b f</td>
 *       <td>f: number of arms</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_broken_rods_star
 *
 * \sa sasfit_polymer_stars.h, ff_polymer_stars
 */
sasfit_polymer_stars_DLLEXP scalar sasfit_ff_broken_rods_star(scalar q, sasfit_param * p);

/**
 * \ingroup ff_broken_rods_star
 *
 * \sa sasfit_polymer_stars.h, ff_polymer_stars
 */
sasfit_polymer_stars_DLLEXP scalar sasfit_ff_broken_rods_star_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_broken_rods_star
 *
 * \sa sasfit_polymer_stars.h, ff_polymer_stars
 */
sasfit_polymer_stars_DLLEXP scalar sasfit_ff_broken_rods_star_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_broken_rods_star ################ */


/* ################ start ff_broken_worms_star ################ */
/**
 * \defgroup ff_broken_worms_star star with semi-flexible arms
 * \ingroup ff_polymer_stars
 *
 * \brief \<some brief description of star with semi-flexible arms function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>contour length of a single arm</td>
 *      </tr><tr>
 *       <td>\b lb</td>
 *       <td>Kuhn length</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b f</td>
 *       <td>f: number of arms</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_broken_worms_star
 *
 * \sa sasfit_polymer_stars.h, ff_polymer_stars
 */
sasfit_polymer_stars_DLLEXP scalar sasfit_ff_broken_worms_star(scalar q, sasfit_param * p);

/**
 * \ingroup ff_broken_worms_star
 *
 * \sa sasfit_polymer_stars.h, ff_polymer_stars
 */
sasfit_polymer_stars_DLLEXP scalar sasfit_ff_broken_worms_star_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_broken_worms_star
 *
 * \sa sasfit_polymer_stars.h, ff_polymer_stars
 */
sasfit_polymer_stars_DLLEXP scalar sasfit_ff_broken_worms_star_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_broken_worms_star ################ */

#endif // this file

