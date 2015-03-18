/*
 * src/sasfit_common/include/sasfit_constants.h
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

#ifndef SASFIT_CONSTANTS_H
#define SASFIT_CONSTANTS_H

/**
 * \file sasfit_constants.h
 * \ingroup sasfit_constants
 */

/**
 * \defgroup sasfit_constants Constants
 * \ingroup sasfit_common
 * Basic constants and types are defined here.
 */

/**
 * \ingroup sasfit_constants
 * Maximum number of parameter values in sasfit_param
 */
#define MAXPAR 40

/**
 * \ingroup sasfit_constants
 * A maximum length definition for strings. Used in sasfit_function.
 */
#define STRLEN 256

/**
 * \ingroup sasfit_constants
 * A common return value for math functions.
 * Used at error in model functions.
 * May be overwritten locally.
 */
#define SASFIT_RETURNVAL_ON_ERROR 0.0

typedef double scalar; //!< Type of most floating point values we calculate with.

#endif

