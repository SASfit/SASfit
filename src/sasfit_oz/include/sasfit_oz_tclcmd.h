/*
 * src/sasfit_oz/sasfit_oz_tclcmd.h
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

#ifndef SASFIT_OZ_TCLCMD_H
#define SASFIT_OZ_TCLCMD_H

#include <tcl.h>
#include <sasfit_common.h>
#include "sasfit_oz_shared_exports.h"

sasfit_oz_DLLEXP int
sasfit_oz_calc_cmd(ClientData     clientData,
                Tcl_Interp *   interp,
                int            objc,
                Tcl_Obj *CONST objv[]);

sasfit_oz_DLLEXP int
sasfit_oz_assign_data_sq_cmd(ClientData     clientData,
                Tcl_Interp *   interp,
                int            objc,
                Tcl_Obj *CONST objv[]);

#endif

