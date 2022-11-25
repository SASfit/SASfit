/*
 * src/sasfit_sd/sasfit_sd_gammaSD.c
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
 *   Ingo Bressler (ingo@cs.tu-berlin.de)
 */

#include <gsl/gsl_math.h>
#include "include/sasfit_sd_utils.h"

#define N			param->p[0]
#define MODE		param->p[1]
#define SIGMA		param->p[2]


scalar sasfit_sd_gammaSD(scalar x, sasfit_param * param)
{
	scalar k, theta, res;

	SASFIT_ASSERT_PTR( param );

// MODE = theta*(k-1)
// variance = sigma^2 = k*theta^2

	theta = 0.5*(sqrt(gsl_pow_2(MODE) + 4.0*gsl_pow_2(SIGMA))-MODE);
	k = (	gsl_pow_2(MODE) 
			+ 2*gsl_pow_2(SIGMA) 
			+ MODE*sqrt(gsl_pow_2(MODE) 
			+ 4.0*gsl_pow_2(SIGMA))
		)/(2.0*gsl_pow_2(SIGMA));
	
	if (x == 0.0) return 0.0;

	res = (k-1.)*log(x/theta) - x/theta - log(theta) - sasfit_gammaln(k);

	return N*exp(res);
}
