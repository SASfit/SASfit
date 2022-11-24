/*
 * src/sasfit_common/sasfit_compare_dist/sasfit_compare.c
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

// referenc:
// Cha, S. H.
// Comprehensive Survey on Distance/Similarity Measures between Probability Density Functions
// 2007
// International Journal of Mathematical Models and Methods in Applied Sciences , Vol. 1, No. 4

double sasfit_compare(double *p, double*q, int dim, sasfit_distance_metric_struct *compare, sasfit_param *param) {
	return -1;
};

double sasfit_Euclidian_L2_d(double *p, double *q, int dim){
// 1. Euclidean L2
	double DL2,pNorm, qNorm;
	int i;
	DL2=0;
	for (i=0;i<dim;i++) {
		DL2=DL2+gsl_pow_2(p[i]-q[i]);
	}
	return DL2;
}

double sasfit_CityBlock_L1_d(double *p, double *q, int dim){
// 2. City block L1
	double DL1;
	int i;
	DL1=0;
	for (i=0;i<dim;i++) {
		DL1=DL1+fabs(p[i]-q[i]);
	}
	return DL1;
}

double sasfit_Minkowski_Lp_d(double *p, double *q, int dim, double k){
// 3. Minkowski Lp
	double DLp;
	int i;
	DLp=0;
	for (i=0;i<dim;i++) {
		DLp=DLp+pow(fabs(p[i]-q[i]),k);
	}
	return pow(DLp,1./k);
}

double sasfit_Chebyshev_Linf_d(double *p, double *q, int dim){
// 4. Chebyshev L∞
	double DLinf;
	int i;
	DLinf=0;
	for (i=0;i<dim;i++) {
		if (DLinf<fabs(p[i]-q[i])) DLinf=fabs(p[i]-q[i]);
	}
	return DLinf;
}

double sasfit_Sorensen_d(double *p, double *q, int dim){
// 5. Sørensen
	double DS,tmp;
	int i;
	DS=0;
	tmp=0;
	for (i=0;i<dim;i++) {
		DS=DS+fabs(p[i]-q[i]);
		tmp=tmp+(p[i]+q[i]);
	}
// %%%%%%%%%%% IF CONDITION NEEDS to checked %%%%%%%%%%%%%
	if (tmp==0) return 0;
	return DS/tmp;
}

double sasfit_Gower_d(double *p, double *q, int dim){
// 6. Gower
	double DG;
	int i;
	DG=0;
	for (i=0;i<dim;i++) {
		DG=DG+fabs(p[i]-q[i]);
	}
	return DG/dim;
}

double sasfit_Soergel_d(double *p, double *q, int dim){
// 7. Soergel
	double DS,tmp;
	int i;
	DS=0;
	for (i=0;i<dim;i++) {
		DS=DS+fabs(p[i]-q[i]);
		tmp=tmp+GSL_MAX(p[i],q[i]);
	}
	return DS/tmp;
}

double sasfit_Kulczynski_d(double *p, double *q, int dim){
// 8. Kulczynski d
	double DK,tmp;
	int i;
	DK=0;
	for (i=0;i<dim;i++) {
		DK=DK+fabs(p[i]-q[i]);
		tmp=tmp+GSL_MIN(p[i],q[i]);
	}
// %%%%%%%%%%% IF CONDITION NEEDS to checked %%%%%%%%%%%%%
	if (tmp==0) return 0;
	return DK/tmp;
}

double sasfit_Canberra_d(double *p, double *q, int dim){
// 9. Canberra
// https://en.wikipedia.org/wiki/Canberra_distance
    int i;
    double DCD,sum;
    DCD=0;
    for (i=0;i<dim;i++) {
        sum = p[i]+q[i];
// %%%%%%%%%%% IF CONDITION NEEDS to checked %%%%%%%%%%%%%
        if (sum!=0) DCD = DCD+fabs(p[i]-q[i])/sum;
    }
    return DCD;
}

double sasfit_Lorentzian_d(double *p, double *q, int dim){
// 10. Lorentzian
	double DL;
	int i;
	DL=0;
	for (i=0;i<dim;i++) {
		DL=DL+log(1+fabs(p[i]-q[i]));
	}
	return DL;
}

double sasfit_Intersection_d(double *p, double *q, int dim){
// 11a. Intersection d
	double DI;
	int i;
	DI=0;
	for (i=0;i<dim;i++) {
		DI=DI+fabs(p[i]-q[i]);
	}
	return DI/2.;
}

double sasfit_Intersection_s(double *p, double *q, int dim){
// 11b. Intersection s
	double DI;
	int i;
	DI=0;
	for (i=0;i<dim;i++) {
        DI=DI+GSL_MIN(fabs(p[i]),fabs(q[i]));
	}
	return DI;
}

double sasfit_Wave_Hedges_d(double *p, double *q, int dim){
// 12. Wave Hedges
	double DWH;
	int i;
	DWH=0;
	for (i=0;i<dim;i++) {
        if (GSL_MAX(fabs(p[i]),fabs(q[i]))!=0.0) {
                DWH=DWH+(fabs(p[i]-q[i]))/GSL_MAX(fabs(p[i]),fabs(q[i]));
        }
	}
	return DWH;
}

double sasfit_Czekanowski_s(double *p, double *q, int dim){
// 13a. Czekanowski
	double DC,tmp;
	int i;
	DC=0;
	tmp=0;
	for (i=0;i<dim;i++) {
        if (GSL_MIN(p[i],q[i]) > 0) {
            tmp=tmp+GSL_MIN(fabs(p[i]),fabs(q[i]));
        } else if (fabs(p[i])<fabs(q[i])){
            tmp=tmp+p[i];
        } else {
            tmp=tmp+p[i];
        }
		DC=DC+(p[i]+q[i]);
	}
// %%%%%%%%%%% IF CONDITION NEEDS to be checked %%%%%%%%%%%%%
	if (DC!=0) return 2*tmp/DC;
	return GSL_NAN;
}

double sasfit_Czekanowski_d(double *p, double *q, int dim){
// 13b. Czekanowski
	double DC,tmp;
	int i;
	DC=0;
	for (i=0;i<dim;i++) {
		DC=DC+fabs(p[i]-q[i]);
		tmp=tmp+p[i]+q[i];
	}
// %%%%%%%%%%% IF CONDITION NEEDS to be checked %%%%%%%%%%%%%
	if (tmp==0) return 0;
	return DC/tmp;
}

double sasfit_Motyka_s(double *p, double *q, int dim){
// 14a. Motyka
	double SM,tmp;
	int i;
	SM=0;
	for (i=0;i<dim;i++) {
		SM=SM+(p[i]+q[i]);
        if (GSL_MIN(p[i],q[i]) > 0) {
            tmp=tmp+GSL_MIN(fabs(p[i]),fabs(q[i]));
        } else if (fabs(p[i])<fabs(q[i])){
            tmp=tmp+p[i];
        } else {
            tmp=tmp+p[i];
        }
	}
// %%%%%%%%%%% IF CONDITION NEEDS to checked %%%%%%%%%%%%%
	if (SM==0) return 0;
	return tmp/SM;
}

double sasfit_Motyka_d(double *p, double *q, int dim){
// 14b. Motyka
	return 1-sasfit_Motyka_s(p,q,dim);
}

double sasfit_Kulczynski_s(double *p, double *q, int dim){
// 15. Kulczynski s
	return 1/sasfit_Kulczynski_d(p,q,dim);
}

double sasfit_Ruzicka_s(double *p, double *q, int dim){
// 16. Ruzicka
    double min_pq, max_pq;
    int i;
    min_pq=0;
    max_pq=0;
	for (i=0;i<dim;i++) {
		min_pq=min_pq+GSL_MIN(p[i],q[i]);
		max_pq=max_pq+GSL_MAX(p[i],q[i]);
	}
	return min_pq/max_pq;
}

double sasfit_Tanimoto_d(double *p, double *q, int dim){
// 17. Tanimoto
    double sum_p, sum_q, min_pq;
    int i;
    min_pq=0;
    sum_p=0;
    sum_q=0;
	for (i=0;i<dim;i++) {
		min_pq=min_pq+GSL_MIN(p[i],q[i]);
		sum_p=sum_p+p[i];
		sum_q=sum_q+q[i];
	}
	return (sum_p+sum_q-2*min_pq)/(sum_p+sum_q-min_pq);
}

double sasfit_InnerProduct_s(double *p, double *q, int dim){
// 18. Inner Product
    double IP;
    int i;
    IP=0;
	for (i=0;i<dim;i++) {
		IP=IP+(p[i]*q[i]);
	}
	return IP;
}

double sasfit_HarmonicMean_s(double *p, double *q, int dim){
// 19. Harmonic mean
    double HM;
    int i;
    HM=0;
	for (i=0;i<dim;i++) {
		HM=HM+(p[i]*q[i])/(p[i]+q[i]);
	}
	return HM;
}

double sasfit_Cosine_s(double *p, double *q, int dim){
// 20. Cosine
    double CS,sum_p,sum_q;
    int i;
    CS=0;
    sum_p=0;
    sum_q=0;
	for (i=0;i<dim;i++) {
		CS=CS+(p[i]*q[i]);
		sum_p=sum_p+p[i]*p[i];
		sum_q=sum_q+q[i]*q[i];
	}
	return CS/sqrt(sum_p*sum_q);
}

double sasfit_KumarHassebrook_s(double *p, double *q, int dim){
// 21. Kumar-Hassebrook (PCE)
    double CS,sum_p,sum_q;
    int i;
    CS=0;
    sum_p=0;
    sum_q=0;
	for (i=0;i<dim;i++) {
		CS=CS+(p[i]*q[i]);
		sum_p=sum_p+p[i]*p[i];
		sum_q=sum_q+q[i]*q[i];
	}
	return CS/(sum_p+sum_q-CS);
}

double sasfit_Jaccard_s(double *p, double *q, int dim){
// 22a. Jaccard
    return sasfit_KumarHassebrook_s(p,q,dim);
}

double sasfit_Jaccard_d(double *p, double *q, int dim){
// 22b. Jaccard
    double CS,sum_p,sum_q, sumpq;
    int i;
    CS=0;
    sum_p=0;
    sum_q=0;
	for (i=0;i<dim;i++) {
		CS=CS+gsl_pow_2(p[i]-q[i]);
		sum_p=sum_p+p[i]*p[i];
		sum_q=sum_q+q[i]*q[i];
		sumpq=sumpq+(p[i]*q[i]);
	}
	return CS/(sum_p+sum_q-sumpq);
}

double sasfit_Dice_s(double *p, double *q, int dim){
// 23a. Dice
    double CS,sum_p,sum_q;
    int i;
    CS=0;
    sum_p=0;
    sum_q=0;
	for (i=0;i<dim;i++) {
		CS=CS+(p[i]*q[i]);
		sum_p=sum_p+p[i]*p[i];
		sum_q=sum_q+q[i]*q[i];
	}
	return 2*CS/(sum_p+sum_q);
}

double sasfit_Dice_d(double *p, double *q, int dim){
// 23b. Dice
    double CS,sum_p,sum_q;
    int i;
    CS=0;
    sum_p=0;
    sum_q=0;
	for (i=0;i<dim;i++) {
		CS=CS+gsl_pow_2(p[i]-q[i]);
		sum_p=sum_p+p[i]*p[i];
		sum_q=sum_q+q[i]*q[i];
	}
	return CS/(sum_p+sum_q);
}

double sasfit_Fidelity_s(double *p, double *q, int dim){
// 24. Fidelity
	double FS;
	int i;
	FS=0;
	for (i=0;i<dim;i++) {
		 FS=FS+sqrt(fabs(p[i]*q[i]));
	}
	return FS;
}

double sasfit_Bhattacharyya_d(double *p, double *q, int dim){
// 25. Bhattacharyya
	return -log(sasfit_Fidelity_s(p,q,dim));
}

double sasfit_Hellinger_d(double *p, double *q, int dim){
// 26. Hellinger
	double DH;
	int i;
	DH=0;
	for (i=0;i<dim;i++) {
		 DH=DH+gsl_pow_2(sqrt(fabs(p[i]))-sqrt(fabs(q[i])));
	}
	return sqrt(DH)*sqrt(2.0);
	return 2*sqrt(1-sasfit_Fidelity_s(p,q,dim));
}

double sasfit_SquaredChord_d(double *p, double *q, int dim){
// 28a. Squared-chord
	double DSQ;
	int i;
	DSQ=0;
	for (i=0;i<dim;i++) {
		 DSQ=DSQ+sqrt(fabs(p[i]))-sqrt(fabs(q[i]));
	}
	return DSQ;
}

double sasfit_SquaredChord_s(double *p, double *q, int dim){
// 28b. Squared-chord
	double DSQ;
	int i;
	DSQ=0;
	for (i=0;i<dim;i++) {
		 DSQ=DSQ+2*sqrt(fabs(p[i]*q[i]));
	}
	return DSQ-1;
}

double sasfit_Matusita_d(double *p, double *q, int dim){
// 27. Matusita
    return sqrt(sasfit_SquaredChord_d(p,q,dim));
}

double sasfit_SquaredEuclidean_d(double *p, double *q, int dim){
// 29. Squared Euclidean
	double SQE;
	int i;
	SQE=0;
	for (i=0;i<dim;i++) {
		 SQE=SQE+gsl_pow_2(p[i]-q[i]);
	}
	return SQE;
}

double sasfit_PearsonChi2_d(double *p, double *q, int dim){
// 30. Pearson χ2
	double SQE;
	int i;
	SQE=0;
	for (i=0;i<dim;i++) {
		 SQE=SQE+gsl_pow_2(p[i]-q[i])/q[i];
	}
	return SQE;
}

double sasfit_NeymanChi2_d(double *p, double *q, int dim){
// 31. Neyman χ2
	double SQE;
	int i;
	SQE=0;
	for (i=0;i<dim;i++) {
		 SQE=SQE+gsl_pow_2(p[i]-q[i])/p[i];
	}
	return SQE;
}

double sasfit_SquaredChi2_d(double *p, double *q, int dim){
// 32. Squared χ2
	double SQE;
	int i;
	SQE=0;
	for (i=0;i<dim;i++) {
		 SQE=SQE+gsl_pow_2(p[i]-q[i])/(p[i]+q[i]);
	}
	return SQE;
}

double sasfit_ProbabilisticSymmetricChi2_d(double *p, double *q, int dim){
// 33. Probabilistic Symmetric χ2
    return 2*sasfit_SquaredChi2_d(p,q,dim);
}

double sasfit_Divergence_d(double *p, double *q, int dim){
// 34. Divergence
	double SQD;
	int i;
	SQD=0;
	for (i=0;i<dim;i++) {
		 SQD=SQD+gsl_pow_2((p[i]-q[i])/(p[i]+q[i]));
	}
	return 2*SQD;
}

double sasfit_Clark_d(double *p, double *q, int dim){
// 35. Clark
	double CD;
	int i;
	CD=0;
	for (i=0;i<dim;i++) {
		 CD=CD+gsl_pow_2(fabs(p[i]-q[i])/(p[i]+q[i]));
	}
	return sqrt(CD);
}

double sasfit_AdditiveSymmetricChi2_d(double *p, double *q, int dim){
// 36. Additive Symmetric χ2
	double ACD;
	int i;
	ACD=0;
	for (i=0;i<dim;i++) {
		 ACD=ACD+gsl_pow_2(p[i]-q[i])*(p[i]+q[i])/(p[i]*q[i]);
	}
	return ACD;
}

double sasfit_KullbackLeibler_d(double *p, double *q, int dim){
// 37. Kullback–Leibler
	double DKL;
	int i;
	DKL=0;
	for (i=0;i<dim;i++) {
		 DKL=DKL+p[i]*log(p[i]/q[i]);
	}
	return DKL;
}

double sasfit_Jeffreys_d(double *p, double *q, int dim){
// 38. Jeffreys
	double DJ;
	int i;
	DJ=0;
	for (i=0;i<dim;i++) {
		 DJ=DJ+(p[i]-q[i])*log(p[i]/q[i]);
	}
	return DJ;
}

double sasfit_Kdivergence_d(double *p, double *q, int dim){
// 39. K divergence
	double DK;
	int i;
	DK=0;
	for (i=0;i<dim;i++) {
		 DK=DK+p[i]*log(2*p[i]/(p[i]+q[i]));
	}
	return DK;
}

double sasfit_Topsoe_d(double *p, double *q, int dim){
// 40. Topsøe
	double DT;
	int i;
	DT=0;
	for (i=0;i<dim;i++) {
		 DT=DT+p[i]*log(2*p[i]/(p[i]+q[i]))+q[i]*log(2*q[i]/(p[i]+q[i]));
	}
	return DT;
}

double sasfit_JensenShannon(double *p, double *q, int dim){
// 41. Jensen-Shannon
    return 0.5*sasfit_Topsoe_d(p,q,dim);
}

double sasfit_Jensen_d(double *p, double *q, int dim){
// 42. Jensen difference
	double DJ;
	int i;
	DJ=0;
	for (i=0;i<dim;i++) {
		 DJ=DJ+(p[i]*log(p[i])+q[i]*log(q[i]))/2.-(p[i]+q[i])/2.*log((p[i]+q[i])/2.);
	}
	return DJ;
}

double sasfit_Taneja_d(double *p, double *q, int dim){
// 43. Taneja
	double DT;
	int i;
	DT=0;
	for (i=0;i<dim;i++) {
		 DT=DT+(p[i]+q[i])/2.*log((p[i]+q[i])/(2.*sqrt(p[i]*q[i])));
	}
	return DT;
}

double sasfit_KumarJohnson_d(double *p, double *q, int dim){
// 44. Kumar-Johnson
	double DK;
	int i;
	DK=0;
	for (i=0;i<dim;i++) {
		 DK=DK+gsl_pow_2(p[i]*p[i]-q[i]*q[i])/(2.*sqrt(fabs(gsl_pow_3(p[i]*q[i]))));
	}
	return DK;
}

double sasfit_AvgL1Linf_d(double *p, double *q, int dim){
// 45. Avg(L1 ,L∞)
    return (sasfit_CityBlock_L1_d(p,q,dim)+sasfit_Chebyshev_Linf_d(p,q,dim))/2.;
}





















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
	double DJS;
	int i;
	DJS=0;
	for (i=0;i<dim;i++) {
		if (fabs(p[i])!=0.0 && fabs(q[i])!=0.0 && fabs(p[i]+q[i])!=0.0)
            DJS=DJS+fabs(p[i])*log(fabs(2*p[i]/(p[i]+q[i])))+fabs(q[i])*log(fabs(2*q[i]/(p[i]+q[i])));
	}
	return 0.5*DJS;
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
	return sqrt(DH)*sqrt(2.0);
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

double sasfit_HistogrammIntersection_Distance(double *p, double *q, int dim){
	double DHI,tmp;
	int i;
	DHI=0;
	for (i=0;i<dim;i++) {
		 DHI=DHI+GSL_MIN(p[i],q[i]);
		 tmp=tmp+q[i];
	}
	return 1-DHI/tmp;
}

double sasfit_HistogrammCorrelation_Distance(double *p, double *q, int dim){
	double DHC,pavg,qavg,pvar,qvar,ptmp,qtmp;
	int i;
	DHC=0;
	pavg=0;
	qavg=0;
	pvar=0;
	qvar=0;
	for (i=0;i<dim;i++) {
		 pavg=pavg+p[i];
		 qavg=qavg+q[i];
	}
	pavg=pavg/dim;
	qavg=qavg/dim;
	for (i=0;i<dim;i++) {
		ptmp=(p[i]-pavg);
		qtmp=(q[i]-qavg);
		DHC=DHC+ptmp*qtmp;
		pvar=pvar+ptmp*ptmp;
		qavg=qavg+qtmp*qtmp;
	}
	return 1-DHC/sqrt(pvar*qvar);
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

int first_nonzero(double *vec, int dim) {
	int i;
	for (i=0;i<dim;i++) {
		if (vec[i] > 0.0) return i;
	}
	return -1;
}

double move_dirt(double *dirt,int di, double *holes, int hi) {
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

double sasfit_WassersteinDistance(double *p, double *q, int dim){
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
  free(dirt);
  free(holes);
  return tot_work;
}

double sasfit_Earth_Mover_Distance(double *p, double *q, int dim) {
    return 0;
}

double sasfit_Chi2Distance(double *p, double *q, int dim){
// https://en.wikipedia.org/wiki/Canberra_distance
    int i;
    double DChi2D,sum;
    DChi2D=0;
    for (i=0;i<dim;i++) {
        sum = fabs(p[i])+fabs(q[i]);
        if (sum!=0) DChi2D = DChi2D+gsl_pow_2(p[i]-q[i])/sum;
    }
    return DChi2D/2.;
}

double sasfit_CosineDistance(double *p, double *q, int dim){
    // https://en.wikipedia.org/wiki/Cosine_similarity
    int i;
    double DCosD,normp, normq;
    DCosD=0;
    normp=0;
    normq=0;
    for (i=0;i<dim;i++) {
        DCosD = DCosD+p[i]*q[i];
        normp = normp+p[i]*p[i];
        normq = normq+q[i]*q[i];

    }
    if (normp*normq!=0) return 1-DCosD/sqrt(normp*normq);
    return 1;
}
