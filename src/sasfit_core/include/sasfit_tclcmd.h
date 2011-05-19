/*
 * src/sasfit_core/include/sasfit_tclcmd.h
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

#ifndef SASFIT_TCLCMD_H
#define SASFIT_TCLCMD_H

#include <sasfit_common.h>
#include "sasfit_core_shared_exports.h"

/**
 * \file sasfit_tclcmd.h
 * Contains functions define custom Tcl commands.
 */


/**
 * A Tcl command which tries to load a given shared library filename.
 * On success, it returns a list of unique IDs of the loaded plugin 
 * description structures.
 * They are used as plugins which provide additional model function 
 * (form factor, structure factor, size distribution)
 * It matches the type of Tcl_ObjCmdProc.
 * @param clientData Arbitrary one-word value to pass.
 * @param interp Interpreter in which to create a new command.
 * @param objc The number of argument objects (including the command name).
 * @param objv The values of the arguments.
 *
 * \sa sasfit_plugin_load_all_cmd
 */
sasfit_core_DLLEXP int sasfit_load_plugin_cmd(ClientData     clientData,
                                              Tcl_Interp *   interp,
                                              int            objc,
                                              Tcl_Obj *CONST objv[]);

/**
 * \copydoc sasfit_get_maxpar
 * \sa sasfit_get_maxpar, sasfit_plugin_load_all_cmd
 */
sasfit_core_DLLEXP int sasfit_get_maxpar_cmd(ClientData     clientData, 
                                             Tcl_Interp *   interp, 
                                             int            objc, 
                                             Tcl_Obj *CONST objv[]);

/**
 * \copydoc sasfit_get_lib_prefix
 * \sa sasfit_get_lib_prefix, sasfit_plugin_load_all_cmd
 */
sasfit_core_DLLEXP int sasfit_get_lib_prefix_cmd(ClientData     clientData, 
                                                 Tcl_Interp *   interp, 
                                                 int            objc, 
                                                 Tcl_Obj *CONST objv[]);

/**
 * \copydoc sasfit_get_lib_suffix
 * \sa sasfit_get_lib_suffix, sasfit_plugin_load_all_cmd
 */
sasfit_core_DLLEXP int sasfit_get_lib_suffix_cmd(ClientData     clientData, 
                                                 Tcl_Interp *   interp, 
                                                 int            objc, 
                                                 Tcl_Obj *CONST objv[]);

/**
 */
sasfit_core_DLLEXP int sasfit_sd_analysis_cmd(ClientData     clientData,
                                              Tcl_Interp   * interp,
                                              int            argc,
                                              const char   * argv[]);
                                            //  int            objc,
                                            //  Tcl_Obj *CONST objv[]);

sasfit_core_DLLEXP int sasfit_covar_cmd(struct sasfit_CData * SASFIT_CData,
                                        Tcl_Interp          * interp, 
                                        int                   objc, 
                                        Tcl_Obj        *CONST objv[]);

#endif

