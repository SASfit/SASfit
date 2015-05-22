/*
 * src/sasfit_common/include/sasfit_mem.h
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

#ifndef SASFIT_MEM_H
#define SASFIT_MEM_H

#include "sasfit_common_shared_exports.h"

/**
 * \file sasfit_mem.h
 * \ingroup sasfit_mem
 */

/**
 * \defgroup sasfit_mem Memory Management
 * \ingroup sasfit_common
 * Utilities for managing data structures which required separate allocation.
 */
/*@{*/

// hide direct functions from plugins
#ifndef MAKE_SASFIT_PLUGIN

/**
 * Allocates memory for a vector of specified size.
 * <b>This function serves only for fixing existing code. 
 * Please use gsl_vector of the GSL library instead.</b>
 * @param vec The address of the pointer.
 * @param size The size of this vector (number of scalars).
 * @sa sasfit_free_vector
 */
sasfit_common_DLLEXP void sasfit_init_vector(scalar ** vec, int size);

/**
 * Frees a vector allocated by sasfit_init_vector.
 * <b>This function serves only for fixing existing code. 
 * Please use gsl_vector of the GSL library instead.</b>
 * @param vec Address to the pointer to a vector the free.
 * 	      Will be set to NULL afterwards.
 * @sa sasfit_init_vector
 */
sasfit_common_DLLEXP void sasfit_free_vector(scalar ** vec);

/*@}*/
#endif // MAKE_SASFIT_PLUGIN
#endif // file

