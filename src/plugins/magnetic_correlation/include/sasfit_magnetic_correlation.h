/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_MAGNETIC_CORRELATION_H
#define SASFIT_PLUGIN_MAGNETIC_CORRELATION_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_magnetic_correlation.h
 * Public available functions and descriptions of the magnetic_correlation plugin.
 */

/**
 * \def sasfit_magnetic_correlation_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_magnetic_correlation_EXPORTS)
	#ifdef sasfit_magnetic_correlation_DLLEXP
	#undef sasfit_magnetic_correlation_DLLEXP
	#endif
	#define sasfit_magnetic_correlation_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_magnetic_correlation_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_magnetic_correlation_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_c_r__for_spin_misalignment ################ */
/**
 * \defgroup ff_c_r__for_spin_misalignment C(r) for spin misalignment
 * \ingroup ff_magnetic_spinmisalignment
 *
 * \brief C(r) for spin misalignment function
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b K</td>
 *       <td>K=8Hp^2V^(-1)</td>
 *      </tr><tr>
 *       <td>\b Hi</td>
 *       <td>internal magnetic field Hi=H0-N*Ms</td>
 *      </tr><tr>
 *       <td>\b l_H</td>
 *       <td>l_H = sqrt[2A/(mu_0*Ms*Hi)] exchange length of magnetic field</td>
 *      </tr><tr>
 *       <td>\b R </td>
 *       <td>"anisotropy-field" radius</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_c_r__for_spin_misalignment
 *
 * \sa sasfit_magnetic_correlation.h, ff_magnetic_spinmisalignment
 */
sasfit_magnetic_correlation_DLLEXP scalar sasfit_ff_c_r__for_spin_misalignment(scalar q, sasfit_param * p);

/**
 * \ingroup ff_c_r__for_spin_misalignment
 *
 * \sa sasfit_magnetic_correlation.h, ff_magnetic_spinmisalignment
 */
sasfit_magnetic_correlation_DLLEXP scalar sasfit_ff_c_r__for_spin_misalignment_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_c_r__for_spin_misalignment
 *
 * \sa sasfit_magnetic_correlation.h, ff_magnetic_spinmisalignment
 */
sasfit_magnetic_correlation_DLLEXP scalar sasfit_ff_c_r__for_spin_misalignment_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_c_r__for_spin_misalignment ################ */

/* ################ start ff_i_q__for_spin_misalignment ################ */
/**
 * \defgroup ff_i_q__for_spin_misalignment I(q) for spin misalignment
 * \ingroup ff_magnetic_spinmisalignment
 *
 * \brief I(q) for spin misalignment function
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b K</td>
 *       <td>K=8Hp^2V^(-1)</td>
 *      </tr><tr>
 *       <td>\b Hi</td>
 *       <td>internal magnetic field Hi=H0-N*Ms</td>
 *      </tr><tr>
 *       <td>\b l_H</td>
 *       <td>l_H = sqrt[2A/(mu_0*Ms*Hi)] exchange length of magnetic field</td>
 *      </tr><tr>
 *       <td>\b R </td>
 *       <td>‘anisotropy-field’ radius</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_i_q__for_spin_misalignment
 *
 * \sa sasfit_magnetic_correlation.h, ff_magnetic_spinmisalignment
 */
sasfit_magnetic_correlation_DLLEXP scalar sasfit_ff_i_q__for_spin_misalignment(scalar q, sasfit_param * p);

/**
 * \ingroup ff_i_q__for_spin_misalignment
 *
 * \sa sasfit_magnetic_correlation.h, ff_magnetic_spinmisalignment
 */
sasfit_magnetic_correlation_DLLEXP scalar sasfit_ff_i_q__for_spin_misalignment_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_i_q__for_spin_misalignment
 *
 * \sa sasfit_magnetic_correlation.h, ff_magnetic_spinmisalignment
 */
sasfit_magnetic_correlation_DLLEXP scalar sasfit_ff_i_q__for_spin_misalignment_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_i_q__for_spin_misalignment ################ */


#endif // this file

