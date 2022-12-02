/*
 * src/sasfit_core/include/sasfit_env.h
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
 *   Ingo Bressler (ingo.bressler@bam.de)
 *    Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_ENV_H
#define SASFIT_ENV_H

/**
 * Stores information about the SASfit runtime environment.
 */
typedef struct
{
	/// TCL-Interpreter which initialized the SASfit library.
	Tcl_Interp * interp;
	int interrupt;
} sasfit_env_t;

/**
 * Returns the TCL-Interpreter, SASfit was initialized with.
 * \returns Pointer to the current TCL-Interpreter.
 */
Tcl_Interp * sasfit_env_get_interp(void);

/**
 * Set the current TCL-Interpreter of SASfit.
 * \param interp Pointer to the current TCL-Interpreter.
 */
void sasfit_env_set_interp(Tcl_Interp * interp);

/**
 * Returns the SASFIT_CONTINUE=1 for continue and SASFIT_INTERRUPT=2 for interrupt signal
 * \returns Pointer to the current TCL-Interpreter.
 */
// Tcl_Interp * sasfit_env_get_interp(void);

/**
 * Set the current TCL-Interpreter of SASfit.
 * \param interp Pointer to the current TCL-Interpreter.
 */
// void sasfit_env_set_interp(Tcl_Interp * interp);

#endif

