#ifndef HEADER_355CBCA42B085C03
#define HEADER_355CBCA42B085C03

/*
 * src/sasfit_common/include/sasfit_eps.h
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

#ifndef SASFIT_EPS_H
#define SASFIT_EPS_H

/**
 * \file sasfit_eps.h
 * \ingroup sasfit_eps
 */

/**
 * \defgroup sasfit_eps Numerical Constants
 * \ingroup sasfit_constants
 * Manages numerical constants like tolerance and differentiation step width.
 * Still contains some numerical values which originate in the
 * implementation of integration and fitting routines. Thus, some of them are
 * obsolete (or will become with future developments).
 */

/**
 * \ingroup sasfit_eps
 * Contains various numerical constants.
 */

typedef struct
{
	scalar  aniso;
	int     jmax_aniso;
	scalar  res;
	int     jmax_res;
	scalar  nriq;
	int     jmax_nriq;
	int     sq_or_iq;
    int     iq_or_gz;
    int     int_strategy;
    int     sphavg_strategy;
    int     hankel_strategy;
    int     sincos_quad_strategy;
	int     robertus_p;
	int     gausslegendre;
	int     chebyshev1;
	int     chebyshev2;
	int     gegenbauer;
	int     exponential;
	int     laguerre;
	int     jacobi;
	int     lebedev;
	int     spherical_t_design;
	int     fibonacci;
	int     iter_4_mc;
	int     fitorsim;
	int     rootalg;
	int     sg_level;
    scalar  N_Ogata;
    scalar  h_Ogata;
    scalar  MSASthickness;
    scalar  MSASlambda;
	scalar  alpha;
	scalar  beta;
	scalar  h;          //!< Percentage of variation for numerical differentiation.
	scalar  comparison; //!< Tolerance value for tests if two numbers are equal.
} sasfit_eps_t;

// hide direct functions from plugins
#ifndef MAKE_SASFIT_PLUGIN

/**
 * \ingroup sasfit_eps
 * Returns relative error used for \ref sasfit_integrate.
 */
sasfit_common_DLLEXP scalar sasfit_eps_get_aniso(void);

sasfit_common_DLLEXP int    sasfit_eps_get_jmax_aniso(void);
sasfit_common_DLLEXP scalar sasfit_eps_get_res(void);
sasfit_common_DLLEXP int    sasfit_eps_get_jmax_res(void);
sasfit_common_DLLEXP scalar sasfit_eps_get_nriq(void);
sasfit_common_DLLEXP int    sasfit_eps_get_jmax_nriq(void);
sasfit_common_DLLEXP int    sasfit_eps_get_sq_or_iq(void);
sasfit_common_DLLEXP int    sasfit_get_iq_or_gz(void);
sasfit_common_DLLEXP int    sasfit_get_sphavg_strategy(void);
sasfit_common_DLLEXP int    sasfit_get_int_strategy(void);
sasfit_common_DLLEXP int    sasfit_get_hankel_strategy(void);
sasfit_common_DLLEXP int    sasfit_get_sincos_quad_strategy(void);
sasfit_common_DLLEXP scalar sasfit_get_h_Ogata(void);
sasfit_common_DLLEXP scalar sasfit_get_N_Ogata(void);
sasfit_common_DLLEXP scalar sasfit_get_MSASthickness(void);
sasfit_common_DLLEXP scalar sasfit_get_MSASlambda(void);
sasfit_common_DLLEXP int    sasfit_eps_get_robertus_p(void);
sasfit_common_DLLEXP int    sasfit_eps_get_gausslegendre(void);
sasfit_common_DLLEXP int    sasfit_eps_get_chebyshev1(void);
sasfit_common_DLLEXP int    sasfit_eps_get_chebyshev2(void);
sasfit_common_DLLEXP int    sasfit_eps_get_gegenbauer(void);
sasfit_common_DLLEXP int    sasfit_eps_get_exponential(void);
sasfit_common_DLLEXP int    sasfit_eps_get_laguerre(void);
sasfit_common_DLLEXP int    sasfit_eps_get_jacobi(void);
sasfit_common_DLLEXP int    sasfit_eps_get_lebedev(void);
sasfit_common_DLLEXP int    sasfit_eps_get_spherical_t_design(void);
sasfit_common_DLLEXP int    sasfit_eps_get_fibonacci(void);
sasfit_common_DLLEXP int    sasfit_eps_get_iter_4_mc(void);
sasfit_common_DLLEXP int    sasfit_eps_get_fitorsim(void);
sasfit_common_DLLEXP int    sasfit_get_rootalg(void);
sasfit_common_DLLEXP scalar sasfit_eps_get_alpha(void);
sasfit_common_DLLEXP scalar sasfit_eps_get_beta(void);
sasfit_common_DLLEXP int sasfit_get_sg_level(void);

sasfit_common_DLLEXP scalar sasfit_eps_get_comp(void);
/**
 * \ingroup sasfit_eps
 * Returns step width for numerical differentiation.
 */
sasfit_common_DLLEXP scalar sasfit_eps_get_h(void);

/**
 * \ingroup sasfit_eps
 * Returns comparison tolerance for equality definition of floats.
 */

sasfit_common_DLLEXP void sasfit_eps_set_aniso(scalar value);
sasfit_common_DLLEXP void sasfit_eps_set_jmax_aniso(int value);
sasfit_common_DLLEXP void sasfit_eps_set_res(scalar value);
sasfit_common_DLLEXP void sasfit_eps_set_jmax_res(int value);
sasfit_common_DLLEXP void sasfit_eps_set_nriq(scalar value);
sasfit_common_DLLEXP void sasfit_eps_set_jmax_nriq(int value);
sasfit_common_DLLEXP void sasfit_eps_set_sq_or_iq(int value);
sasfit_common_DLLEXP void sasfit_set_iq_or_gz(int value);
sasfit_common_DLLEXP void sasfit_set_sphavg_strategy(int value);
sasfit_common_DLLEXP void sasfit_set_int_strategy(int value);
sasfit_common_DLLEXP void sasfit_set_hankel_strategy(int value);
sasfit_common_DLLEXP void sasfit_set_sincos_quad_strategy(int value);
sasfit_common_DLLEXP void sasfit_set_h_Ogata(scalar value);
sasfit_common_DLLEXP void sasfit_set_N_Ogata(scalar value);
sasfit_common_DLLEXP void sasfit_set_MSASthickness(scalar value);
sasfit_common_DLLEXP void sasfit_set_MSASlambda(scalar value);
sasfit_common_DLLEXP void sasfit_eps_set_robertus_p(int value);
sasfit_common_DLLEXP void sasfit_eps_set_gausslegendre(int value);
sasfit_common_DLLEXP void sasfit_eps_set_chebyshev1(int value);
sasfit_common_DLLEXP void sasfit_eps_set_chebyshev2(int value);
sasfit_common_DLLEXP void sasfit_eps_set_gegenbauer(int value);
sasfit_common_DLLEXP void sasfit_eps_set_exponential(int value);
sasfit_common_DLLEXP void sasfit_eps_set_laguerre(int value);
sasfit_common_DLLEXP void sasfit_eps_set_jacobi(int value);
sasfit_common_DLLEXP void sasfit_eps_set_lebedev(int value);
sasfit_common_DLLEXP void sasfit_eps_set_spherical_t_design(int value);
sasfit_common_DLLEXP void sasfit_eps_set_fibonacci(int value);
sasfit_common_DLLEXP void sasfit_eps_set_iter_4_mc(int value);
sasfit_common_DLLEXP void sasfit_eps_set_fitorsim(int value);
sasfit_common_DLLEXP void sasfit_set_rootalg(int value);
sasfit_common_DLLEXP void sasfit_eps_set_h(scalar value);
sasfit_common_DLLEXP void sasfit_eps_set_comp(scalar value);
sasfit_common_DLLEXP void sasfit_eps_set_alpha(scalar value);
sasfit_common_DLLEXP void sasfit_eps_set_beta(scalar value);
sasfit_common_DLLEXP void sasfit_set_sg_level(int value);

/**
 * \ingroup sasfit_eps
 * Prints the current values of a sasfit_eps_t structure to stderr.
 * <b>For internal use only.</b>
 * @param fitp The sasfit_eps_t structure to print.
 */
void sasfit_eps_print_struct(const sasfit_eps_t * fitp);

/**
 * \ingroup sasfit_eps
 * Prints the current values of a sasfit_eps_t structure to stderr.
 * Useful for debugging.
 */
sasfit_common_DLLEXP void sasfit_eps_print(void);


#endif // MAKE_SASFIT_PLUGIN
#endif // file

#endif // header guard 

