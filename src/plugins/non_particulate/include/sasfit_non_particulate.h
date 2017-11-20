/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_NON_PARTICULATE_H
#define SASFIT_PLUGIN_NON_PARTICULATE_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_non_particulate.h
 * Public available functions and descriptions of the non_particulate plugin.
 */

/**
 * \def sasfit_non_particulate_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_non_particulate_EXPORTS)
	#ifdef sasfit_non_particulate_DLLEXP
	#undef sasfit_non_particulate_DLLEXP
	#endif
	#define sasfit_non_particulate_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_non_particulate_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_non_particulate_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_generalized_guinier_porod_law ################ */
/** 
 * \defgroup ff_generalized_guinier_porod_law generalized Guinier-Porod Law
 * \ingroup ff_non-particulate
 *
 * \brief \<some brief description of generalized Guinier-Porod Law function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b G2</td>
 *       <td>scaling factor</td>
 *      </tr><tr>
 *       <td>\b s2</td>
 *       <td>s2: potential law for the small q-region, dimensionality parameter: (3>s1>=s2)</td>
 *      </tr><tr>
 *       <td>\b Rg2</td>
 *       <td>radius of gyration (larger dimension, Rg1<Rg2) </td>
 *      </tr><tr>
 *       <td>\b s1</td>
 *       <td>s1: potential law in intermediate q-range, dimensionality parameter: (3>s1>=s2)</td>
 *      </tr><tr>
 *       <td>\b Rg1</td>
 *       <td>radius of gyration (smaller dimension, Rg1<Rg2)</td>
 *      </tr><tr>
 *       <td>\b m</td>
 *       <td>m: potential law at large q-values (m>=s1)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_generalized_guinier_porod_law
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_generalized_guinier_porod_law(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_guinier_porod_law
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_generalized_guinier_porod_law_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_generalized_guinier_porod_law
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_generalized_guinier_porod_law_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_generalized_guinier_porod_law ################ */

/* ################ start ff_extended_guinier_law ################ */
/** 
 * \defgroup ff_extended_guinier_law extended Guinier law
 * \ingroup ff_non-particulate
 *
 * \brief ((a==0) ? I(Q) = I0 exp(-Ra^2Q^2/3) : I(Q) = a pi/Q^a I0 exp(-Ra^2Q^2/(3-a))); a=0: sphere, a=1: rod, a=2: lamellar 
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Monodisperse
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I0</td>
 *       <td>scakling factor</td>
 *      </tr><tr>
 *       <td>\b a</td>
 *       <td>dimensionality parameter: a=0:sphere, a=1:rod, a=2:lamellar </td>
 *      </tr><tr>
 *       <td>\b Ra</td>
 *       <td>radius of gyration</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_extended_guinier_law
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_extended_guinier_law(scalar q, sasfit_param * p);

/**
 * \ingroup ff_extended_guinier_law
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_extended_guinier_law_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_extended_guinier_law
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_extended_guinier_law_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_extended_guinier_law ################ */

/* ################ start ff_beaucage_exppowlaw_2 ################ */
/** 
 * \defgroup ff_beaucage_exppowlaw_2 Beaucage ExpPowLaw 2
 * \ingroup ff_non-particulate
 *
 * \brief Unified Exponential Power Law according to Beaucage
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref Delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b G</td>
 *       <td>Guinier prefactor</td>
 *      </tr><tr>
 *       <td>\b B</td>
 *       <td>prefactor specific to the type of power-law</td>
 *      </tr><tr>
 *       <td>\b Rg_i</td>
 *       <td>size of large structure</td>
 *      </tr><tr>
 *       <td>\b Rg_i+1</td>
 *       <td>size of small structure</td>
 *      </tr><tr>
 *       <td>\b k</td>
 *       <td>scaling parameter, should be close to 1</td>
 *      </tr><tr>
 *       <td>\b P</td>
 *       <td>4<P<3: suface fractal, P<3: mass fractal, P>4 diffuse interface, P=2 for Gaussian polymer, P=1 for rod-like Kuhn steps</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_beaucage_exppowlaw_2
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_beaucage_exppowlaw_2(scalar q, sasfit_param * p);

/**
 * \ingroup ff_beaucage_exppowlaw_2
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_beaucage_exppowlaw_2_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_beaucage_exppowlaw_2
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_beaucage_exppowlaw_2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_beaucage_exppowlaw_2 ################ */

/* ################ start ff_beaucage_exppowlaw ################ */
/** 
 * \defgroup ff_beaucage_exppowlaw Beaucage ExpPowLaw
 * \ingroup ff_non-particulate
 *
 * \brief Unified Exponential Power Law according to Beaucage
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b G</td>
 *       <td>Guinier prefactor</td>
 *      </tr><tr>
 *       <td>\b B</td>
 *       <td>prefactor specific to the type of power-law</td>
 *      </tr><tr>
 *       <td>\b Gs</td>
 *       <td>Guinier prefactor</td>
 *      </tr><tr>
 *       <td>\b Bs</td>
 *       <td>prefactor specific to the type of power-law</td>
 *      </tr><tr>
 *       <td>\b Rg</td>
 *       <td>size of large-scale polymer coil</td>
 *      </tr><tr>
 *       <td>\b Rsub</td>
 *       <td>size of small-scale Kuhn steps - size of subunits of large-scale polymer</td>
 *      </tr><tr>
 *       <td>\b Rs</td>
 *       <td>size of small-scale Kuhn steps - size of subunits of large-scale polymer</td>
 *      </tr><tr>
 *       <td>\b P</td>
 *       <td>Ps should be equal 1 for rod-like Kuhn steps</td>
 *      </tr><tr>
 *       <td>\b Ps</td>
 *       <td></td>
 *      </tr></table>
 */

/**
 * \ingroup ff_beaucage_exppowlaw
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_beaucage_exppowlaw(scalar q, sasfit_param * p);

/**
 * \ingroup ff_beaucage_exppowlaw
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_beaucage_exppowlaw_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_beaucage_exppowlaw
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_beaucage_exppowlaw_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_beaucage_exppowlaw ################ */

/* ################ start ff_spinodal ################ */
/** 
 * \defgroup ff_spinodal Spinodal
 * \ingroup ff_non-particulate
 *
 * \brief \<some brief description of Spinodal function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Imax</td>
 *       <td>scattering intensity at peak position I(Q=Qmax)</td>
 *      </tr><tr>
 *       <td>\b Qmax</td>
 *       <td>position of peak maximum</td>
 *      </tr><tr>
 *       <td>\b gamma</td>
 *       <td>the exponent gamma is equal to d+1 for off-critical concentration mixtures\nand 2d for critical concentration mixtures (d:dimension)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_spinodal
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_spinodal(scalar q, sasfit_param * p);

/**
 * \ingroup ff_spinodal
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_spinodal_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_spinodal
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_spinodal_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_spinodal ################ */

/* ################ start ff_debye_anderson_brumberger ################ */
/** 
 * \defgroup ff_debye_anderson_brumberger Debye-Anderson-Brumberger
 * \ingroup ff_non-particulate
 *
 * \brief I(Q,Rg,I0) = (pi*eta)^2*(2xi)^6/(1+Q^2*xi^2)^2
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
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_debye_anderson_brumberger
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_debye_anderson_brumberger(scalar q, sasfit_param * p);

/**
 * \ingroup ff_debye_anderson_brumberger
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_debye_anderson_brumberger_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_debye_anderson_brumberger
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_debye_anderson_brumberger_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_debye_anderson_brumberger ################ */

/* ################ start ff_g_dab ################ */
/** 
 * \defgroup ff_g_dab gDAB
 * \ingroup ff_non-particulate
 *
 * \brief I(Q,Rg,I0) = (4 pi eta (1+2H))^2 (xi)^6/(1+Q^2*xi^2)^(3/2+H)
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
 *       <td>Hurst exponent</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td>dummy</td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>scattering length density contrast</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_g_dab
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_g_dab(scalar q, sasfit_param * p);

/**
 * \ingroup ff_g_dab
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_g_dab_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_g_dab
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_g_dab_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_g_dab ################ */

/* ################ start ff_ornstein_zernike ################ */
/** 
 * \defgroup ff_ornstein_zernike Ornstein-Zernike
 * \ingroup ff_non-particulate
 *
 * \brief I(Q)=I0/(1+Q^2*xi^2)
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering I(Q=0)</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>correlation length</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_ornstein_zernike
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_ornstein_zernike(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ornstein_zernike
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_ornstein_zernike_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_ornstein_zernike
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_ornstein_zernike_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_ornstein_zernike ################ */

/* ################ start ff_broad_peak ################ */
/** 
 * \defgroup ff_broad_peak Broad-Peak
 * \ingroup ff_non-particulate
 *
 * \brief I(Q)=I0/(1+(|q-q0|*xi)^m)^p
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
 *       <td>\b xi</td>
 *       <td>correlation length</td>
 *      </tr><tr>
 *       <td>\b q0</td>
 *       <td>peak position which is related to the d-spacing as q0 = 2pi/d</td>
 *      </tr><tr>
 *       <td>\b m</td>
 *       <td>I(Q)=I0/(1+(|q-q0|*xi)^m)^p</td>
 *      </tr><tr>
 *       <td>\b p</td>
 *       <td>I(Q)=I0/(1+(|q-q0|*xi)^m)^p</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_broad_peak
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_broad_peak(scalar q, sasfit_param * p);

/**
 * \ingroup ff_broad_peak
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_broad_peak_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_broad_peak
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_broad_peak_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_broad_peak ################ */

/* ################ start ff_teubner_strey ################ */
/** 
 * \defgroup ff_teubner_strey Teubner-Strey
 * \ingroup ff_non-particulate
 *
 * \brief I(Q)=eta^2*8*pi/xi/(a^2-2bq^2+q^4)\nwith k=2pi/d, a=k^2+1/xi^2
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b eta</td>
 *       <td>correlation length</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>correlation length</td>
 *      </tr><tr>
 *       <td>\b d</td>
 *       <td>characteristic domain size (the periodicity)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_teubner_strey
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_teubner_strey(scalar q, sasfit_param * p);

/**
 * \ingroup ff_teubner_strey
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_teubner_strey_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_teubner_strey
 *
 * \sa sasfit_non_particulate.h, ff_non-particulate
 */
sasfit_non_particulate_DLLEXP scalar sasfit_ff_teubner_strey_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_teubner_strey ################ */


#endif // this file

