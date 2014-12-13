/*
 * src/sasfit_old/SASFIT_indexx.c
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
 * original file indexx.c
 * changes: #include "SASFIT_nr.h"
 *          name of subroutine changed: indexx -> SASFITindexx
 *          all vectors and materices start now with [0] or [0][0]
 *          as there first element, therefore all loop counters has been
 *          decremented by 1
 * Joachim Kohlbrecher
 * 15.05.98
 */


#include "include/SASFIT_nr.h"

void SASFITindexx(n,arrin,indx)
/* 
 * index an array arrin[0..n-1], i.e. outputs the array indx[0..n-1]
 * such that arrin[indx[j]] is in ascending order for j=0,1,...,n-1. 
 * The input quantities n and arrin are not changed.
 */
int n,indx[];
float arrin[];
{
	int l,j,ir,indxt,i;
	float q;

	for (j=0;j<=n-1;j++) indx[j]=j;
	l=((n-1) >> 1);
	ir=n-1;
	for (;;) {
		if (l > 0)
			q=arrin[(indxt=indx[--l])];
		else {
			q=arrin[(indxt=indx[ir])];
			indx[ir]=indx[0];
			if (--ir == 0) {
				indx[0]=indxt;
				return;
			}
		}
		i=l;
		j=(l << 1)+1;
		while (j <= ir) {
			if (j < ir && arrin[indx[j]] < arrin[indx[j+1]]) j++;
			if (q < arrin[indx[j]]) {
				indx[i]=indx[j];
				j += (i=j);
			}
			else j=ir+1;
		}
		indx[i]=indxt;
	}
}

void indexx(n,arrin,indx)
int n,indx[];
float arrin[];
{
	int l,j,ir,indxt,i;
	float q;

	for (j=1;j<=n;j++) indx[j]=j;
	l=(n >> 1) + 1;
	ir=n;
	for (;;) {
		if (l > 1)
			q=arrin[(indxt=indx[--l])];
		else {
			q=arrin[(indxt=indx[ir])];
			indx[ir]=indx[1];
			if (--ir == 1) {
				indx[1]=indxt;
				return;
			}
		}
		i=l;
		j=l << 1;
		while (j <= ir) {
			if (j < ir && arrin[indx[j]] < arrin[indx[j+1]]) j++;
			if (q < arrin[indx[j]]) {
				indx[i]=indx[j];
				j += (i=j);
			}
			else j=ir+1;
		}
		indx[i]=indxt;
	}
}

void SASFIT2indexx(n,arrin,indx)
/* 
 * index an array arrin[0..n-1], i.e. outputs the array indx[0..n-1]
 * such that arrin[indx[j]-1] is in ascending order for j=0,1,...,n-1. 
 * The input quantities n and arrin are not changed.
 */
int n,indx[];
float arrin[];
{
	int l,j,ir,indxt,i;
	float q;

	for (j=1;j<=n;j++) indx[j-1]=j;
	l=(n >> 1) + 1;
	ir=n;
	for (;;) {
		if (l > 1)
			q=arrin[(indxt=indx[(--l)-1])-1];
		else {
			q=arrin[(indxt=indx[ir-1])-1];
			indx[ir-1]=indx[0];
			if (--ir == 1) {
				indx[0]=indxt;
				return;
			}
		}
		i=l;
		j=l << 1;
		while (j <= ir) {
			if (j < ir && arrin[indx[j-1]-1] < arrin[indx[j+1-1]-1]) j++;
			if (q < arrin[indx[j-1]-1]) {
				indx[i-1]=indx[j-1];
				j += (i=j);
			}
			else j=ir+1;
		}
		indx[i-1]=indxt;
	}
}

