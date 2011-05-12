/*
 * src/sasfit_core/include/sasfit_analytPar_utils.h
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

#ifndef SASFIT_ANALYTPAR_UTILS_H
#define SASFIT_ANALYTPAR_UTILS_H

/**
 * \file sasfit_analytpar_utils.h
 */

/**
 * Calls a function to set a value extracted from the Tcl Interpreter.
 * \param type The type of the value to extract (int, double or bool).
 * \param name1 The variable or array name in Tcl scope.
 * \param name2 The element name within the array when name1 is an array.
 * \param func_name The function to call with the extracted value as argument.
 *
 * The flollowing variables have to be declared externally:
 * - <em>tcl_result(int)</em>
 * - <em>tcl_\<type\>(\<type\>)</em>
 * - <em>interp(Tcl_Interp *)</em>
 */
#define SF_TCL_GET_F(type, name1, name2, func_name) \
do { \
	tcl_result = sasfit_tcl_get_ ##type (interp, &(tcl_ ##type), name1, name2); \
	if (tcl_result != TCL_OK) { \
		sasfit_err("Could not read '%s' '%s' !\n", name1, name2); \
	} \
	(func_name)((tcl_ ##type)); \
} while(0)

/**
 * Sets a variable to a value extracted from the Tcl Interpreter.
 * \param var_type The type of the value to extract (int, double or bool).
 * \param name1 The variable or array name in Tcl scope.
 * \param name2 The element name within the array when name1 is an array.
 * \param target_name The variable name to assign the extracted value to.
 *
 * The flollowing variables have to be declared externally:
 * - <em>tcl_result(int)</em>
 * - <em>tcl_\<var_type\>(\<var_type\>)</em>
 * - <em>interp(Tcl_Interp *)</em>
 */
#define SF_TCL_GET(var_type, name1, name2, target_name) \
do { \
	tcl_result = sasfit_tcl_get_ ##var_type (interp, &(tcl_ ##var_type), name1, name2); \
	if (tcl_result != TCL_OK) { \
		sasfit_err("Could not read '%s' '%s' !\n", name1, name2); \
	} \
	(target_name) = (tcl_ ##var_type); \
} while(0)


#endif // this file
