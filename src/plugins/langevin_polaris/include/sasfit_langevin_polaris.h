/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_LANGEVIN_POLARIS_H
#define SASFIT_PLUGIN_LANGEVIN_POLARIS_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_langevin_polaris.h
 * Public available functions and descriptions of the langevin_polaris plugin.
 */

/**
 * \def sasfit_langevin_polaris_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_langevin_polaris_EXPORTS)
	#ifdef sasfit_langevin_polaris_DLLEXP
	#undef sasfit_langevin_polaris_DLLEXP
	#endif
	#define sasfit_langevin_polaris_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_langevin_polaris_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_langevin_polaris_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_langevin ################ */
/**
 * \defgroup ff_langevin Langevin
 * \ingroup ff_plugins_stroboscopic_superparamagnet
 *
 * \brief stroboscopic SANS measurements on structures showing superparamagnetic behaviour
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b B0 </td>
 *       <td>amplitude of oscillating magnetic field </td>
 *      </tr><tr>
 *       <td>\b B1 </td>
 *       <td>amplitude of static magnetic field</td>
 *      </tr><tr>
 *       <td>\b NU </td>
 *       <td>frequency of magnetic field</td>
 *      </tr><tr>
 *       <td>\b RHO_0 </td>
 *       <td>instrumental phase shift</td>
 *      </tr><tr>
 *       <td>\b PHI_ALPHA </td>
 *       <td>phase shift of Langevin parameter</td>
 *      </tr><tr>
 *       <td>\b PSI_DEG </td>
 *       <td>angle between scattering vector q and magnetic field B in degree</td>
 *      </tr><tr>
 *       <td>\b SQ_AMPL </td>
 *       <td>structure factor amplitude (1 for no structure factor)</td>
 *      </tr><tr>
 *       <td>\b SQ_PHASE </td>
 *       <td>phase shift of structure factor</td>
 *      </tr><tr>
 *       <td>\b SQ_STATIC </td>
 *       <td>value between 0 and 1 to describe if the structure factor changes with the magnetic field (SQ_STATIC=0) or not (SQ_STATIC=1)</td>
 *      </tr><tr>
 *       <td>\b FNUC_FMAG </td>
 *       <td>ratio between nuclear and magnetic scattering length density contrast</td>
 *      </tr><tr>
 *       <td>\b MU_KT </td>
 *       <td>ratio between magnetic moment of particle and kT</td>
 *      </tr><tr>
 *       <td>\b LAM_CENTER</td>
 *       <td>wavelength in Angstrom</td>
 *      </tr><tr>
 *       <td>\b D_LAM_LAM </td>
 *       <td>wavelength spread</td>
 *      </tr><tr>
 *       <td>\b SD </td>
 *       <td>sample to detector distance in meters</td>
 *      </tr><tr>
 *       <td>\b P_POL </td>
 *       <td> incident polarisation (-1,1)</td>
 *      </tr><tr>
 *       <td>\b Epsilon </td>
 *       <td> spin flipper efficiency (0,1)</td>
 *      </tr><tr>
 *       <td>\b T_up </td>
 *       <td> transmission of up polarisation of analyser (0,1)</td>
 *      </tr><tr>
 *       <td>\b T_down </td>
 *       <td> transmission of down polarisation of analyser (0,1)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_langevin
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin(scalar q, sasfit_param * p);

/**
 * \ingroup ff_langevin
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_langevin
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_langevin ################ */


/* ################ start ff_langevin_static ################ */
/**
 * \defgroup ff_langevin_static Langevin_static
 * \ingroup ff_plugins_stroboscopic_superparamagnet
 *
 * \brief stroboscopic SANS measurements on structures showing superparamagnetic behaviour
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b B0 </td>
 *       <td>amplitude of oscillating magnetic field </td>
 *      </tr><tr>
 *       <td>\b B1 </td>
 *       <td>amplitude of static magnetic field</td>
 *      </tr><tr>
 *       <td>\b NU </td>
 *       <td>frequency of magnetic field</td>
 *      </tr><tr>
 *       <td>\b RHO_0 </td>
 *       <td>instrumental phase shift</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b PSI_DEG </td>
 *       <td>angle between scattering vector q and magnetic field B in degree</td>
 *      </tr><tr>
 *       <td>\b SQ_AMPL </td>
 *       <td>structure factor amplitude (1 for no structure factor)</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b FNUC_FMAG </td>
 *       <td>ratio between nuclear and magnetic scattering length density contrast</td>
 *      </tr><tr>
 *       <td>\b MU_KT </td>
 *       <td>ratio between magnetic moment of particle and kT</td>
 *      </tr><tr>
 *       <td>\b LAM_CENTER</td>
 *       <td>wavelength in Angstrom</td>
 *      </tr><tr>
 *       <td>\b D_LAM_LAM </td>
 *       <td>wavelength spread</td>
 *      </tr><tr>
 *       <td>\b SD </td>
 *       <td>sample to detector distance in meters</td>
 *      </tr><tr>
 *       <td>\b P_POL </td>
 *       <td> incident polarisation (-1,1)</td>
 *      </tr><tr>
 *       <td>\b Epsilon </td>
 *       <td> spin flipper efficiency (0,1)</td>
 *      </tr><tr>
 *       <td>\b T_up </td>
 *       <td> transmission of up polarisation of analyser (0,1)</td>
 *      </tr><tr>
 *       <td>\b T_down </td>
 *       <td> transmission of down polarisation of analyser (0,1)</td>
 *      </tr><tr>
 *       <td>\b B_static </td>
 *       <td> effective field describing the orientation distribution of the fozen moments</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_langevin_static
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_static(scalar q, sasfit_param * p);

/**
 * \ingroup ff_langevin_static
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_static_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_langevin_static
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_static_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_langevin_static ################ */


/* ################ start ff_langevin_tau ################ */
/**
 * \defgroup ff_langevin_tau Langevin_tau
 * \ingroup ff_plugins_stroboscopic_superparamagnet
 *
 * \brief stroboscopic SANS measurements on structures showing superparamagnetic behaviour
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b B0 </td>
 *       <td>amplitude of oscillating magnetic field </td>
 *      </tr><tr>
 *       <td>\b B1 </td>
 *       <td>amplitude of static magnetic field</td>
 *      </tr><tr>
 *       <td>\b NU </td>
 *       <td>frequency of magnetic field</td>
 *      </tr><tr>
 *       <td>\b RHO_0 </td>
 *       <td>instrumental phase shift</td>
 *      </tr><tr>
 *       <td>\b TAU </td>
 *       <td>relaxation time of the system</td>
 *      </tr><tr>
 *       <td>\b PSI_DEG </td>
 *       <td>angle between scattering vector q and magnetic field B in degree</td>
 *      </tr><tr>
 *       <td>\b SQ_AMPL </td>
 *       <td>structure factor amplitude (1 for no structure factor)</td>
 *      </tr><tr>
 *       <td>\b SQ_PHASE </td>
 *       <td>phase shift of structure factor</td>
 *      </tr><tr>
 *       <td>\b SQ_STATIC </td>
 *       <td>value between 0 and 1 to describe if the structure factor changes with the magnetic field (SQ_STATIC=0) or not (SQ_STATIC=1)</td>
 *      </tr><tr>
 *       <td>\b FNUC_FMAG </td>
 *       <td>ratio between nuclear and magnetic scattering length density contrast</td>
 *      </tr><tr>
 *       <td>\b MU_KT </td>
 *       <td>ratio between magnetic moment of particle and kT</td>
 *      </tr><tr>
 *       <td>\b LAM_CENTER</td>
 *       <td>wavelength in Angstrom</td>
 *      </tr><tr>
 *       <td>\b D_LAM_LAM </td>
 *       <td>wavelength spread</td>
 *      </tr><tr>
 *       <td>\b SD </td>
 *       <td>sample to detector distance in meters</td>
 *      </tr><tr>
 *       <td>\b P_POL </td>
 *       <td> incident polarisation (-1,1)</td>
 *      </tr><tr>
 *       <td>\b Epsilon </td>
 *       <td> spin flipper efficiency (0,1)</td>
 *      </tr><tr>
 *       <td>\b T_up </td>
 *       <td> transmission of up polarisation of analyser (0,1)</td>
 *      </tr><tr>
 *       <td>\b T_down </td>
 *       <td> transmission of down polarisation of analyser (0,1)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_langevin_tau
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_tau(scalar q, sasfit_param * p);

/**
 * \ingroup ff_langevin_tau
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_tau_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_langevin_tau
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_tau_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_langevin_tau ################ */







/* ################ start ff_tisane ################ */
/**
 * \defgroup ff_tisane TISANE
 * \ingroup ff_plugins_stroboscopic_superparamagnet
 *
 * \brief stroboscopic SANS measurements on structures showing superparamagnetic behaviour
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b B0 </td>
 *       <td>amplitude of oscillating magnetic field </td>
 *      </tr><tr>
 *       <td>\b B1 </td>
 *       <td>amplitude of static magnetic field</td>
 *      </tr><tr>
 *       <td>\b NU </td>
 *       <td>frequency of magnetic field</td>
 *      </tr><tr>
 *       <td>\b RHO_0 </td>
 *       <td>instrumental phase shift</td>
 *      </tr><tr>
 *       <td>\b PHI_ALPHA </td>
 *       <td>phase shift of Langevin parameter</td>
 *      </tr><tr>
 *       <td>\b PSI_DEG </td>
 *       <td>angle between scattering vector q and magnetic field B in degree</td>
 *      </tr><tr>
 *       <td>\b SQ_AMPL </td>
 *       <td>structure factor amplitude (1 for no structure factor)</td>
 *      </tr><tr>
 *       <td>\b SQ_PHASE </td>
 *       <td>phase shift of structure factor</td>
 *      </tr><tr>
 *       <td>\b SQ_STATIC </td>
 *       <td>value between 0 and 1 to describe if the structure factor changes with the magnetic field (SQ_STATIC=0) or not (SQ_STATIC=1)</td>
 *      </tr><tr>
 *       <td>\b FNUC_FMAG </td>
 *       <td>ratio between nuclear and magnetic scattering length density contrast</td>
 *      </tr><tr>
 *       <td>\b MU_KT </td>
 *       <td>ratio between magnetic moment of particle and kT</td>
 *      </tr><tr>
 *       <td>\b DELTA_T_(bottom)</td>
 *       <td>TISANE time resolution (bottom base of trapezoidal)</td>
 *      </tr><tr>
 *       <td>\b DELTA_T_(top) </td>
 *       <td>TISANE time resolution (top base of trapezoidal)</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b P_POL </td>
 *       <td> incident polarisation (-1,1)</td>
 *      </tr><tr>
 *       <td>\b Epsilon </td>
 *       <td> spin flipper efficiency (0,1)</td>
 *      </tr><tr>
 *       <td>\b T_up </td>
 *       <td> transmission of up polarisation of analyser (0,1)</td>
 *      </tr><tr>
 *       <td>\b T_down </td>
 *       <td> transmission of down polarisation of analyser (0,1)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_tisane
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_tisane(scalar q, sasfit_param * p);

/**
 * \ingroup ff_tisane
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_tisane_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_tisane
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_tisane_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_tisane ################ */


/* ################ start ff_tisane_static ################ */
/**
 * \defgroup ff_tisane_static TISANE_static
 * \ingroup ff_plugins_stroboscopic_superparamagnet
 *
 * \brief stroboscopic SANS measurements on structures showing superparamagnetic behaviour
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b B0 </td>
 *       <td>amplitude of oscillating magnetic field </td>
 *      </tr><tr>
 *       <td>\b B1 </td>
 *       <td>amplitude of static magnetic field</td>
 *      </tr><tr>
 *       <td>\b NU </td>
 *       <td>frequency of magnetic field</td>
 *      </tr><tr>
 *       <td>\b RHO_0 </td>
 *       <td>instrumental phase shift</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b PSI_DEG </td>
 *       <td>angle between scattering vector q and magnetic field B in degree</td>
 *      </tr><tr>
 *       <td>\b SQ_AMPL </td>
 *       <td>structure factor amplitude (1 for no structure factor)</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b FNUC_FMAG </td>
 *       <td>ratio between nuclear and magnetic scattering length density contrast</td>
 *      </tr><tr>
 *       <td>\b MU_KT </td>
 *       <td>ratio between magnetic moment of particle and kT</td>
 *      </tr><tr>
 *       <td>\b DELTA_T_(bottom)</td>
 *       <td>TISANE time resolution (bottom base of trapezoidal)</td>
 *      </tr><tr>
 *       <td>\b DELTA_T_(top) </td>
 *       <td>TISANE time resolution (top base of trapezoidal)</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b P_POL </td>
 *       <td> incident polarisation (-1,1)</td>
 *      </tr><tr>
 *       <td>\b Epsilon </td>
 *       <td> spin flipper efficiency (0,1)</td>
 *      </tr><tr>
 *       <td>\b T_up </td>
 *       <td> transmission of up polarisation of analyser (0,1)</td>
 *      </tr><tr>
 *       <td>\b T_down </td>
 *       <td> transmission of down polarisation of analyser (0,1)</td>
 *      </tr><tr>
 *       <td>\b B_static </td>
 *       <td> effective field describing the orientation distribution of the fozen moments</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_tisane_static
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_tisane_static(scalar q, sasfit_param * p);

/**
 * \ingroup ff_tisane_static
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_tisane_static_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_tisane_static
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_tisane_static_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_tisane_static ################ */


/* ################ start ff_tisane_tau ################ */
/**
 * \defgroup ff_tisane_tau TISANE_tau
 * \ingroup ff_plugins_stroboscopic_superparamagnet
 *
 * \brief stroboscopic SANS measurements on structures showing superparamagnetic behaviour
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b B0 </td>
 *       <td>amplitude of oscillating magnetic field </td>
 *      </tr><tr>
 *       <td>\b B1 </td>
 *       <td>amplitude of static magnetic field</td>
 *      </tr><tr>
 *       <td>\b NU </td>
 *       <td>frequency of magnetic field</td>
 *      </tr><tr>
 *       <td>\b RHO_0 </td>
 *       <td>instrumental phase shift</td>
 *      </tr><tr>
 *       <td>\b TAU </td>
 *       <td>relaxation time of the system</td>
 *      </tr><tr>
 *       <td>\b PSI_DEG </td>
 *       <td>angle between scattering vector q and magnetic field B in degree</td>
 *      </tr><tr>
 *       <td>\b SQ_AMPL </td>
 *       <td>structure factor amplitude (1 for no structure factor)</td>
 *      </tr><tr>
 *       <td>\b SQ_PHASE </td>
 *       <td>phase shift of structure factor</td>
 *      </tr><tr>
 *       <td>\b SQ_STATIC </td>
 *       <td>value between 0 and 1 to describe if the structure factor changes with the magnetic field (SQ_STATIC=0) or not (SQ_STATIC=1)</td>
 *      </tr><tr>
 *       <td>\b FNUC_FMAG </td>
 *       <td>ratio between nuclear and magnetic scattering length density contrast</td>
 *      </tr><tr>
 *       <td>\b MU_KT </td>
 *       <td>ratio between magnetic moment of particle and kT</td>
 *      </tr><tr>
 *       <td>\b DELTA_T_(bottom) </td>
 *       <td>TISANE time resolution (bottom base of trapezoidal)</td>
 *      </tr><tr>
 *       <td>\b DELTA_T_(top) </td>
 *       <td>TISANE time resolution (top base of trapezoidal)</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b P_POL </td>
 *       <td> incident polarisation (-1,1)</td>
 *      </tr><tr>
 *       <td>\b Epsilon </td>
 *       <td> spin flipper efficiency (0,1)</td>
 *      </tr><tr>
 *       <td>\b T_up </td>
 *       <td> transmission of up polarisation of analyser (0,1)</td>
 *      </tr><tr>
 *       <td>\b T_down </td>
 *       <td> transmission of down polarisation of analyser (0,1)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_tisane_tau
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_tisane_tau(scalar q, sasfit_param * p);

/**
 * \ingroup ff_tisane_tau
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_tisane_tau_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_tisane_tau
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_tisane_tau_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_langevin_tau ################ */


















/* ################ start ff_langevin_up_up ################ */
/**
 * \defgroup ff_langevin_up_up Langevin_up_up
 * \ingroup ff_plugins_stroboscopic_superparamagnet
 *
 * \brief stroboscopic SANS measurements on structures showing superparamagnetic behaviour
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b B0 </td>
 *       <td>amplitude of oscillating magnetic field </td>
 *      </tr><tr>
 *       <td>\b B1 </td>
 *       <td>amplitude of static magnetic field</td>
 *      </tr><tr>
 *       <td>\b NU </td>
 *       <td>frequency of magnetic field</td>
 *      </tr><tr>
 *       <td>\b RHO_0 </td>
 *       <td>instrumental phase shift</td>
 *      </tr><tr>
 *       <td>\b PHI_ALPHA </td>
 *       <td>phase shift of Langevin parameter</td>
 *      </tr><tr>
 *       <td>\b PSI_DEG </td>
 *       <td>angle between scattering vector q and magnetic field B in degree</td>
 *      </tr><tr>
 *       <td>\b SQ_AMPL </td>
 *       <td>structure factor amplitude (1 for no structure factor)</td>
 *      </tr><tr>
 *       <td>\b SQ_PHASE </td>
 *       <td>phase shift of structure factor</td>
 *      </tr><tr>
 *       <td>\b SQ_STATIC </td>
 *       <td>value between 0 and 1 to describe if the structure factor changes with the magnetic field (SQ_STATIC=0) or not (SQ_STATIC=1)</td>
 *      </tr><tr>
 *       <td>\b FNUC_FMAG </td>
 *       <td>ratio between nuclear and magnetic scattering length density contrast</td>
 *      </tr><tr>
 *       <td>\b MU_KT </td>
 *       <td>ratio between magnetic moment of particle and kT</td>
 *      </tr><tr>
 *       <td>\b LAM_CENTER</td>
 *       <td>wavelength in Angstrom</td>
 *      </tr><tr>
 *       <td>\b D_LAM_LAM </td>
 *       <td>wavelength spread</td>
 *      </tr><tr>
 *       <td>\b SD </td>
 *       <td>sample to detector distance in meters</td>
 *      </tr><tr>
 *       <td>\b P_POL </td>
 *       <td> incident polarisation</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_langevin_up_up
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_up_up(scalar q, sasfit_param * p);

/**
 * \ingroup ff_langevin_up_up
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_up_up_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_langevin_up_up
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_up_up_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_langevin_up_up ################ */

/* ################ start ff_langevin_down_down ################ */
/**
 * \defgroup ff_langevin_down_down Langevin_down_down
 * \ingroup ff_plugins_stroboscopic_superparamagnet
 *
* \brief stroboscopic SANS measurements on structures showing superparamagnetic behaviour
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b B0 </td>
 *       <td>amplitude of oscillating magnetic field </td>
 *      </tr><tr>
 *       <td>\b B1 </td>
 *       <td>amplitude of static magnetic field</td>
 *      </tr><tr>
 *       <td>\b NU </td>
 *       <td>frequency of magnetic field</td>
 *      </tr><tr>
 *       <td>\b RHO_0 </td>
 *       <td>instrumental phase shift</td>
 *      </tr><tr>
 *       <td>\b PHI_ALPHA </td>
 *       <td>phase shift of Langevin parameter</td>
 *      </tr><tr>
 *       <td>\b PSI_DEG </td>
 *       <td>angle between scattering vector q and magnetic field B in degree</td>
 *      </tr><tr>
 *       <td>\b SQ_AMPL </td>
 *       <td>structure factor amplitude (1 for no structure factor)</td>
 *      </tr><tr>
 *       <td>\b SQ_PHASE </td>
 *       <td>phase shift of structure factor</td>
 *      </tr><tr>
 *       <td>\b SQ_STATIC </td>
 *       <td>value between 0 and 1 to describe if the structure factor changes with the magnetic field (SQ_STATIC=0) or not (SQ_STATIC=1)</td>
 *      </tr><tr>
 *       <td>\b FNUC_FMAG </td>
 *       <td>ratio between nuclear and magnetic scattering length density contrast</td>
 *      </tr><tr>
 *       <td>\b MU_KT </td>
 *       <td>ratio between magnetic moment of particle and kT</td>
 *      </tr><tr>
 *       <td>\b LAM_CENTER</td>
 *       <td>wavelength in Angstrom</td>
 *      </tr><tr>
 *       <td>\b D_LAM_LAM </td>
 *       <td>wavelength spread</td>
 *      </tr><tr>
 *       <td>\b SD </td>
 *       <td>sample to detector distance in meters</td>
 *      </tr><tr>
 *       <td>\b P_POL </td>
 *       <td> incident polarisation</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_langevin_down_down
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_down_down(scalar q, sasfit_param * p);

/**
 * \ingroup ff_langevin_down_down
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_down_down_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_langevin_down_down
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_down_down_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_langevin_down_down ################ */

/* ################ start ff_langevin_up_down ################ */
/**
 * \defgroup ff_langevin_up_down Langevin_up_down
 * \ingroup ff_plugins_stroboscopic_superparamagnet
 *
 * \brief stroboscopic SANS measurements on structures showing superparamagnetic behaviour
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b B0 </td>
 *       <td>amplitude of oscillating magnetic field </td>
 *      </tr><tr>
 *       <td>\b B1 </td>
 *       <td>amplitude of static magnetic field</td>
 *      </tr><tr>
 *       <td>\b NU </td>
 *       <td>frequency of magnetic field</td>
 *      </tr><tr>
 *       <td>\b RHO_0 </td>
 *       <td>instrumental phase shift</td>
 *      </tr><tr>
 *       <td>\b PHI_ALPHA </td>
 *       <td>phase shift of Langevin parameter</td>
 *      </tr><tr>
 *       <td>\b PSI_DEG </td>
 *       <td>angle between scattering vector q and magnetic field B in degree</td>
 *      </tr><tr>
 *       <td>\b SQ_AMPL </td>
 *       <td>structure factor amplitude (1 for no structure factor)</td>
 *      </tr><tr>
 *       <td>\b SQ_PHASE </td>
 *       <td>phase shift of structure factor</td>
 *      </tr><tr>
 *       <td>\b SQ_STATIC </td>
 *       <td>value between 0 and 1 to describe if the structure factor changes with the magnetic field (SQ_STATIC=0) or not (SQ_STATIC=1)</td>
 *      </tr><tr>
 *       <td>\b FNUC_FMAG </td>
 *       <td>ratio between nuclear and magnetic scattering length density contrast</td>
 *      </tr><tr>
 *       <td>\b MU_KT </td>
 *       <td>ratio between magnetic moment of particle and kT</td>
 *      </tr><tr>
 *       <td>\b LAM_CENTER</td>
 *       <td>wavelength in Angstrom</td>
 *      </tr><tr>
 *       <td>\b D_LAM_LAM </td>
 *       <td>wavelength spread</td>
 *      </tr><tr>
 *       <td>\b SD </td>
 *       <td>sample to detector distance in meters</td>
 *      </tr><tr>
 *       <td>\b P_POL </td>
 *       <td> incident polarisation</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_langevin_up_down
 *
 * \sa sasfit_langevin_polaris.h, ff_plugins_polaris
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_up_down(scalar q, sasfit_param * p);

/**
 * \ingroup ff_langevin_up_down
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_up_down_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_langevin_up_down
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_up_down_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_langevin_up_down ################ */

/* ################ start ff_langevin_down_up ################ */
/**
 * \defgroup ff_langevin_down_up Langevin_down_up
 * \ingroup ff_plugins_stroboscopic_superparamagnet
 *
 * \brief stroboscopic SANS measurements on structures showing superparamagnetic behaviour
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b B0 </td>
 *       <td>amplitude of oscillating magnetic field </td>
 *      </tr><tr>
 *       <td>\b B1 </td>
 *       <td>amplitude of static magnetic field</td>
 *      </tr><tr>
 *       <td>\b NU </td>
 *       <td>frequency of magnetic field</td>
 *      </tr><tr>
 *       <td>\b RHO_0 </td>
 *       <td>instrumental phase shift</td>
 *      </tr><tr>
 *       <td>\b PHI_ALPHA </td>
 *       <td>phase shift of Langevin parameter</td>
 *      </tr><tr>
 *       <td>\b PSI_DEG </td>
 *       <td>angle between scattering vector q and magnetic field B in degree</td>
 *      </tr><tr>
 *       <td>\b SQ_AMPL </td>
 *       <td>structure factor amplitude (1 for no structure factor)</td>
 *      </tr><tr>
 *       <td>\b SQ_PHASE </td>
 *       <td>phase shift of structure factor</td>
 *      </tr><tr>
 *       <td>\b SQ_STATIC </td>
 *       <td>value between 0 and 1 to describe if the structure factor changes with the magnetic field (SQ_STATIC=0) or not (SQ_STATIC=1)</td>
 *      </tr><tr>
 *       <td>\b FNUC_FMAG </td>
 *       <td>ratio between nuclear and magnetic scattering length density contrast</td>
 *      </tr><tr>
 *       <td>\b MU_KT </td>
 *       <td>ratio between magnetic moment of particle and kT</td>
 *      </tr><tr>
 *       <td>\b LAM_CENTER</td>
 *       <td>wavelength in Angstrom</td>
 *      </tr><tr>
 *       <td>\b D_LAM_LAM </td>
 *       <td>wavelength spread</td>
 *      </tr><tr>
 *       <td>\b SD </td>
 *       <td>sample to detector distance in meters</td>
 *      </tr><tr>
 *       <td>\b P_POL </td>
 *       <td> incident polarisation</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_langevin_down_up
 *
 * \sa sasfit_langevin_polaris.h, ff_plugins_polaris
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_down_up(scalar t, sasfit_param * p);

/**
 * \ingroup ff_langevin_down_up
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_down_up_f(scalar t, sasfit_param * p);

/**
 * \ingroup ff_langevin_down_up
 *
 * \sa sasfit_langevin_polaris.h, ff_plugins_polaris
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_down_up_v(scalar t, sasfit_param * p, int dist);
/* ################ stop ff_langevin_down_up ################ */

/* ################ start ff_langevin_up ################ */
/**
 * \defgroup ff_langevin_up Langevin_up
 * \ingroup ff_plugins_stroboscopic_superparamagnet
 *
 * \brief stroboscopic SANS measurements on structures showing superparamagnetic behaviour
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b B0 </td>
 *       <td>amplitude of oscillating magnetic field </td>
 *      </tr><tr>
 *       <td>\b B1 </td>
 *       <td>amplitude of static magnetic field</td>
 *      </tr><tr>
 *       <td>\b NU </td>
 *       <td>frequency of magnetic field</td>
 *      </tr><tr>
 *       <td>\b RHO_0 </td>
 *       <td>instrumental phase shift</td>
 *      </tr><tr>
 *       <td>\b PHI_ALPHA </td>
 *       <td>phase shift of Langevin parameter</td>
 *      </tr><tr>
 *       <td>\b PSI_DEG </td>
 *       <td>angle between scattering vector q and magnetic field B in degree</td>
 *      </tr><tr>
 *       <td>\b SQ_AMPL </td>
 *       <td>structure factor amplitude (1 for no structure factor)</td>
 *      </tr><tr>
 *       <td>\b SQ_PHASE </td>
 *       <td>phase shift of structure factor</td>
 *      </tr><tr>
 *       <td>\b SQ_STATIC </td>
 *       <td>value between 0 and 1 to describe if the structure factor changes with the magnetic field (SQ_STATIC=0) or not (SQ_STATIC=1)</td>
 *      </tr><tr>
 *       <td>\b FNUC_FMAG </td>
 *       <td>ratio between nuclear and magnetic scattering length density contrast</td>
 *      </tr><tr>
 *       <td>\b MU_KT </td>
 *       <td>ratio between magnetic moment of particle and kT</td>
 *      </tr><tr>
 *       <td>\b LAM_CENTER</td>
 *       <td>wavelength in Angstrom</td>
 *      </tr><tr>
 *       <td>\b D_LAM_LAM </td>
 *       <td>wavelength spread</td>
 *      </tr><tr>
 *       <td>\b SD </td>
 *       <td>sample to detector distance in meters</td>
 *      </tr><tr>
 *       <td>\b P_POL </td>
 *       <td> incident polarisation</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_langevin_up
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_up(scalar t, sasfit_param * p);

/**
 * \ingroup ff_langevin_up
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_up_f(scalar t, sasfit_param * p);

/**
 * \ingroup ff_langevin_up
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_up_v(scalar t, sasfit_param * p, int dist);
/* ################ stop ff_langevin_up ################ */


/* ################ start ff_langevin_down ################ */
/**
 * \defgroup ff_langevin_down Langevin_down
 * \ingroup ff_plugins_stroboscopic_superparamagnet
 *
 * \brief stroboscopic SANS measurements on structures showing superparamagnetic behaviour
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b B0 </td>
 *       <td>amplitude of oscillating magnetic field </td>
 *      </tr><tr>
 *       <td>\b B1 </td>
 *       <td>amplitude of static magnetic field</td>
 *      </tr><tr>
 *       <td>\b NU </td>
 *       <td>frequency of magnetic field</td>
 *      </tr><tr>
 *       <td>\b RHO_0 </td>
 *       <td>instrumental phase shift</td>
 *      </tr><tr>
 *       <td>\b PHI_ALPHA </td>
 *       <td>phase shift of Langevin parameter</td>
 *      </tr><tr>
 *       <td>\b PSI_DEG </td>
 *       <td>angle between scattering vector q and magnetic field B in degree</td>
 *      </tr><tr>
 *       <td>\b SQ_AMPL </td>
 *       <td>structure factor amplitude (1 for no structure factor)</td>
 *      </tr><tr>
 *       <td>\b SQ_PHASE </td>
 *       <td>phase shift of structure factor</td>
 *      </tr><tr>
 *       <td>\b SQ_STATIC </td>
 *       <td>value between 0 and 1 to describe if the structure factor changes with the magnetic field (SQ_STATIC=0) or not (SQ_STATIC=1)</td>
 *      </tr><tr>
 *       <td>\b FNUC_FMAG </td>
 *       <td>ratio between nuclear and magnetic scattering length density contrast</td>
 *      </tr><tr>
 *       <td>\b MU_KT </td>
 *       <td>ratio between magnetic moment of particle and kT</td>
 *      </tr><tr>
 *       <td>\b LAM_CENTER</td>
 *       <td>wavelength in Angstrom</td>
 *      </tr><tr>
 *       <td>\b D_LAM_LAM </td>
 *       <td>wavelength spread</td>
 *      </tr><tr>
 *       <td>\b SD </td>
 *       <td>sample to detector distance in meters</td>
 *      </tr><tr>
 *       <td>\b P_POL </td>
 *       <td> incident polarisation</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_langevin_up
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_down(scalar t, sasfit_param * p);

/**
 * \ingroup ff_langevin_down
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_down_f(scalar t, sasfit_param * p);

/**
 * \ingroup ff_langevin_down
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_down_v(scalar t, sasfit_param * p, int dist);
/* ################ stop ff_langevin_down ################ */

#endif // this file


/* ################ start ff_langevin_static_up_up ################ */
/**
 * \defgroup ff_langevin_static_up_up Langevin_static_up_up
 * \ingroup ff_plugins_stroboscopic_superparamagnet
 *
 * \brief stroboscopic SANS measurements on structures showing superparamagnetic behaviour
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b B0 </td>
 *       <td>amplitude of oscillating magnetic field </td>
 *      </tr><tr>
 *       <td>\b B1 </td>
 *       <td>amplitude of static magnetic field</td>
 *      </tr><tr>
 *       <td>\b NU </td>
 *       <td>frequency of magnetic field</td>
 *      </tr><tr>
 *       <td>\b RHO_0 </td>
 *       <td>instrumental phase shift</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b PSI_DEG </td>
 *       <td>angle between scattering vector q and magnetic field B in degree</td>
 *      </tr><tr>
 *       <td>\b SQ_AMPL </td>
 *       <td>structure factor amplitude (1 for no structure factor)</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b FNUC_FMAG </td>
 *       <td>ratio between nuclear and magnetic scattering length density contrast</td>
 *      </tr><tr>
 *       <td>\b MU_KT </td>
 *       <td>ratio between magnetic moment of particle and kT</td>
 *      </tr><tr>
 *       <td>\b LAM_CENTER</td>
 *       <td>wavelength in Angstrom</td>
 *      </tr><tr>
 *       <td>\b D_LAM_LAM </td>
 *       <td>wavelength spread</td>
 *      </tr><tr>
 *       <td>\b SD </td>
 *       <td>sample to detector distance in meters</td>
 *      </tr><tr>
 *       <td>\b P_POL </td>
 *       <td> incident polarisation</td>
 *      </tr></table>
 */


/**
 * \ingroup ff_langevin_static_up_up
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_static_up_up(scalar q, sasfit_param * p);

/**
 * \ingroup ff_langevin_static_up_up
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_static_up_up_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_langevin_static_up_up
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_static_up_up_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_langevin_static_up_up ################ */


/* ################ start ff_langevin_static_up_down ################ */
/**
 * \defgroup ff_langevin_static_up_down Langevin_static_up_down
 * \ingroup ff_plugins_stroboscopic_superparamagnet
 *
 * \brief stroboscopic SANS measurements on structures showing superparamagnetic behaviour
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b B0 </td>
 *       <td>amplitude of oscillating magnetic field </td>
 *      </tr><tr>
 *       <td>\b B1 </td>
 *       <td>amplitude of static magnetic field</td>
 *      </tr><tr>
 *       <td>\b NU </td>
 *       <td>frequency of magnetic field</td>
 *      </tr><tr>
 *       <td>\b RHO_0 </td>
 *       <td>instrumental phase shift</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b PSI_DEG </td>
 *       <td>angle between scattering vector q and magnetic field B in degree</td>
 *      </tr><tr>
 *       <td>\b SQ_AMPL </td>
 *       <td>structure factor amplitude (1 for no structure factor)</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b FNUC_FMAG </td>
 *       <td>ratio between nuclear and magnetic scattering length density contrast</td>
 *      </tr><tr>
 *       <td>\b MU_KT </td>
 *       <td>ratio between magnetic moment of particle and kT</td>
 *      </tr><tr>
 *       <td>\b LAM_CENTER</td>
 *       <td>wavelength in Angstrom</td>
 *      </tr><tr>
 *       <td>\b D_LAM_LAM </td>
 *       <td>wavelength spread</td>
 *      </tr><tr>
 *       <td>\b SD </td>
 *       <td>sample to detector distance in meters</td>
 *      </tr><tr>
 *       <td>\b P_POL </td>
 *       <td> incident polarisation</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_langevin_static_up_down
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_static_up_down(scalar q, sasfit_param * p);

/**
 * \ingroup ff_langevin_static_up_down
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_static_up_down_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_langevin_static_up_down
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_static_up_down_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_langevin_static_up_down ################ */

/* ################ start ff_langevin_static_down_up ################ */
/**
 * \defgroup ff_langevin_static_down_up Langevin_static_down_up
 * \ingroup ff_plugins_stroboscopic_superparamagnet
 *
 * \brief stroboscopic SANS measurements on structures showing superparamagnetic behaviour
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b B0 </td>
 *       <td>amplitude of oscillating magnetic field </td>
 *      </tr><tr>
 *       <td>\b B1 </td>
 *       <td>amplitude of static magnetic field</td>
 *      </tr><tr>
 *       <td>\b NU </td>
 *       <td>frequency of magnetic field</td>
 *      </tr><tr>
 *       <td>\b RHO_0 </td>
 *       <td>instrumental phase shift</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b PSI_DEG </td>
 *       <td>angle between scattering vector q and magnetic field B in degree</td>
 *      </tr><tr>
 *       <td>\b SQ_AMPL </td>
 *       <td>structure factor amplitude (1 for no structure factor)</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b FNUC_FMAG </td>
 *       <td>ratio between nuclear and magnetic scattering length density contrast</td>
 *      </tr><tr>
 *       <td>\b MU_KT </td>
 *       <td>ratio between magnetic moment of particle and kT</td>
 *      </tr><tr>
 *       <td>\b LAM_CENTER</td>
 *       <td>wavelength in Angstrom</td>
 *      </tr><tr>
 *       <td>\b D_LAM_LAM </td>
 *       <td>wavelength spread</td>
 *      </tr><tr>
 *       <td>\b SD </td>
 *       <td>sample to detector distance in meters</td>
 *      </tr><tr>
 *       <td>\b P_POL </td>
 *       <td> incident polarisation</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_langevin_static_down_up
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_static_down_up(scalar q, sasfit_param * p);

/**
 * \ingroup ff_langevin_static_down_up
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_static_down_up_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_langevin_static_down_up
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_static_down_up_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_langevin_static_down_up ################ */


/* ################ start ff_langevin_static_down_down ################ */
/**
 * \defgroup ff_langevin_static_down_down Langevin_static_down_down
 * \ingroup ff_plugins_stroboscopic_superparamagnet
 *
 * \brief stroboscopic SANS measurements on structures showing superparamagnetic behaviour
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b B0 </td>
 *       <td>amplitude of oscillating magnetic field </td>
 *      </tr><tr>
 *       <td>\b B1 </td>
 *       <td>amplitude of static magnetic field</td>
 *      </tr><tr>
 *       <td>\b NU </td>
 *       <td>frequency of magnetic field</td>
 *      </tr><tr>
 *       <td>\b RHO_0 </td>
 *       <td>instrumental phase shift</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b PSI_DEG </td>
 *       <td>angle between scattering vector q and magnetic field B in degree</td>
 *      </tr><tr>
 *       <td>\b SQ_AMPL </td>
 *       <td>structure factor amplitude (1 for no structure factor)</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b FNUC_FMAG </td>
 *       <td>ratio between nuclear and magnetic scattering length density contrast</td>
 *      </tr><tr>
 *       <td>\b MU_KT </td>
 *       <td>ratio between magnetic moment of particle and kT</td>
 *      </tr><tr>
 *       <td>\b LAM_CENTER</td>
 *       <td>wavelength in Angstrom</td>
 *      </tr><tr>
 *       <td>\b D_LAM_LAM </td>
 *       <td>wavelength spread</td>
 *      </tr><tr>
 *       <td>\b SD </td>
 *       <td>sample to detector distance in meters</td>
 *      </tr><tr>
 *       <td>\b P_POL </td>
 *       <td> incident polarisation</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_langevin_static_down_down
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_static_down_down(scalar q, sasfit_param * p);

/**
 * \ingroup ff_langevin_static_down_down
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_static_down_down_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_langevin_static_down_down
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_static_down_down_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_langevin_static_down_down ################ */


/* ################ start ff_langevin_static_up ################ */
/**
 * \defgroup ff_langevin_static_up Langevin_static_up
 * \ingroup ff_plugins_stroboscopic_superparamagnet
 *
 * \brief stroboscopic SANS measurements on structures showing superparamagnetic behaviour
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b B0 </td>
 *       <td>amplitude of oscillating magnetic field </td>
 *      </tr><tr>
 *       <td>\b B1 </td>
 *       <td>amplitude of static magnetic field</td>
 *      </tr><tr>
 *       <td>\b NU </td>
 *       <td>frequency of magnetic field</td>
 *      </tr><tr>
 *       <td>\b RHO_0 </td>
 *       <td>instrumental phase shift</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b PSI_DEG </td>
 *       <td>angle between scattering vector q and magnetic field B in degree</td>
 *      </tr><tr>
 *       <td>\b SQ_AMPL </td>
 *       <td>structure factor amplitude (1 for no structure factor)</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b FNUC_FMAG </td>
 *       <td>ratio between nuclear and magnetic scattering length density contrast</td>
 *      </tr><tr>
 *       <td>\b MU_KT </td>
 *       <td>ratio between magnetic moment of particle and kT</td>
 *      </tr><tr>
 *       <td>\b LAM_CENTER</td>
 *       <td>wavelength in Angstrom</td>
 *      </tr><tr>
 *       <td>\b D_LAM_LAM </td>
 *       <td>wavelength spread</td>
 *      </tr><tr>
 *       <td>\b SD </td>
 *       <td>sample to detector distance in meters</td>
 *      </tr><tr>
 *       <td>\b P_POL </td>
 *       <td> incident polarisation</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_langevin_static_up
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_static_up(scalar q, sasfit_param * p);

/**
 * \ingroup ff_langevin_static_up
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_static_up_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_langevin_static_up
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_static_up_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_langevin_static_up ################ */

/* ################ start ff_langevin_static_down ################ */
/**
 * \defgroup ff_langevin_static_down Langevin_static_down
 * \ingroup ff_plugins_stroboscopic_superparamagnet
 *
 * \brief stroboscopic SANS measurements on structures showing superparamagnetic behaviour
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b B0 </td>
 *       <td>amplitude of oscillating magnetic field </td>
 *      </tr><tr>
 *       <td>\b B1 </td>
 *       <td>amplitude of static magnetic field</td>
 *      </tr><tr>
 *       <td>\b NU </td>
 *       <td>frequency of magnetic field</td>
 *      </tr><tr>
 *       <td>\b RHO_0 </td>
 *       <td>instrumental phase shift</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b PSI_DEG </td>
 *       <td>angle between scattering vector q and magnetic field B in degree</td>
 *      </tr><tr>
 *       <td>\b SQ_AMPL </td>
 *       <td>structure factor amplitude (1 for no structure factor)</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b dummy </td>
 *       <td>not used</td>
 *      </tr><tr>
 *       <td>\b FNUC_FMAG </td>
 *       <td>ratio between nuclear and magnetic scattering length density contrast</td>
 *      </tr><tr>
 *       <td>\b MU_KT </td>
 *       <td>ratio between magnetic moment of particle and kT</td>
 *      </tr><tr>
 *       <td>\b LAM_CENTER</td>
 *       <td>wavelength in Angstrom</td>
 *      </tr><tr>
 *       <td>\b D_LAM_LAM </td>
 *       <td>wavelength spread</td>
 *      </tr><tr>
 *       <td>\b SD </td>
 *       <td>sample to detector distance in meters</td>
 *      </tr><tr>
 *       <td>\b P_POL </td>
 *       <td> incident polarisation</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_langevin_static_down
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_static_down(scalar q, sasfit_param * p);

/**
 * \ingroup ff_langevin_static_down
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_static_down_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_langevin_static_down
 *
 * \sa sasfit_langevin_polaris.h
 */
sasfit_langevin_polaris_DLLEXP scalar sasfit_ff_langevin_static_down_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_langevin_static_down ################ */

