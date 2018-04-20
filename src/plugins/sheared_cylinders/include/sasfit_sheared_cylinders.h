/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_SHEARED_CYLINDERS_H
#define SASFIT_PLUGIN_SHEARED_CYLINDERS_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_sheared_cylinders.h
 * Public available functions and descriptions of the sheared_cylinders plugin.
 */

/**
 * \def sasfit_sheared_cylinders_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_sheared_cylinders_EXPORTS)
	#ifdef sasfit_sheared_cylinders_DLLEXP
	#undef sasfit_sheared_cylinders_DLLEXP
	#endif
	#define sasfit_sheared_cylinders_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_sheared_cylinders_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_sheared_cylinders_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_sheared_cylinders__maier_saupe_ ################ */
/**
 * \defgroup ff_sheared_cylinders__maier_saupe_ Sheared Cylinders (Maier-Saupe)
 * \ingroup ff_plugins_cylindrical_obj
 *
 * \brief \<some brief description of Sheared Cylinders (Maier-Saupe) function\>
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
 *       <td>\b psi</td>
 *       <td>direction of Q in degree</td>
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
 * \ingroup ff_sheared_cylinders__maier_saupe_
 *
 * \sa sasfit_sheared_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_sheared_cylinders_DLLEXP scalar sasfit_ff_sheared_cylinders__maier_saupe_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sheared_cylinders__maier_saupe_
 *
 * \sa sasfit_sheared_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_sheared_cylinders_DLLEXP scalar sasfit_ff_sheared_cylinders__maier_saupe__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sheared_cylinders__maier_saupe_
 *
 * \sa sasfit_sheared_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_sheared_cylinders_DLLEXP scalar sasfit_ff_sheared_cylinders__maier_saupe__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_sheared_cylinders__maier_saupe_ ################ */

/* ################ start ff_sheared_cylinders__heavyside_ ################ */
/**
 * \defgroup ff_sheared_cylinders__heavyside_ Sheared Cylinders (Heavyside)
 * \ingroup ff_plugins_cylindrical_obj
 *
 * \brief \<some brief description of Sheared Cylinders (Heavyside) function\>
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
 *       <td>\b psi</td>
 *       <td>direction of Q in degree</td>
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
 * \ingroup ff_sheared_cylinders__heavyside_
 *
 * \sa sasfit_sheared_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_sheared_cylinders_DLLEXP scalar sasfit_ff_sheared_cylinders__heavyside_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sheared_cylinders__heavyside_
 *
 * \sa sasfit_sheared_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_sheared_cylinders_DLLEXP scalar sasfit_ff_sheared_cylinders__heavyside__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sheared_cylinders__heavyside_
 *
 * \sa sasfit_sheared_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_sheared_cylinders_DLLEXP scalar sasfit_ff_sheared_cylinders__heavyside__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_sheared_cylinders__heavyside_ ################ */

/* ################ start ff_sheared_cylinders__boltzmann_ ################ */
/**
 * \defgroup ff_sheared_cylinders__boltzmann_ Sheared Cylinders (Boltzmann)
 * \ingroup ff_plugins_cylindrical_obj
 *
 * \brief \<some brief description of Sheared Cylinders (Boltzmann) function\>
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
 *       <td>\b psi</td>
 *       <td>direction of Q in degree</td>
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
 * \ingroup ff_sheared_cylinders__boltzmann_
 *
 * \sa sasfit_sheared_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_sheared_cylinders_DLLEXP scalar sasfit_ff_sheared_cylinders__boltzmann_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sheared_cylinders__boltzmann_
 *
 * \sa sasfit_sheared_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_sheared_cylinders_DLLEXP scalar sasfit_ff_sheared_cylinders__boltzmann__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sheared_cylinders__boltzmann_
 *
 * \sa sasfit_sheared_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_sheared_cylinders_DLLEXP scalar sasfit_ff_sheared_cylinders__boltzmann__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_sheard_cylinder__boltzmann_ ################ */

/* ################ start ff_sheared_cylinders__gauss_ ################ */
/**
 * \defgroup ff_sheared_cylinders__gauss_ Sheared Cylinders (Gauss)
 * \ingroup ff_plugins_cylindrical_obj
 *
 * \brief \<some brief description of Sheared Cylinders (Gauss) function\>
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
 *       <td>\b psi</td>
 *       <td>direction of Q in degree</td>
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
 * \ingroup ff_sheared_cylinders__gauss_
 *
 * \sa sasfit_sheared_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_sheared_cylinders_DLLEXP scalar sasfit_ff_sheared_cylinders__gauss_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sheared_cylinders__gauss_
 *
 * \sa sasfit_sheared_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_sheared_cylinders_DLLEXP scalar sasfit_ff_sheared_cylinders__gauss__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sheared_cylinders__gauss_
 *
 * \sa sasfit_sheared_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_sheared_cylinders_DLLEXP scalar sasfit_ff_sheared_cylinders__gauss__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_sheared_cylinders__gauss_ ################ */

/* ################ start ff_sheared_cylinders__hayterpenfold_ ################ */
/**
 * \defgroup ff_sheared_cylinders__hayterpenfold_ Sheared Cylinders (HayterPenfold)
 * \ingroup ff_plugins_cylindrical_obj
 *
 * \brief \<some brief description of Sheared Cylinders (HayterPenfold) function\>
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
 *       <td>\b psi</td>
 *       <td>direction of Q in degree</td>
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
 * \ingroup ff_sheared_cylinders__hayterpenfold_
 *
 * \sa sasfit_sheared_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_sheared_cylinders_DLLEXP scalar sasfit_ff_sheared_cylinders__hayterpenfold_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sheared_cylinders__hayterpenfold_
 *
 * \sa sasfit_sheared_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_sheared_cylinders_DLLEXP scalar sasfit_ff_sheared_cylinders__hayterpenfold__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sheared_cylinders__hayterpenfold_
 *
 * \sa sasfit_sheared_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_sheared_cylinders_DLLEXP scalar sasfit_ff_sheared_cylinders__hayterpenfold__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_sheared_cylinders__hayterpenfold_ ################ */

/* ################ start ff_sheared_cylinders__onsager_ ################ */
/**
 * \defgroup ff_sheared_cylinders__onsager_ Sheared Cylinders (Onsager)
 * \ingroup ff_plugins_cylindrical_obj
 *
 * \brief \<some brief description of Sheared Cylinders (Onsager) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b DR</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b kappa</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_sheared_cylinders__onsager_
 *
 * \sa sasfit_sheared_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_sheared_cylinders_DLLEXP scalar sasfit_ff_sheared_cylinders__onsager_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sheared_cylinders__onsager_
 *
 * \sa sasfit_sheared_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_sheared_cylinders_DLLEXP scalar sasfit_ff_sheared_cylinders__onsager__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sheared_cylinders__onsager_
 *
 * \sa sasfit_sheared_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_sheared_cylinders_DLLEXP scalar sasfit_ff_sheared_cylinders__onsager__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_sheared_cylinders__onsager_ ################ */

/* ################ start ff_sheared_cylinders__legendre_ ################ */
/**
 * \defgroup ff_sheared_cylinders__legendre_ Sheared Cylinders (Legendre)
 * \ingroup ff_plugins_cylindrical_obj
 *
 * \brief \<some brief description of Sheared Cylinders (Legendre) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b DR</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b psi</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b P2</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b P4</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b P6</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_sheared_cylinders__legendre_
 *
 * \sa sasfit_sheared_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_sheared_cylinders_DLLEXP scalar sasfit_ff_sheared_cylinders__legendre_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sheared_cylinders__legendre_
 *
 * \sa sasfit_sheared_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_sheared_cylinders_DLLEXP scalar sasfit_ff_sheared_cylinders__legendre__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_sheared_cylinders__legendre_
 *
 * \sa sasfit_sheared_cylinders.h, ff_plugins_cylindrical_obj
 */
sasfit_sheared_cylinders_DLLEXP scalar sasfit_ff_sheared_cylinders__legendre__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_sheared_cylinders__legendre_ ################ */


#endif // this file

