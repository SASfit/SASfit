/*
 * src/sasfit_ff/superparamagnetic_ff_psi/include/calc_alpha.h
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

#ifndef CALC_ALPHA_H
#define CALC_ALPHA_H

/*
 * functions for calculation of alpha
 */

#define SET_ALPHA_PLAIN \
\
	V[ALPHA] = V[MU_KT] * pow( V[R]/V[R_AV], 3.0)

#define SET_ALPHA_DEFAULT \
\
	SET_ALPHA_PLAIN; \
	V[ALPHA] = V[BT] * V[ALPHA]

#define SET_ALPHA_MOD \
\
	SET_ALPHA_PLAIN; \
	V[ALPHA] = fabs( V[BT] ) * V[ALPHA]

#define SET_ALPHA_ALBR_STAT \
\
	V[ALPHA] = V[MU_KT] * (V[B1] + V[B0])

#define SET_L_LA \
\
	if ( fabs(V[ALPHA]) < 1.0e-5 ) \
	{ \
		V[LA] = 1.0/3.0; \
		V[L] = 1.0/3.0*V[ALPHA]; \
/*	} else if (fabs(alpha)<=1.0e-3) */ \
/*	{ */ \
/*		V[L] = 1./3.* alpha; */ \
/*		V[LA] = 1.0/3.0; */ \
	} else \
	{ \
		V[L] = 1.0/tanh(V[ALPHA]) - 1.0/V[ALPHA]; \
		V[LA] = V[L] / V[ALPHA]; \
	}


#endif

