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
 *   Ingo Bressler (ingo@cs.tu-berlin.de)
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
	int     robertus_p;
	int     iter_4_mc;
	int     fitorsim;
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
sasfit_common_DLLEXP int    sasfit_get_int_strategy(void);
sasfit_common_DLLEXP int    sasfit_get_int_strategy(void);
sasfit_common_DLLEXP int    sasfit_eps_get_robertus_p(void);
sasfit_common_DLLEXP int    sasfit_eps_get_iter_4_mc(void);
sasfit_common_DLLEXP int    sasfit_eps_get_fitorsim(void);

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
sasfit_common_DLLEXP void sasfit_set_int_strategy(int value);
sasfit_common_DLLEXP void sasfit_set_int_strategy(int value);
sasfit_common_DLLEXP void sasfit_eps_set_robertus_p(int value);
sasfit_common_DLLEXP void sasfit_eps_set_iter_4_mc(int value);
sasfit_common_DLLEXP void sasfit_eps_set_fitorsim(int value);
sasfit_common_DLLEXP void sasfit_eps_set_h(scalar value);
sasfit_common_DLLEXP void sasfit_eps_set_comp(scalar value);

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

