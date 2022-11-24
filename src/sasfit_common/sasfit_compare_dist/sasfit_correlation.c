/*
 * src/sasfit_common/sasfit_compare_dist/sasfit_correlation.c
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

double sasfit_sum(double *x, int dim) {
	double sum;
	int i;
	for (i=0;i<dim;i++) {
		sum=sum+x[i];
	}
	return sum;
}

double sasfit_mean(double *x, int dim) {
	return sasfit_sum(x,dim)/dim;
}

double pearson_corr_centered(double *x, double *y, int ndim, sasfit_param *param){
	double mean_x, mean_y, r;
	double x_diff, y_diff, x_diff_sq, y_diff_sq, sumxydiff, sumxdiff2, sumydiff2;
	int i;
    r = 0.0;
    mean_x = sasfit_mean(x,ndim);
    mean_y = sasfit_mean(y,ndim);
    sumxydiff = 0;
	sumxdiff2 = 0;
	sumydiff2 = 0;

	if (!gsl_finite(mean_x) || !gsl_finite(mean_y)) {
		sasfit_out("input vectors store values, which are not finite or not-a-number\n");
	}
	for (i=0;i<ndim;i++) {
		x_diff    = x[i] - mean_x;
		x_diff_sq = x_diff*x_diff;
		y_diff    = y[i] - mean_y;
		y_diff_sq = y_diff*y_diff;
		sumxydiff = sumxydiff +x_diff * y_diff;
		sumxdiff2 = sumxdiff2 +x_diff_sq;
		sumydiff2 = sumydiff2 +y_diff_sq;
	}
	r = sumxydiff / sqrt(sumxdiff2*sumydiff2);
// r = ( Rcpp::sum(x_diff * y_diff) ) / (sqrt(Rcpp::sum(x_diff_sq)) * sqrt(Rcpp::sum(y_diff_sq)));
	return r;
}


double pearson_corr_uncentered(double *x, double *y, int ndim, sasfit_param *param){
	double mean_x, mean_y, r_un;
	double x_diff, y_diff, x_diff_sq, y_diff_sq, sumxy, sumxdiff2, sumydiff2;
	int i;
    r_un = 0.0;
    mean_x = sasfit_mean(x,ndim);
    mean_y = sasfit_mean(y,ndim);
    sumxdiff2 = 0;
    sumydiff2 = 0;
	sumxy = 0;

	if (!gsl_finite(mean_x) || !gsl_finite(mean_y)) {
		sasfit_out("input vectors store values, which are not finite or not-a-number\n");
	}
	for (i=0;i<ndim;i++) {
		x_diff    = x[i] - mean_x;
		x_diff_sq = x_diff*x_diff;
		y_diff    = y[i] - mean_y;
		y_diff_sq = y_diff*y_diff;
		sumxy = sumxy +x[i]*y[i];
		sumxdiff2 = sumxdiff2 +x_diff_sq;
		sumydiff2 = sumydiff2 +y_diff_sq;
	}
	r_un = sumxy / sqrt(sumxdiff2*sumydiff2);
//  r_un = ( Rcpp::sum(Rcpp::NumericVector(x) * Rcpp::NumericVector(y)) ) / (sqrt(Rcpp::sum(x_diff_sq)) * sqrt(Rcpp::sum(y_diff_sq)));
	return r_un;
}


double squared_pearson_corr(double *x, double *y, int ndim, sasfit_param *param){
	double mean_x, mean_y, r_sq;
	double x_diff, y_diff, sumxydiff, sumx2, sumy2;
	int i;
    r_sq = 0.0;
    mean_x = sasfit_mean(x,ndim);
    mean_y = sasfit_mean(y,ndim);
    sumxydiff = 0;
	sumx2 = 0;
	sumy2 = 0;

	if (!gsl_finite(mean_x) || !gsl_finite(mean_y)) {
		sasfit_out("input vectors store values, which are not finite or not-a-number\n");
	}
	for (i=0;i<ndim;i++) {
		x_diff    = x[i] - mean_x;
		y_diff    = y[i] - mean_y;
		sumxydiff = sumxydiff +x_diff*x_diff;
		sumx2 = sumx2 +x[i]*x[i];
		sumy2 = sumy2 +y[i]*y[i];
	}

        r_sq = sumxydiff / (sqrt(sumx2) * sqrt(sumy2));
 // r_sq = ( Rcpp::sum(x_diff * y_diff) ) / (sqrt(Rcpp::sum(x * x)) * sqrt(Rcpp::sum(y * y)));
        return r_sq * r_sq;
}

