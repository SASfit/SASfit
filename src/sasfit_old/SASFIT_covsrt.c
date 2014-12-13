/*
 * src/sasfit_old/SASFIT_covsrt.c
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
 * original file covsrt.c
 * changes: #include "SASFIT_nr.h"
 *          name of subroutine changed: covsrt -> SASFITcovsrt
 *          all vectors and materices start now with [0] or [0][0]
 *          as there first element, therefore all loop counters has been
 *          decremented by 1
 * Joachim Kohlbrecher
 * 08.05.98
 */

#include <tcl.h>
#include "include/SASFIT_nr.h"


void SASFITcovsrt(interp,covar,ma,lista,mfit,error)
bool *error;
Tcl_Interp *interp;
float **covar;
int ma,lista[],mfit;
{
	int i,j;
	float swap;

	for (j=0;j<(ma-1);j++)
		for (i=j+1;i<=(ma-1);i++) covar[i][j]=0.0;
	for (i=0;i<(mfit-1);i++)
		for (j=i+1;j<=(mfit-1);j++) {
			if (lista[j] > lista[i])
				covar[lista[j]][lista[i]]=covar[i][j];
			else
				covar[lista[i]][lista[j]]=covar[i][j];
		}
	swap=covar[0][0];
	for (j=0;j<=(ma-1);j++) {
		covar[0][j]=covar[j][j];
		covar[j][j]=0.0;
	}
	covar[lista[0]][lista[0]]=swap;
	for (j=1;j<=(mfit-1);j++) covar[lista[j]][lista[j]]=covar[0][j];
	for (j=1;j<=(ma-1);j++)
		for (i=0;i<=j-1;i++) covar[i][j]=covar[j][i];
}
