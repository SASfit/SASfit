/*
 * src/sasfit_core/include/sasfit_error_core.h
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
 */

#ifndef SASFIT_CORE_ERROR_H
#define SASFIT_CORE_ERROR_H

#include "sasfit_core_shared_exports.h"

/**
 * Outputs an error in Tcl scope.
 * Uses the \e puts Tcl command to write the message to stderr of the 
 * current interpreter. Used as alternate error handler.
 * \param format \e printf() formatted error message.
 * \param arglist List of arguments for the message in \e format.
 * \sa sasfit_msg_handler
 */
sasfit_core_DLLEXP void sasfit_tcl_err(const char * format, va_list arglist);

/**
 * Outputs a message in Tcl scope.
 * Uses the \e puts Tcl command to write the message to stdout of the 
 * current interpreter. Used as alternate message handler.
 * \param format \e printf() formatted error message.
 * \param arglist List of arguments for the message in \e format.
 * \sa sasfit_msg_handler
 */
sasfit_core_DLLEXP void sasfit_tcl_out(const char * format, va_list arglist);

#endif

