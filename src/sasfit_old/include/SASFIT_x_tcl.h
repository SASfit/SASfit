/*
 * src/sasfit_old/include/SASFIT_x_tcl.h
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
 */

/*
 * Joachim Kohlbrecher
 * 12.12.2004
 */

#ifndef __SASFIT_X_TCL
#define __SASFIT_X_TCL

int save_AP(Tcl_Interp *, const char *,sasfit_analytpar *,int,float);
int save_GlobalAP(Tcl_Interp *,char *,sasfit_analytpar *,sasfit_commonpar *,int,float);
int save_covar_alpha(Tcl_Interp *,char *,float **,float **,int);
int get_covar_alpha(Tcl_Interp *,char **,float ***,float ***,int);

int get_GlobalAP(Tcl_Interp *,char **,sasfit_analytpar **,sasfit_commonpar *,int *,
		   float *,int *,float ***,float ***,float ***,float ***,float ***,int**,int**);

#endif
