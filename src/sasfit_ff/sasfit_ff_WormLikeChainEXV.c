/*
 * src/sasfit_ff/sasfit_ff_WormLikeChainEXV.c
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
#include "include/sasfit_ff_utils.h"

scalar s_exv_app__(scalar * q);
scalar s_kp_exv__(scalar *q, scalar *rl, scalar *rll, int *i_exvol);

/*
float WormLikeChainEXV(Tcl_Interp *interp,
		float Q,
		  float S0,
		  float RL,
		float RRL,
		float R,
		  bool  *error)
*/
/*		MONODISPERSE WORM-LIKE CHAINS */
/* */
/*		Spherical Cross-section FOR CHAINS  assumed */
/* */
/*		A(1) = S(0) */
/*		A(2) = CONTOUR LENGTH OF CHAINS */
/*		A(3) = KUHN LENGTH */
/*		A(4) = XS R */


/* ******************************************************************* */
/*		kp chains with excl volume. R/b=0.1 */
/*		S(q) from MC simulations parametreised by the Yoshizaki and */
/*		Yamakawa approach */
/*		RL= CONTOUR LENGTH */
/*		RRL = KUHN LENGTH */
/*				  * THAT IS RLL  IS THE STATISTICLA SEGMENT LENGTH */
/*		I_EXVOL = 0 : GAUSSIAN COILS */
/*					 1 : EXCLUDED VOLUME EFFECTS INCLUDED */

scalar sasfit_ff_WormLikeChainEXV(scalar q, sasfit_param * param)
{
	scalar arg, fxs, xsec, S0, RL, RLL, R;
	int i_exvol;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 4, &S0, &RL, &RLL, &R);

	arg = q*R;
	i_exvol = 1;

	fxs = gsl_pow_2(2.0*sasfit_jinc(arg));

	if (RL == 0.0) RL=1e-6;
	if (RLL == 0.0) RLL=1e-6;
	if (q==0.0) {
		xsec = S0*fxs;
	} else {
		xsec = S0 * s_kp_exv__(&q, &RL, &RLL,&i_exvol) * fxs;
	}

	return xsec;
}

scalar s_exv_app__(scalar * q)
{
	/* System generated locals */
	scalar ret_val, r__1;
	scalar d__1, d__2, d__3, d__4, d__5;

	/* Local variables */
	static scalar aexp, s_deb__, w, x, y2;

	/* Computing 2nd power */
	r__1 = *q;
	x = r__1 * r__1;
	if (x < 0.01)
	{
		s_deb__ = 1.0 - x * 0.333333333;
	} else
	{
		if (x > 74.0)
		{
			aexp = 0.0;
		} else
		{
			aexp = exp(-x);
		}

		/* Computing 2nd power */
		r__1 = x;
		s_deb__ = (aexp + x - 1.0) * 2.0 / (r__1 * r__1);
	}
	w = (tanh((*q - 1.6251) / 0.13939) + 1.0) * 0.5;
	if (*q < 0.3)
	{
		w = 0.0;
		goto L10;
	}
	d__1 = (*q);
	d__2 = (*q);
	d__3 = (*q);
	d__4 = (*q);
	d__5 = (*q);

	y2 = 1.29532 / pow(d__1, 1.66666667) + 1.79731 / pow(d__2,3.33333333) -
		2.3221670999000001 / pow(d__3, 3.5) + 0.14975 / pow(d__4, 5.1) - 0.9913
		/ pow(d__5, 6.8);
L10:
	ret_val = (1.0 - w) * s_deb__ + w * y2;
	return ret_val;
}


/* ******************************************************************* */
/*      kp chains with excl volume. R/b=0.1 */
/*      S(q) from MC simulations parametreised by the Yoshizaki and */
/*      Yamakawa approach */
scalar s_kp_exv__(scalar *q, scalar *rl, scalar *rll, int *i_exvol)
{
	/* Initialized data */
	static scalar a1[12] /* was [4][3] */ = { -0.1222, 0.3051, -0.0711, 0.0584, 1.761, 2.252, -1.291, 0.6994, -26.04, 20.0, 4.382, 1.594 };
	static scalar a2[8] /* was [4][2] */ = { 0.1212, -.4169, 0.1988, 0.3435, 0.017, -0.4731, 0.1869, 0.335 };
	static scalar b1[9] /* was [3][3] */ = { -0.0699, -0.09, 0.2677, 0.1342, 0.0138, 0.1898, -0.202,	-0.0114, 0.0123 };
	static scalar b2[6] /* was [3][2] */ = { -0.5171, -0.2028, -0.3112, 0.695, -0.3238, -0.5403 };
	static scalar pi = 3.1415927;

	/* System generated locals */
	scalar ret_val, r__1, r__2;
	double d__1, d__2, d__3, d__4;

	/* Local variables */
	static scalar f_ip__, aexp, epsi, aexp1, a;
	static int i__;
	static scalar k, l, u, scale, k_mem__, f_rod__, expan, const__, f1, f2;
	static scalar s2, aa[4], bb[3];
	static int ii, kk;
	static scalar fgamma;
	static scalar chi, f_debye__, psi;
	static int i_exvol__;

/*      RL= CONTOUR LENGTH */
/*      RRL = KUHN LENGTH */
/*              * THAT IS RLL  IS THE STATISTICLA SEGMENT LENGTH */
/*      I_EXVOL = 0 : GAUSSIAN COILS */
/*                1 : EXCLUDED VOLUME EFFECTS INCLUDED */


/*     DIMENSIONLESS UNITS */

	k = *q * *rll;
	l = *rl / *rll;
	/*      WRITE(*,*)RL,RLL,L */
	i_exvol__ = *i_exvol;
	scale = 1.0;
	kk = 0;
	if (k > 10.0)
	{
		k_mem__ = k;
		k = 10.0;
		kk = 1;
	}

/*     R_G^2 OF DEBYE FUNCTION */

	if (i_exvol__ == 0)
	{
		aexp = l * (float)2.;
		if (aexp > (float)74.)
		{
			aexp = (float)0.;
		} else
		{
			aexp = exp(-aexp);
		}
		s2 = l / (float)6. - (float).25 + (float).25 / l - ((float)1. - aexp) / (l * (float)8. * l);

/*     DEBYE FUNCTION */

		u = s2 * k * k;
		aexp = u;
		if (aexp > 74.0)
		{
			aexp = 0.0;
		} else
		{
			aexp = exp(-aexp);
		}
		if (u < 0.01)
		{
			f_debye__ = 1.0 - u * 0.333333333;
		} else
		{
			/* Computing 2nd power */
			r__1 = u;
			f_debye__ = (aexp + u - 1.0) * 2.0 / (r__1 * r__1);
		}

/*     EXCLUDED VOLUME EFFECTS */

	} else {
		aexp = l * 2.0;
		if (aexp > 74.0)
		{
			aexp = 0.;
		} else
		{
			aexp = exp(-aexp);
		}
		epsi = 0.17;
		d__2 = (r__1 = l / 3.12, fabs(r__1));
		d__3 = (r__2 = l / 8.67, fabs(r__2));
		d__1 = pow(d__2, 2.0) + 1.0 + pow(d__3,3.0);
		d__4 = epsi / 3.0;
		expan = pow(d__1, d__4);
		s2 = l / 6.0 - 0.25 + 0.25 / l - (1.0 - aexp) / (l * 8.0 * l);
		s2 *= expan;
		/*       WRITE(*,*)' RG = ', SQRT(S2) */
		/*       WRITE(*,*)' EXP= ', SQRT(EXPAN) */
		r__1 = k * sqrt(s2);
		f_debye__ = s_exv_app__(&r__1);
	}

/*     ROD SCATTERING FUNCTION */

	a = k * l;
	/* Computing 2nd power */
	r__1 = a;
	/* Computing 2nd power */
	r__2 = sin(a * 0.5);
	f_rod__ = sasfit_si(a) * 2.0 / a - 4.0 / (r__1 * r__1) * (r__2 * r__2);

/*     WEIGHT CHI */

	if (i_exvol__ == 0)
	{
		psi = pi * s2 * k / (l * 2.0);
	} else
	{
		d__1 = (pi / (r__1 = l * 1.103, fabs(r__1)));
		d__2 = s2;
		psi = k * pow(d__1, 1.5) * pow(d__2, 1.282);
	}
	/* Computing 5th power */
	r__1 = psi, r__2 = r__1, r__1 *= r__1;
	aexp = 1.0 / (r__2 * (r__1 * r__1));
	if (aexp > 74.0)
	{
		chi = 0.0;
	} else
	{
		chi = exp(-aexp);
	}

/*     INTERPOLATED FUNCTION */

	f_ip__ = ((float)1. - chi) * f_debye__ + chi * f_rod__;

/*     CORRECTION FUNCTION FGAMMA */

/*     CALCULATE A'S */

	aexp = 40.0 / (l * 4.0);
	if (aexp > 74.0)
	{
		aexp = 0.0;
	} else
	{
		aexp = exp(-aexp);
	}
	aexp1 = l * 2.0;
	if (aexp1 > 74.0)
	{
		aexp1 = 0.0;
	} else
	{
		aexp1 = exp(-aexp1);
	}
	for (i__ = 2; i__ <= 5; ++i__)
	{
		aa[i__ - 2] = 0.0;
		for (ii = 0; ii <= 2; ++ii)
		{
			if (ii == 0)
			{
				aa[i__ - 2] += a1[i__ + (ii << 2) - 2] / pow(l, ii) * aexp;
			} else {
				aa[i__ - 2] = aa[i__ - 2] + a1[i__ + (ii << 2) - 2] / pow(l, ii) * aexp + a2[i__ + (ii << 2) - 6] * pow(l,ii) * aexp1;
			}
		}
	}

/*     CALULATE B'S */

	for (i__ = 0; i__ <= 2; ++i__)
	{
		bb[i__] = 0.0;
		for (ii = 0; ii <= 2; ++ii)
		{
			if (ii == 0)
			{
				bb[i__] += b1[i__ + ii * 3] / pow(l, ii);
			} else
			{
				bb[i__] = bb[i__] + b1[i__ + ii * 3] / pow(l, ii) + b2[i__ + ii * 3 - 3] * pow(l, ii) * aexp1;
			}
		}
	}

/*     CALCULATE FGAMMA */

	f1 = 0.0;
	f2 = 0.0;
	for (i__ = 2; i__ <= 5; ++i__)
	{
		f1 += aa[i__ - 2] * pow(psi, i__);
	}
	for (i__ = 0; i__ <= 2; ++i__)
	{
		f2 += bb[i__] / pow(psi, i__);
	}
	fgamma = (1.0 - chi) * f1 + 1.0 + chi * f2;

	/*     FINAL FUNCTION */

	/*      REDUCE GAMMA BY TRIAL AND ERROR FOR EXCL. VOLUME EFFECTS */

	/*       WRITE(*,*)K,FGAMMA */
	if (i_exvol__ == 1)
	{
		fgamma = scale * (fgamma - 1.0) + 1.0;
	}

/*     LARGE ARGUMENT EXPANSION */

	if (kk == 0)
	{
		ret_val = f_ip__ * fgamma;
		return ret_val;
	} else
	{
		const__ = (f_ip__ * fgamma - pi / (l * 10.0)) * 100.0;
		ret_val = pi / (k_mem__ * l) + const__ / (k_mem__ * k_mem__);
		return ret_val;
	}
	return ret_val;
}

