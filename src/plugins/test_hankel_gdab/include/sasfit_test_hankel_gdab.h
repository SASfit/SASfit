/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_TEST_HANKEL_GDAB_H
#define SASFIT_PLUGIN_TEST_HANKEL_GDAB_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_test_hankel_gdab.h
 * Public available functions and descriptions of the test_hankel_gdab plugin.
 */

/**
 * \def sasfit_test_hankel_gdab_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_test_hankel_gdab_EXPORTS)
	#ifdef sasfit_test_hankel_gdab_DLLEXP
	#undef sasfit_test_hankel_gdab_DLLEXP
	#endif
	#define sasfit_test_hankel_gdab_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_test_hankel_gdab_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_test_hankel_gdab_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_hankel_gdab_ ################ */
/** 
 * \defgroup ff_hankel_gdab_ Hankel(gDAB)
 * \ingroup ff_user1
 *
 * \brief \<some brief description of Hankel(gDAB) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
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
 * \ingroup ff_hankel_gdab_
 *
 * \sa sasfit_test_hankel_gdab.h, ff_user1
 */
sasfit_test_hankel_gdab_DLLEXP scalar sasfit_ff_hankel_gdab_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_hankel_gdab_
 *
 * \sa sasfit_test_hankel_gdab.h, ff_user1
 */
sasfit_test_hankel_gdab_DLLEXP scalar sasfit_ff_hankel_gdab__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_hankel_gdab_
 *
 * \sa sasfit_test_hankel_gdab.h, ff_user1
 */
sasfit_test_hankel_gdab_DLLEXP scalar sasfit_ff_hankel_gdab__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_hankel_gdab_ ################ */

/* ################ start ff_hankel2_gdab ################ */
/** 
 * \defgroup ff_hankel2_gdab Hankel2(gDAB)
 * \ingroup ff_user1
 *
 * \brief test of Hankel2(gDAB)
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b a</td>
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
 * \ingroup ff_hankel2_gdab
 *
 * \sa sasfit_test_hankel_gdab.h, ff_user1
 */
sasfit_test_hankel_gdab_DLLEXP scalar sasfit_ff_hankel2_gdab(scalar q, sasfit_param * p);

/**
 * \ingroup ff_hankel2_gdab
 *
 * \sa sasfit_test_hankel_gdab.h, ff_user1
 */
sasfit_test_hankel_gdab_DLLEXP scalar sasfit_ff_hankel2_gdab_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_hankel2_gdab
 *
 * \sa sasfit_test_hankel_gdab.h, ff_user1
 */
sasfit_test_hankel_gdab_DLLEXP scalar sasfit_ff_hankel2_gdab_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_hankel2_gdab ################ */

#endif // this file

