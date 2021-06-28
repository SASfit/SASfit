/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_MASS_FRACTAL_W_NN_PERTUB_H
#define SASFIT_PLUGIN_MASS_FRACTAL_W_NN_PERTUB_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_mass_fractal_w_nn_pertub.h
 * Public available functions and descriptions of the mass_fractal_w_nn_pertub plugin.
 */

/**
 * \def sasfit_mass_fractal_w_nn_pertub_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_mass_fractal_w_nn_pertub_EXPORTS)
	#ifdef sasfit_mass_fractal_w_nn_pertub_DLLEXP
	#undef sasfit_mass_fractal_w_nn_pertub_DLLEXP
	#endif
	#define sasfit_mass_fractal_w_nn_pertub_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_mass_fractal_w_nn_pertub_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_mass_fractal_w_nn_pertub_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq_mass_fractal_incl__nn_pertubation ################ */
/**
 * \defgroup sq_mass_fractal_incl__nn_pertubation mass fractal incl. nn pertubation
 * \ingroup sq_plugins_fractal_objects
 *
 * \brief \<some brief description of mass fractal incl. nn pertubation function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>particle radius</td>
 *      </tr><tr>
 *       <td>\b z1</td>
 *       <td>number of particles in first shell</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>fractal correlation length</td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td>fractal dimension</td>
 *      </tr><tr>
 *       <td>\b N</td>
 *       <td>number of particles in the fractal aggregate</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_mass_fractal_incl__nn_pertubation
 *
 * \sa sasfit_mass_fractal_w_nn_pertub.h, sq_plugins_fractal_objects
 */
sasfit_mass_fractal_w_nn_pertub_DLLEXP scalar sasfit_sq_mass_fractal_incl__nn_pertubation(scalar q, sasfit_param * p);

/**
 * \ingroup sq_mass_fractal_incl__nn_pertubation
 *
 * \sa sasfit_mass_fractal_w_nn_pertub.h, sq_plugins_fractal_objects
 */
sasfit_mass_fractal_w_nn_pertub_DLLEXP scalar sasfit_sq_mass_fractal_incl__nn_pertubation_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_mass_fractal_incl__nn_pertubation
 *
 * \sa sasfit_mass_fractal_w_nn_pertub.h, sq_plugins_fractal_objects
 */
sasfit_mass_fractal_w_nn_pertub_DLLEXP scalar sasfit_sq_mass_fractal_incl__nn_pertubation_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_mass_fractal_incl__nn_pertubation ################ */

/* ################ start sq_rlca_incl_nn_pertubation ################ */
/**
 * \defgroup sq_rlca_incl_nn_pertubation RLCA incl. nn pertubation
 * \ingroup sq_plugins_fractal_objects
 *
 * \brief \<some brief description of RLCA incl. nn pertubation function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>particle radius</td>
 *      </tr><tr>
 *       <td>\b N</td>
 *       <td>number of particles in the fractal aggregate</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_rlca_incl_nn_pertubation
 *
 * \sa sasfit_mass_fractal_w_nn_pertub.h, sq_plugins_fractal_objects
 */
sasfit_mass_fractal_w_nn_pertub_DLLEXP scalar sasfit_sq_rlca_incl_nn_pertubation(scalar q, sasfit_param * p);

/**
 * \ingroup sq_rlca_incl_nn_pertubation
 *
 * \sa sasfit_mass_fractal_w_nn_pertub.h, sq_plugins_fractal_objects
 */
sasfit_mass_fractal_w_nn_pertub_DLLEXP scalar sasfit_sq_rlca_incl_nn_pertubation_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_rlca_incl_nn_pertubation
 *
 * \sa sasfit_mass_fractal_w_nn_pertub.h, sq_plugins_fractal_objects
 */
sasfit_mass_fractal_w_nn_pertub_DLLEXP scalar sasfit_sq_rlca_incl_nn_pertubation_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_RLCA_incl_nn_pertubation ################ */

/* ################ start sq_DLCA_incl_nn_pertubation ################ */
/**
 * \defgroup sq_dlca_incl_nn_pertubation DLCA incl. nn pertubation
 * \ingroup sq_plugins_fractal_objects
 *
 * \brief \<some brief description of DLCA incl. nn pertubation function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>particle radius</td>
 *      </tr><tr>
 *       <td>\b N</td>
 *       <td>number of particles in the fractal aggregate</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_dlca_incl_nn_pertubation
 *
 * \sa sasfit_mass_fractal_w_nn_pertub.h, sq_plugins_fractal_objects
 */
sasfit_mass_fractal_w_nn_pertub_DLLEXP scalar sasfit_sq_dlca_incl_nn_pertubation(scalar q, sasfit_param * p);

/**
 * \ingroup sq_dlca_incl_nn_pertubation
 *
 * \sa sasfit_mass_fractal_w_nn_pertub.h, sq_plugins_fractal_objects
 */
sasfit_mass_fractal_w_nn_pertub_DLLEXP scalar sasfit_sq_dlca_incl_nn_pertubation_f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_dlca_incl_nn_pertubation
 *
 * \sa sasfit_mass_fractal_w_nn_pertub.h, sq_plugins_fractal_objects
 */
sasfit_mass_fractal_w_nn_pertub_DLLEXP scalar sasfit_sq_dlca_incl_nn_pertubation_v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_dlca_incl_nn_pertubation ################ */


#endif // this file

