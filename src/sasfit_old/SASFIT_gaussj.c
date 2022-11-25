/*
 * src/sasfit_old/SASFIT_gaussj.c
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
 * original file gaussj.c
 * changes: #include "SASFIT_nr.h"
 *          name of subroutine changed: gaussj -> SASFITgaussj
 *          all vectors and materices start now with [0] or [0][0]
 *          as there first element, therefore all loop counters has been
 *          decremented by 1
 * Joachim Kohlbrecher
 * 08.05.98
 */

#include <math.h>
#include <stdio.h>
#include <tcl.h>
#include "include/SASFIT_nr.h"


#define SWAP(a,b) {float temp=(a);(a)=(b);(b)=temp;}

void SASFITgaussj(interp,a,n,b,m,error)
Tcl_Interp *interp;
float **a,**b;
bool  *error;
int n,m;
{
	int *indxc,*indxr,*ipiv;
	int i=0,icol=0,irow=0,j=0,k=0,l=0,ll=0;
	float big,dum,pivinv;

	*error = FALSE;
	indxc=ivector(0,n-1);
	indxr=ivector(0,n-1);
	ipiv=ivector(0,n-1);
	for (j=0;j<=(n-1);j++) {ipiv[j]=0; indxr[j]=0; indxc[j]=0;}
	for (i=0;i<=(n-1);i++) {
		big=0.0;
		for (j=0;j<=(n-1);j++)
			if (ipiv[j] != 1)
				for (k=0;k<=(n-1);k++) {
					if (ipiv[k] == 0) {
						if (fabs(a[j][k]) >= big) {
							big=fabs(a[j][k]);
							irow=j;
							icol=k;
						}
					} else if (ipiv[k] > 1) {
					   sasfit_err("GAUSSJ: Singular Matrix-1\n");
					   *error = TRUE;
					   free_ivector(ipiv,0,n-1);
					   free_ivector(indxr,0,n-1);
					   free_ivector(indxc,0,n-1);
					   return;
					}
				}
		++(ipiv[icol]);
		if (irow != icol) {
			for (l=0;l<=(n-1);l++) SWAP(a[irow][l],a[icol][l])
			for (l=0;l<=(m-1);l++) SWAP(b[irow][l],b[icol][l])
		}
		indxr[i]=irow;
		indxc[i]=icol;
		if (a[icol][icol] == 0.0) {
			sasfit_err("GAUSSJ: Singular Matrix-2\n");
//			for (j=0;j<=(n-1);j++) {
//				for (k=0;k<=(n-1);k++) {
//					sasfit_out("alpha(%d)(%d) = %g\n",j,k,a[j][k]);
//				}
//			}
			*error = TRUE;
			free_ivector(ipiv,0,n-1);
			free_ivector(indxr,0,n-1);
			free_ivector(indxc,0,n-1);
			return;
		}
		pivinv=1.0/a[icol][icol];
		a[icol][icol]=1.0;
		for (l=0;l<=(n-1);l++) a[icol][l] *= pivinv;
		for (l=0;l<=(m-1);l++) b[icol][l] *= pivinv;
		for (ll=0;ll<=(n-1);ll++)
			if (ll != icol) {
				dum=a[ll][icol];
				a[ll][icol]=0.0;
				for (l=0;l<=(n-1);l++) a[ll][l]-=a[icol][l]*dum;
				for (l=0;l<=(m-1);l++) b[ll][l]-=b[icol][l]*dum;
			}
	}
	for (l=(n-1);l>=0;l--) {
		if (indxr[l] != indxc[l]) {
			for (k=0;k<=(n-1);k++) {
				if ((indxr[l] > (n-1)) || (indxr[l] < 0) ||(indxc[l] > (n-1)) || (indxc[l] < 0) ||(k > (n-1)) || (k < 0)) {
				   sasfit_err("GAUSSJ: indxr[l], indxrc[l] or k out of range, (l,n,m): %d ## %d ## %d ## %d ## %d ## %d\n",indxr[l],indxc[l],k,l,n,m);
			       *error = TRUE;
				   free_ivector(ipiv,0,n-1);
                   free_ivector(indxr,0,n-1);
	               free_ivector(indxc,0,n-1);				   
				   return;
				}
				SWAP(a[k][indxr[l]],a[k][indxc[l]]);
			}
		}
	}
	free_ivector(ipiv,0,n-1);
	free_ivector(indxr,0,n-1);
	free_ivector(indxc,0,n-1);
}

#undef SWAP
