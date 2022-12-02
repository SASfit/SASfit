/*
 * src/sasfit_ff/sasfit_ff_IsuperparamagneticFFpsiStrobo_old.c
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
 *   Ingo Bressler (ingo.bressler@bam.de)
 */

/*
 * reasoning:
 * The macro approach is chosen because it is the most straight forward
 * way to solve the problem: simple string based replication of code while 
 * reducing the overall number of lines of code. It avoids to write down 
 * the same calculations over and over again.
 * Drawback is a certain lack of readability (plus no syntax highlighting
 * by default).
 * A possible function based approach introduces several new problems:
 *   - large number of nested functions to be called inside a form factor 
 *     (plus inside possible internal integrations which increases the
 *     resource overhead exponentially)
 *   - parameter and function pointer passing, ordering, conversion
 *   - increased number of code lines by function declarations and parameter checks
 *   - difficulties with multiple parallel return values
 *   - provides a high dynamic functionality which is not used/required
 *
 * Guidelines for creation and modification of form factors:
 * (alpha.draft, possible redundant phrasing)
 *
 * local variables _always_ lowercase.
 * uppercase is reserved for 
 *   - macros (precompiler #defines)
 *   - enumerations (e.g. of parameter variables names)
 *
 * Parameters are accessed directly via a macro definition: V[] with the
 * parameter name between the brackets. The parameter name must be a member
 * of the enumeration 'variable_names' which is part of each form factor. 
 * Those enums are numbered by default starting from 0. The number of variable 
 * names must not exceed the maximum number of parameters MAXPAR which is 
 * defined in sasfit_common.
 *
 * files containing a form factor, size distrib or structure factor, start with
 * sasfit_ff_ sasfit_sd_ or sasfit_sq_ respectively
 * directory structure for form factors:
 *
 * <form factor base name>			(directory)
 *
 *   sasfit_ff_<form factor base name>.c	(code of form factor function)
 *   utils.c					(optional)
 *   include					(directory)
 *     
 *     public.h			(declarations of functions available to the outside)
 *     private.h		(declarations required for ff implementation)
 *     <useful name>.h		(optional, declarations which may be used by other
 *                               form factors too)
 *
 * following defines and structures as well as helper functions will be
 * accessible only locally in a 'private' sense for the purpose of 
 * implementing the form factor.
 * here private: Not declared next to the actual form factor funtion. But
 * in a separate header file which must not be included by the public
 * and only in another form factor *.c file for implementation.
 * Otherwise: _much_ longer names get necessary to prevent naming conflicts.
 * With more than 100 form factors it happens easily that at least a variable
 * name definition represents different numbers for different form factors.
 * (See the enum variable_names below)
 *
 * In a macro definition, the last line does not end with a semicolon ';'. This
 * way it has to be added after the macro name when it is written in the C code.
 * This is more consistent to the regular C syntax and prevents confusion about
 * missing semicolons.
 *
 * constraints by the C programming language:
 * variable declarations have to be the first thing written in a function
 * (and nowhere else)
 *   -> that's why the macro of variable declaration/initialization is separated
 *      from the code section where the calculation occurs
 */

