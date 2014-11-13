/*
 * src/plugins/rod/include/sasfit_rod.h
 *
 * Copyright (c) 2008-2009, Paul Scherrer Institute (PSI)
 *
 * This file is part of SASfit.
 *
 * SASfit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SASfit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SASfit.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   Ingo Bressler (ingo@cs.tu-berlin.de)
 */

#ifndef SASFIT_PLUGIN_ROD_H
#define SASFIT_PLUGIN_ROD_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_rod.h
 * Public available functions and descriptions of the rod plugin.
 */

/**
 * \def sasfit_rod_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_rod_EXPORTS) 
	#ifdef sasfit_rod_DLLEXP
	#undef sasfit_rod_DLLEXP
	#endif
	#define sasfit_rod_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_rod_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_rod_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_rod ################ */
/** 
 * \defgroup ff_rod Rod
 * \ingroup ff_plugins
 *
 * \brief F^2(Q,L,eta) = (eta*L)^2* (2.0*si(Q*L)/(Q*L)-pow(sin(Q*L*0.5)/(Q*L*0.5),2.0)) 
 *
 * \image html form_factor/cylindrical_obj/rod.png "a rod-like object"
 * \image latex form_factor/cylindrical_obj/rod.eps "a rod-like object" width=10cm
 *
 * \f[
 *    I_{Rod}(Q,L,\Delta\eta) = \Delta\eta^2 L^2 \\
 *    \left( \frac{2}{QL} Si(QL) - \frac{\sin(QL/2)}{QL/2} \right)
 * \f]
 * with
 * \f[
 *    Si(x) = \int_0^x \! \frac{\sin t}{t} \, \, dt
 * \f]
 * and
 * \f[
 *    \lim_{Q=0} I_{Rod}(Q,L,\Delta\eta) = \Delta\eta^2 L
 * \f]
 *
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b length</td>
 *       <td>The length of the rod in \<unit\>.</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta</td>
 *       <td>The scattering length density \f$ \Delta\eta \f$.</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_rod
 *
 * \sa sasfit_rod.h, form_fac
 */
sasfit_rod_DLLEXP scalar sasfit_ff_rod(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod
 *
 * \sa sasfit_rod.h, form_fac
 */
sasfit_rod_DLLEXP scalar sasfit_ff_rod_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rod
 *
 * \sa sasfit_rod.h, form_fac
 */
sasfit_rod_DLLEXP scalar sasfit_ff_rod_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rod ################ */


#endif // this file

