/*
 * src/sasfit_common/sasfit_integrate.c
 *
 * Copyright (c) 2008-2022, Paul Scherrer Institute (PSI)
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
 

#include "../include/sasfit_common.h"
#include "../include/sasfit_function.h"
#include "../include/sasfit_divergence.h"



double sasfit_KullbackLeiblerDivergency(double *p, double *q, int dim){
	double DKL;
	int i;
	DKL=0;
	for (i=0;i<dim;i++) {
		if (fabs(p[i])!=0.0 && fabs(q[i])!=0.0) DKL=DKL+fabs(p[i])*log(fabs(p[i]/q[i]))-p[i]+q[i];
	}
	return DKL;
}

double sasfit_JensenShannonDivergency(double *p, double *q, int dim){
	return (sasfit_KullbackLeiblerDivergency(p,q,dim)+sasfit_KullbackLeiblerDivergency(q,p,dim))/2.;
}

double sasfit_RenyiDivergency(double *p, double *q, int dim, double alpha){
	double Dalpha;
	int i;
	Dalpha=0;
	if (alpha ==1) return sasfit_KullbackLeiblerDivergency(p,q,dim);
	for (i=0;i<dim;i++) {
		if (fabs(p[i])!=0.0 && fabs(q[i])!=0.0) 
				Dalpha=Dalpha+pow(fabs(p[i]),alpha)/pow(fabs(q[i]),alpha-1);
	}
	return log(Dalpha)/(alpha-1);
}

double sasfit_chi2Divergency(double *p, double *q, int dim){
	double Dchi2;
	int i;
	Dchi2=0;
	for (i=0;i<dim;i++) {
		if (q[i]!=0.0) Dchi2=Dchi2+gsl_pow_2(p[i]-q[i])/q[i];
	}
	return Dchi2;
}

double sasfit_HellingerDistance(double *p, double *q, int dim){
	double DH;
	int i;
	DH=0;
	for (i=0;i<dim;i++) {
		 DH=DH+gsl_pow_2(sqrt(fabs(p[i]))-sqrt(fabs(q[i])));
	}
	return sqrt(DH)/sqrt(2);
}

double sasfit_SquaredHellingerDistance(double *p, double *q, int dim){
	return gsl_pow_2(sasfit_HellingerDistance(p,q,dim));
}

double sasfit_Bhattacharyya_Distance(double *p, double *q, int dim){
	double DB;
	int i;
	DB=0;
	for (i=0;i<dim;i++) {
		 DB=DB+sqrt(fabs(p[i]*q[i]));
	}
	return -log(DB);
}

double sasfit_Itakura_Saito_Distance(double *p, double *q, int dim){
	double DIS;
	int i;
	DIS=0;
	for (i=0;i<dim;i++) {
		 if (fabs(p[i])!=0.0 && fabs(q[i])!=0.0) DIS=DIS+fabs(p[i]/q[i])+log(fabs(p[i]/q[i]))-1;
	}
	return DIS;
}

double sasfit_HellingerSquaredDistance(double *p, double *q, int dim){
	double DH2;
	int i;
	DH2=0;
	for (i=0;i<dim;i++) {
		 DH2=DH2+sqrt(fabs(p[i]*q[i]));
	}
	return 1-DH2;
}

int first_nonzero(scalar *vec, int dim) {
	int i;
	for (i=0;i<dim;i++) {
		if (vec[i] > 0.0) return i;
	}
	return -1;
}

scalar move_dirt(scalar *dirt,int di, scalar *holes, int hi) {
  // move as much dirt at [di] as possible to h[hi]
  double flow, dist;
  if (dirt[di] <= holes[hi]) { 		// use all dirt
    flow = dirt[di];
    dirt[di] = 0.0;            		// all dirt got moved
    holes[hi] = holes[hi]-flow;		// less to fill now
  } else if (dirt[di] > holes[hi]) { // use just part of dirt
    flow = holes[hi];          		// fill remainder of hole
    dirt[di] = dirt[di]-flow;   	// less dirt left
    holes[hi] = 0.0;       			// hole is filled
  }
  dist = abs(di - hi);
  return flow * dist;          		// work
}


scalar sasfit_WassersteinDistance(scalar *p, scalar *q, int dim){
	scalar tot_work, work, *holes, *dirt;
	int from_idx, to_idx, icount;
	dirt = malloc(dim*sizeof(scalar));
	holes = malloc(dim*sizeof(scalar));
	tot_work = 0.0;

  icount = 0;
  while (icount < 1000*dim) {  		//  TODO: add sanity counter check
    from_idx = first_nonzero(dirt,dim);
    to_idx 	 = first_nonzero(holes, dim);
    if (from_idx == -1 || to_idx == -1) break;
    work = move_dirt(dirt, from_idx, holes, to_idx);
    tot_work = tot_work+work;
	icount++;
  }
  if (icount == 1000*dim) sasfit_err("Too many steps in sasfit_wasserstein!\n");
  return tot_work;
}
