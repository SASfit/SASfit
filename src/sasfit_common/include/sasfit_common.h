/*
 * src/sasfit_common/include/sasfit_common.h
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
 *   Ingo Bressler (ingo@cs.tu-berlin.de)
 */

#ifndef SASFIT_COMMON_H
#define SASFIT_COMMON_H

#include "bool.h"
#include "math.h"
#include "sasfit_common_shared_exports.h"
#include "sasfit_timer.h"
#include "sasfit_constants.h"
#include "sasfit_function.h"
#include "sasfit_utils.h"
#include "sasfit_mem.h"
#include "sasfit_message.h"
#include "sasfit_error_common.h"
#include "sasfit_eps.h"
#include "sasfit_common_stubs.h"

#define OOURA_DOUBLE_EXP_QUADRATURE 0
#define OOURA_CLENSHAW_CURTIS_QUADRATURE 1
#define GSL_CQUAD 2
#define GSL_QAG 3
#define H_CUBATURE 4
#define P_CUBATURE 5
#define NR_QROMB 6
#define Sphere_Lebedev 7
#define TanhSinh 8
#define GSL_QAGS 9
#define GSL_QNG 10
#define GSL_G962D 11
#define GSL_GAUSSLEGENDRE 12
#define GSL_CHEBYSHEV1 13
#define GSL_CHEBYSHEV2 14
#define GSL_GEGENBAUER 15
#define GSL_EXPONENTIAL 16
#define GSL_LAGUERRE 17
#define GSL_JACOBI 18
#define GSL_MC_MISER 19
#define GSL_MC_VEGAS 20
#define GSL_MC_PLAIN 21
#define TANHSINH_1 22
#define TANHSINH_2 23

#define SPHAVG_GSL_2D_GAUSSLEGENDRE 0
#define SPHAVG_Lebedev 1
#define SPHAVG_FIBONACCI 2
#define SPHAVG_HCUBATURE 3
#define SPHAVG_PCUBATURE 4
#define SPHAVG_MC_MISER 5
#define SPHAVG_MC_VEGAS 6
#define SPHAVG_MC_PLAIN 7
#define SPHAVG_SPHERICAL_T_DESIGN 8



// helper for quoting compiler definitions
#define __Q(x) #x
#define SASFIT_QUOTE(x) __Q(x)

#define SASFIT_CONCAT(a, b) a ## b

#ifdef SASFIT_COMMON_STUBS
    #undef SASFIT_COMMON_STUBS
#endif
/// Uses the stub pointer getter from this file. Is overridden by plugin specific getter elsewhere.
#define SASFIT_COMMON_STUBS sasfit_common_stubs_ptr

/// Returns address of lookup table for exported sasfit_common functions.
/// *dllexp* does not work for plugins on windows because they are not linked against libsasfit.dll
sasfit_common_DLLEXP
const sasfit_common_stubs_t * sasfit_common_stubs_ptr();

#endif // file
