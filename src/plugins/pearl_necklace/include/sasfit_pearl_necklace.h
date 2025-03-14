/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_PEARL_NECKLACE_H
#define SASFIT_PLUGIN_PEARL_NECKLACE_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_pearl_necklace.h
 * Public available functions and descriptions of the pearl_necklace plugin.
 */

/**
 * \def sasfit_pearl_necklace_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_pearl_necklace_EXPORTS)
	#ifdef sasfit_pearl_necklace_DLLEXP
	#undef sasfit_pearl_necklace_DLLEXP
	#endif
	#define sasfit_pearl_necklace_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_pearl_necklace_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_pearl_necklace_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_pearl_necklace ################ */
/**
 * \defgroup ff_pearl_necklace Pearl Necklace
 * \ingroup ff_devel
 *
 * \brief \<some brief description of Pearl Necklace function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius of pearl particle</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b l</td>
 *       <td>length of monomers (rods)</td>
 *      </tr><tr>
 *       <td>\b beta_r</td>
 *       <td>excess scattering length of monomers (rods)</td>
 *      </tr><tr>
 *       <td>\b beta_s</td>
 *       <td>excess scattering length of spheres</td>
 *      </tr><tr>
 *       <td>\b Np</td>
 *       <td>number of pearls</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_pearl_necklace
 *
 * \sa sasfit_pearl_necklace.h, ff_devel
 */
sasfit_pearl_necklace_DLLEXP scalar sasfit_ff_pearl_necklace(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pearl_necklace
 *
 * \sa sasfit_pearl_necklace.h, ff_devel
 */
sasfit_pearl_necklace_DLLEXP scalar sasfit_ff_pearl_necklace_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pearl_necklace
 *
 * \sa sasfit_pearl_necklace.h, ff_devel
 */
sasfit_pearl_necklace_DLLEXP scalar sasfit_ff_pearl_necklace_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_pearl_necklace ################ */


/* ################ start ff_pearl_necklace_ext ################ */
/**
 * \defgroup ff_pearl_necklace_ext Pearl Necklace (ext)
 * \ingroup ff_devel
 *
 * \brief \<some brief description of Pearl Necklace (ext) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius of pearl particle</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b l</td>
 *       <td>length of monomers (rods)</td>
 *      </tr><tr>
 *       <td>\b beta_r</td>
 *       <td>excess scattering length of monomers (rods)</td>
 *      </tr><tr>
 *       <td>\b beta_s</td>
 *       <td>excess scattering length of spheres</td>
 *      </tr><tr>
 *       <td>\b Np</td>
 *       <td>number of pearls</td>
 *      </tr><tr>
 *       <td>\b g</td>
 *       <td>number of monomers between pearls</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_pearl_necklace_ext
 *
 * \sa sasfit_pearl_necklace.h, ff_devel
 */
sasfit_pearl_necklace_DLLEXP scalar sasfit_ff_pearl_necklace_ext(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pearl_necklace_ext
 *
 * \sa sasfit_pearl_necklace.h, ff_devel
 */
sasfit_pearl_necklace_DLLEXP scalar sasfit_ff_pearl_necklace_ext_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pearl_necklace_ext
 *
 * \sa sasfit_pearl_necklace.h, ff_devel
 */
sasfit_pearl_necklace_DLLEXP scalar sasfit_ff_pearl_necklace_ext_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_pearl_necklace_ext ################ */

/* ################ start ff_pearl_necklace_mod ################ */
/**
 * \defgroup ff_pearl_necklace_mod Pearl Necklace (mod)
 * \ingroup ff_devel
 *
 * \brief \<some brief description of Pearl Necklace (mod) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius of pearl particle</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b l</td>
 *       <td>length of monomers (rods)</td>
 *      </tr><tr>
 *       <td>\b beta_r</td>
 *       <td>excess scattering length of monomers (rods)</td>
 *      </tr><tr>
 *       <td>\b beta_s</td>
 *       <td>excess scattering length of spheres</td>
 *      </tr><tr>
 *       <td>\b Np</td>
 *       <td>number of pearls</td>
 *      </tr><tr>
 *       <td>\b g</td>
 *       <td>number of monomers between pearls</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_pearl_necklace_mod
 *
 * \sa sasfit_pearl_necklace.h, ff_devel
 */
sasfit_pearl_necklace_DLLEXP scalar sasfit_ff_pearl_necklace_mod(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pearl_necklace_mod
 *
 * \sa sasfit_pearl_necklace.h, ff_devel
 */
sasfit_pearl_necklace_DLLEXP scalar sasfit_ff_pearl_necklace_mod_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_pearl_necklace_mod
 *
 * \sa sasfit_pearl_necklace.h, ff_devel
 */
sasfit_pearl_necklace_DLLEXP scalar sasfit_ff_pearl_necklace_mod_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_pearl_necklace_mod ################ */

#endif // this file

