/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_HAMMOUDA_BRANCHED_H
#define SASFIT_PLUGIN_HAMMOUDA_BRANCHED_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_hammouda_branched.h
 * Public available functions and descriptions of the hammouda_branched plugin.
 */

/**
 * \def sasfit_hammouda_branched_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_hammouda_branched_EXPORTS)
	#ifdef sasfit_hammouda_branched_DLLEXP
	#undef sasfit_hammouda_branched_DLLEXP
	#endif
	#define sasfit_hammouda_branched_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_hammouda_branched_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_hammouda_branched_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_fract_branched_polym ################ */
/** 
 * \defgroup ff_fract_branched_polym fract branched polym
 * \ingroup ff_polymer_branched
 *
 * \brief doi:10.1002/mats.201100111
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
 *       <td>\b vm</td>
 *       <td>excluded volume</td>
 *      </tr><tr>
 *       <td>\b c</td>
 *       <td>scaling</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>forward scattering I(q=0)</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_fract_branched_polym
 *
 * \sa sasfit_hammouda_branched.h, ff_polymer_branched
 */
sasfit_hammouda_branched_DLLEXP scalar sasfit_ff_fract_branched_polym(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fract_branched_polym
 *
 * \sa sasfit_hammouda_branched.h, ff_polymer_branched
 */
sasfit_hammouda_branched_DLLEXP scalar sasfit_ff_fract_branched_polym_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fract_branched_polym
 *
 * \sa sasfit_hammouda_branched.h, ff_polymer_branched
 */
sasfit_hammouda_branched_DLLEXP scalar sasfit_ff_fract_branched_polym_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_fract_branched_polym ################ */


#endif // this file

