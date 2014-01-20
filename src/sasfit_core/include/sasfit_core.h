/*
 * src/sasfit_core/include/sasfit_core.h
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

#ifndef SASFIT_CORE_H
#define SASFIT_CORE_H

#include <tcl.h>
#include <sasfit_common.h>
#include "sasfit_analytpar.h"
#include "sasfit_error_core.h"
#include "sasfit_vector.h"
#include "sasfit_tclcmd.h"
#include "sasfit_env.h"
#include "sasfit_2d.h"
#include "sasfit_analytpar_utils.h"
#include "sasfit_core_shared_exports.h"

/**
 * \file sasfit_core.h
 * Contains adapter-functions between old and new parts of sasfit
 */

/**
 * Calculates the size distribution.
 * \param x Position at which the size distribution is evaluated.
 * \param a Array of paramters.
 * \param sd Pointer to size distribution function structure.
 * \param dparam Array of parameter indices to differentiate.
 * \param err Contains TCL_OK if the value was calculated successfully, 
 *            TCL_ERROR if an error occurred. The sasfit_param structure 
 *            inside sd contains an error string.
 * @sa sasfit_sq, sasfit_ff
 */
sasfit_core_DLLEXP scalar sasfit_sd(scalar x, scalar * a, 
                                    sasfit_function * sd, 
                                    int * dparam, int * err);

/**
 * Initializes the size distribution function structure.
 * Sets the function Pointers according to the selected algorithms from the GUI in AP.
 * @param ap Pointer to sasfit_analytpar structure which contains all data from the GUI.
 * @sa sasfit_sd
 */
sasfit_core_DLLEXP int sasfit_sd_init(sasfit_analytpar * ap);

sasfit_core_DLLEXP scalar sasfit_sq(scalar q, scalar * a, 
                                    sasfit_function * sq, 
                                    int * dparam, int * err);

/**
 * Initializes the structure factor function structure.
 * Sets the function Pointers according to the selected algorithms from the GUI in AP.
 * @param ap Pointer to sasfit_analytpar structure which contains all data from the GUI.
 * @sa sasfit_sq
 */
sasfit_core_DLLEXP int sasfit_sq_init(sasfit_analytpar * ap);

sasfit_core_DLLEXP scalar sasfit_ff(scalar q, scalar * a, 
                                    sasfit_function * ff, 
                                    int * dparam, int * err);

sasfit_core_DLLEXP int sasfit_ff_init(sasfit_analytpar * ap);

sasfit_core_DLLEXP scalar sasfit_volume(scalar x, scalar * l, 
                                        sasfit_function * ff, 
                                        int distr, int * err);

/**
 * Fills a sasfit_function with model function pointers.
 * In \e sf the identifier of the requested model function is stored as 
 * string. It is converted to integer at the according function record
 * is retrieved in constant time.
 * \param sf Pointer to a function for execution with the requested model 
 * function identifier stored as string in sasfit_function.typestr.
 * \returns TCL_OK on successful retrieval, TCL_ERROR otherwise.
 */
int sasfit_function_select_id(sasfit_function * sf);

#endif

