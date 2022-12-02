/*
 * src/sasfit_ff/sasfit_ff_sphere.c
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
 *   Ingo Bressler (ingo.bressler@bam.de)
 */

#include <gsl/gsl_math.h>
#include "include/sasfit_ff_utils.h"

#define R   param->p[0]
#define ETA param->p[3]

scalar sasfit_ff_sphere(scalar q, sasfit_param * param)
{
    scalar res;

    res = sasfit_ff_sphere_f(q, param);

    return gsl_pow_2(res);
}

scalar sasfit_ff_sphere_f(scalar q, sasfit_param * param)
{
    SASFIT_ASSERT_PTR(param);

    SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
    SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R);
    if (q*R<1e-4) return ETA*4.0/3.0*M_PI*R*R*R*(1 - gsl_pow_2(q*R)/10. + gsl_pow_4(q*R)/280. - gsl_pow_6(q*R)/15120.);
    return ETA*4.0*M_PI*(sin(q*R) - q*R*cos(q*R))/gsl_pow_3(q);
}

scalar sasfit_ff_sphere_v(scalar x, sasfit_param * param, int distr)
{
    SASFIT_ASSERT_PTR(param);

    if ( distr != 0 ) x = param->p[0];

    return 4.0/3.0 * M_PI * gsl_pow_3(x);
}
