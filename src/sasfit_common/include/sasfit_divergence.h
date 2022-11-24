/*
 * src/sasfit_common/sasfit_dvergence.h
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

typedef enum
{
	Euclidean_L2_d, 				//1
	City_block_L1_d,				//2
	Minkowski_Lp_d,					//3
	Chebyshev_Linfty_d,				//4
	Sorensen_d,						//5
	Gower_d,						//6
	Soergel_d,						//7
	Kulczynski_d,					//8
	Cranberra_d,					//9
	Lorentzian_d,					//10
	Intersection_d,					//11a
	Intersection_s,					//11b
	Wave_Hedges_d,					//12
	Czekanowski_s,					//13a
	Czekanowski_d,					//13b
	Motyka_s,						//14a
	Motyka_d,						//14b
	Kulczynski_s,					//15
	Ruzicka_s,						//16
	Tanimoto_d,						//17
	InnerProduct_s,					//18
	HarmonicMean_s,					//19
	Cosine_s,						//20
	KumarHassebrook_s,				//21
	Jaccard_s,						//22a
	Jaccard_d,						//22b
	Dice_s,							//23a
	Dice_d,							//23b
	Fidelity_s,						//24
	Bhattacharyya_d,				//25
	Hellinger_d,					//26
	SquaredChord_d,					//28a
	SquaredChord_s,					//28b
	Matusita_d,						//27
	SquaredEuclidean_d,				//29
	PearsonChi2_d,					//30
	NeymanChi2_d,					//31
	SquaredChi2_d,					//32
	ProbabilisticSymmetricChi2_d,	//33
	Divergence_d,					//34
	Clark_d,						//35
	AdditiveSymmetricChi2_d,		//36
	KullbackLeibler_d,				//37
	Jeffreys_d,						//38
	Kdivergence_d,					//39
	Topsoe_d,						//40
	JensenShannon_d,				//41
	Jensen_d,						//42
	Taneja_d,						//43
	KumarJohnson_d,					//44
	AvgL1Linf_d,					//45
} sasfit_distance_metric_type;

typedef struct
{
	sasfit_distance_metric_type	metric_type;
	bool normalize2one;
} sasfit_distance_metric_struct;

double sasfit_Euclidian_L2_d				(double *, double *, int);	//1
double sasfit_CityBlock_L1_d				(double *, double *, int);	//2
double sasfit_Chebyshev_Linf_d				(double *, double *, int);	//3
double sasfit_Minkowski_Lp_d				(double *, double *, int, double);	//4
double sasfit_Sorensen_d					(double *, double *, int);	//5
double sasfit_Gower_d						(double *, double *, int);	//6
double sasfit_Soergel_d						(double *, double *, int);	//7
double sasfit_Kulczynski_d					(double *, double *, int);	//8
double sasfit_Canberra_d					(double *, double *, int);	//9
double sasfit_Lorentzian_d					(double *, double *, int);	//10
double sasfit_Intersection_d				(double *, double *, int);	//11a
double sasfit_Intersection_s				(double *, double *, int);	//11b
double sasfit_Wave_Hedges_d						(double *, double *, int);	//12
double sasfit_Czekanowski_s					(double *, double *, int);	//13a
double sasfit_Czekanowski_d					(double *, double *, int);	//13a
double sasfit_Motyka_s						(double *, double *, int);	//14a
double sasfit_Motyka_d						(double *, double *, int);	//14b
double sasfit_Kulczynski_s					(double *, double *, int);	//15
double sasfit_Ruzicka_s						(double *, double *, int);	//16
double sasfit_Tanimoto_d					(double *, double *, int);	//17
double sasfit_InnerProduct_s				(double *, double *, int);	//18
double sasfit_HarmonicMean_s				(double *, double *, int);	//19
double sasfit_Cosine_s						(double *, double *, int);	//20
double sasfit_KumarHassebrook_s				(double *, double *, int);	//21
double sasfit_Jaccard_s						(double *, double *, int);	//22a
double sasfit_Jaccard_d						(double *, double *, int);	//22b
double sasfit_Dice_s						(double *, double *, int);	//23a
double sasfit_Dice_d						(double *, double *, int);	//23b
double sasfit_Fidelity_s					(double *, double *, int);	//24
double sasfit_Bhattacharyya_d				(double *, double *, int);	//25
double sasfit_Hellinger_d					(double *, double *, int);	//26
double sasfit_SquaredChord_d				(double *, double *, int);	//28a
double sasfit_SquaredChord_s				(double *, double *, int);	//28b
double sasfit_Matusita_d					(double *, double *, int);	//27
double sasfit_SquaredEuclidean_d			(double *, double *, int);	//29
double sasfit_PearsonChi2_d					(double *, double *, int);	//30
double sasfit_NeymanChi2_d					(double *, double *, int);	//31
double sasfit_SquaredChi2_d					(double *, double *, int);	//32
double sasfit_ProbabilisticSymmetricChi2_d	(double *, double *, int);	//33
double sasfit_Divergence_d					(double *, double *, int);	//34
double sasfit_Clark_d						(double *, double *, int);	//35
double sasfit_AdditiveSymmetricChi2_d		(double *, double *, int);	//36
double sasfit_KullbackLeibler_d				(double *, double *, int);	//37
double sasfit_Jeffreys_d					(double *, double *, int);	//38
double sasfit_Kdivergence_d					(double *, double *, int);	//39
double sasfit_Topsoe_d						(double *, double *, int);	//40
double sasfit_JensenShannon_d				(double *, double *, int);	//41
double sasfit_Jensen_d						(double *, double *, int);	//42
double sasfit_Taneja_d						(double *, double *, int);	//43
double sasfit_KumarJohnson_d				(double *, double *, int);	//44
double sasfit_AvgL1Linf_d					(double *, double *, int);	//45

double sasfit_KullbackLeiblerDivergency(double *, double *, int);
double sasfit_JensenShannonDivergency  (double *, double *, int);
double sasfit_RenyiDivergency          (double *, double *, int, double);
double sasfit_chi2Divergency           (double *, double *, int);
double sasfit_HellingerDistance        (double *, double *, int);
double sasfit_SquaredHellingerDistance (double *, double *, int);
double sasfit_HellingerSquaredDistance (double *, double *, int);
double sasfit_Bhattacharyya_Distance   (double *, double *, int);
double sasfit_HistogrammIntersection_Distance   (double *, double *, int);
double sasfit_HistogrammCorrelation_Distance    (double *, double *, int);
double sasfit_Itakura_Saito_Distance   (double *, double *, int);
double sasfit_WassersteinDistance      (double *, double *, int);
double sasfit_Earth_Mover_Distance     (double *, double *, int);
double sasfit_CanberraDistance         (double *, double *, int);
double sasfit_Chi2Distance             (double *, double *, int);
double sasfit_CosineDistance           (double *, double *, int);


double sasfit_compare(double *, double*, int, sasfit_distance_metric_struct *, sasfit_param *);