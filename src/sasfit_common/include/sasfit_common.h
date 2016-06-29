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

// hide direct functions from plugins
#ifndef MAKE_SASFIT_PLUGIN


// global variables/storage

/// Lookup table for exported sasfit_common functions.
sasfit_common_stubs_t	sasfit_common_stubs;

#endif // MAKE_SASFIT_PLUGIN
#endif // file

