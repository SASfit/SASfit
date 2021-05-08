/*
 * src/sasfit_common/sasfit_eps.c
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

#include "include/sasfit_common.h"

#define FitPrecis sasfit_eps
static sasfit_eps_t     sasfit_eps;

sasfit_common_DLLEXP
scalar sasfit_eps_get_aniso(void) {
	return sasfit_eps.aniso;
}
sasfit_common_DLLEXP
int sasfit_eps_get_jmax_aniso(void) {
	return sasfit_eps.jmax_aniso;
}
sasfit_common_DLLEXP
scalar sasfit_eps_get_res(void) {
	return sasfit_eps.res;
}
sasfit_common_DLLEXP
int sasfit_eps_get_jmax_res(void) {
	return sasfit_eps.jmax_res;
}
sasfit_common_DLLEXP
scalar sasfit_eps_get_nriq(void) {
	return sasfit_eps.nriq;
}
sasfit_common_DLLEXP
int sasfit_eps_get_jmax_nriq(void) {
	return sasfit_eps.jmax_nriq;
}
sasfit_common_DLLEXP
int sasfit_eps_get_sq_or_iq(void) {
	return sasfit_eps.sq_or_iq;
}
sasfit_common_DLLEXP
int sasfit_eps_get_robertus_p(void) {
	return sasfit_eps.robertus_p;
}
sasfit_common_DLLEXP
int sasfit_eps_get_gausslegendre(void) {
	return sasfit_eps.gausslegendre;
}
sasfit_common_DLLEXP
int sasfit_eps_get_chebyshev1(void) {
	return sasfit_eps.chebyshev1;
}
sasfit_common_DLLEXP
int sasfit_eps_get_chebyshev2(void) {
	return sasfit_eps.chebyshev2;
}
sasfit_common_DLLEXP
int sasfit_eps_get_gegenbauer(void) {
	return sasfit_eps.gegenbauer;
}
sasfit_common_DLLEXP
int sasfit_eps_get_exponential(void) {
	return sasfit_eps.exponential;
}
sasfit_common_DLLEXP
int sasfit_eps_get_laguerre(void) {
	return sasfit_eps.laguerre;
}
sasfit_common_DLLEXP
int sasfit_eps_get_jacobi(void) {
	return sasfit_eps.jacobi;
}
sasfit_common_DLLEXP
int sasfit_eps_get_lebedev(void) {
	return sasfit_eps.lebedev;
}
sasfit_common_DLLEXP
int sasfit_eps_get_fibonacci(void) {
	return sasfit_eps.fibonacci;
}
sasfit_common_DLLEXP
int sasfit_eps_get_iter_4_mc(void) {
	return sasfit_eps.iter_4_mc;
}
sasfit_common_DLLEXP
int sasfit_eps_get_fitorsim(void) {
	return sasfit_eps.fitorsim;
}
sasfit_common_DLLEXP
scalar sasfit_eps_get_h(void) {
	return sasfit_eps.h;
}
sasfit_common_DLLEXP
scalar sasfit_eps_get_comp(void) {
	return sasfit_eps.comparison;
}
sasfit_common_DLLEXP
void sasfit_set_iq_or_gz(int value) {
	sasfit_eps.iq_or_gz = value;
}
sasfit_common_DLLEXP
int sasfit_get_iq_or_gz(void) {
	return sasfit_eps.iq_or_gz;
}
sasfit_common_DLLEXP
scalar sasfit_eps_get_alpha(void) {
	return sasfit_eps.alpha;
}
sasfit_common_DLLEXP
scalar sasfit_eps_get_beta(void) {
	return sasfit_eps.beta;
}
sasfit_common_DLLEXP
void sasfit_set_int_strategy(int value) {
	sasfit_eps.int_strategy = value;
}
sasfit_common_DLLEXP
int sasfit_get_int_strategy(void) {
	return sasfit_eps.int_strategy;
}

sasfit_common_DLLEXP
void sasfit_set_sphavg_strategy(int value) {
	sasfit_eps.sphavg_strategy = value;
}
sasfit_common_DLLEXP
int sasfit_get_sphavg_strategy(void) {
	return sasfit_eps.sphavg_strategy;
}

sasfit_common_DLLEXP
void sasfit_eps_set_aniso(scalar value) {
	sasfit_eps.aniso = value;
}
sasfit_common_DLLEXP
void sasfit_eps_set_jmax_aniso(int value) {
	sasfit_eps.jmax_aniso = value;
}
sasfit_common_DLLEXP
void sasfit_eps_set_res(scalar value) {
	sasfit_eps.res = value;
}
sasfit_common_DLLEXP
void sasfit_eps_set_jmax_res(int value) {
	sasfit_eps.jmax_res = value;
}
sasfit_common_DLLEXP
void sasfit_eps_set_nriq(scalar value) {
	sasfit_eps.nriq = value;
}
sasfit_common_DLLEXP
void sasfit_eps_set_jmax_nriq(int value) {
	sasfit_eps.jmax_nriq = value;
}
sasfit_common_DLLEXP
void sasfit_eps_set_sq_or_iq(int value) {
	sasfit_eps.sq_or_iq = value;
}
sasfit_common_DLLEXP
void sasfit_eps_set_robertus_p(int value) {
	sasfit_eps.robertus_p = value;
}
sasfit_common_DLLEXP
void sasfit_eps_set_gausslegendre(int value) {
	sasfit_eps.gausslegendre = value;
}
sasfit_common_DLLEXP
void sasfit_eps_set_chebyshev1(int value) {
	sasfit_eps.chebyshev1 = value;
}
sasfit_common_DLLEXP
void sasfit_eps_set_chebyshev2(int value) {
	sasfit_eps.chebyshev2 = value;
}
sasfit_common_DLLEXP
void sasfit_eps_set_gegenbauer(int value) {
	sasfit_eps.gegenbauer = value;
}
sasfit_common_DLLEXP
void sasfit_eps_set_exponential(int value) {
	sasfit_eps.exponential = value;
}
sasfit_common_DLLEXP
void sasfit_eps_set_laguerre(int value) {
	sasfit_eps.laguerre = value;
}
sasfit_common_DLLEXP
void sasfit_eps_set_jacobi(int value) {
	sasfit_eps.jacobi = value;
}
sasfit_common_DLLEXP
void sasfit_eps_set_lebedev(int value) {
	sasfit_eps.lebedev = value;
}
sasfit_common_DLLEXP
void sasfit_eps_set_fibonacci(int value) {
	sasfit_eps.fibonacci = value;
}
sasfit_common_DLLEXP
void sasfit_eps_set_iter_4_mc(int value) {
	sasfit_eps.iter_4_mc = value;
}
sasfit_common_DLLEXP
void sasfit_eps_set_fitorsim(int value) {
	sasfit_eps.fitorsim = value;
}
sasfit_common_DLLEXP
void sasfit_eps_set_h(scalar value) {
	sasfit_eps.h = value;
}
sasfit_common_DLLEXP
void sasfit_eps_set_alpha(scalar value) {
	sasfit_eps.alpha = value;
}
sasfit_common_DLLEXP
void sasfit_eps_set_beta(scalar value) {
	sasfit_eps.beta = value;
}
sasfit_common_DLLEXP
void sasfit_eps_set_comp(scalar value) {
	sasfit_eps.comparison = value;
}




void sasfit_eps_print_struct(const sasfit_eps_t * fitp)
{
	SASFIT_ASSERT_VOID(fitp);
	sasfit_out("epsanisotropic:  %f\n", fitp->aniso);
	sasfit_out("JMAXanisotropic: %d\n", fitp->jmax_aniso);
	sasfit_out("epsresolution:   %f\n", fitp->res);
	sasfit_out("JMAXresolution:  %d\n", fitp->jmax_res);
	sasfit_out("epsNRIQ:         %f\n", fitp->nriq);
	sasfit_out("JMAXNRIQ:        %d\n", fitp->jmax_nriq);
	sasfit_out("SQ_or_IQ:        %d\n", fitp->sq_or_iq);
    sasfit_out("IQorGz_int       %d\n", fitp->iq_or_gz);
    sasfit_out("IntStrategy_int  %d\n", fitp->int_strategy);
    sasfit_out("SphAvgStrategy_int  %d\n", fitp->sphavg_strategy);
	sasfit_out("Robertus_p:      %d\n", fitp->robertus_p);
	sasfit_out("GSL_GAUSSLEGENDRE points: %d\n", fitp->gausslegendre);
	sasfit_out("GSL_CHEBYSHEV1 points:    %d\n", fitp->chebyshev1);
	sasfit_out("GSL_CHEBYSHEV2 points:    %d\n", fitp->chebyshev2);
	sasfit_out("GSL_GEGENBAUER points:    %d\n", fitp->gegenbauer);
	sasfit_out("GSL_EXPONENTIAL points:   %d\n", fitp->exponential);
	sasfit_out("GSL_LAGUERRE points:      %d\n", fitp->laguerre);
	sasfit_out("GSL_JACOBI points:        %d\n", fitp->jacobi);
	sasfit_out("alpha:               %f\n", fitp->alpha);
	sasfit_out("beta :               %f\n", fitp->beta);
	sasfit_out("Lebedev order [1,64]:     %d\n", fitp->lebedev);
	sasfit_out("FIBONACCI points:         %d\n", fitp->fibonacci);
	sasfit_out("iter_4_MC:       %d\n", fitp->iter_4_mc);
	sasfit_out("fitorsim:        %d\n", fitp->fitorsim);
	sasfit_out("h:               %f\n", fitp->h);
	sasfit_out("eps_comparison:  %f\n", fitp->comparison);
}

void sasfit_eps_print()
{
	sasfit_eps_print_struct(&sasfit_eps);
}

