/*
 * src/sasfit_sq/sasfit_sq_WormLikeChain.c
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
#include <gsl/gsl_sf.h>
#include "../include/sasfit_sq_utils.h"

scalar sq_exv_app__(scalar * q);
scalar sq_kp_exv__(scalar *q, scalar *rl, scalar *rll, int *i_exvol);
scalar IDebye(scalar q, scalar Rg);
scalar IRod(scalar q, scalar H);
scalar fRg2(scalar cl, scalar lb);
scalar falpha2(scalar cl, scalar lb);
scalar S_SB(scalar q, scalar Rg, sasfit_param *param);
scalar S_EXV(scalar q, scalar Rg, sasfit_param *param);
scalar fcorr (scalar q, scalar RG, sasfit_param *param);

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

scalar sasfit_sq_Pprime_WormLikeChainEXV(scalar q, sasfit_param * param)
/*
	Code taken from lsqres.for and converted by f2c into C. 
	lsqres has been written by Jan Skov Pedersen
*/
{
	scalar arg, fxs, xsec, S0, RL, RRL, R;
	int i_exvol;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 2, &RRL, &RL);

	S0 = RL*RL;
	R= 0.0;

	arg = q*R;
	i_exvol = 1;

	if (arg == 0.0) 
	{
		fxs = 1.0;
	} else 
	{
		fxs = pow((2.0*sasfit_bessj1(arg)/arg),2.0);
	}

	if (RL == 0.0) RL=1e-6;
	if (RRL == 0.0) RRL=1e-6;
	xsec = S0 * sq_kp_exv__(&q, &RL, &RRL,&i_exvol) * fxs;

	return xsec;
}


scalar sasfit_sq_Pprime_WormLikeChainGaussian(scalar q, sasfit_param * param)
{
/*
	Code taken from lsqres.for and converted by f2c into C. 
	lsqres has been written by Jan Skov Pedersen
*/

	scalar arg, fxs, xsec, S0, RL, RRL, R;
	int i_exvol;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 2, &RRL, &RL);

	S0 = RL*RL;
	R= 0.0;

	arg = q*R;
	i_exvol = 0;

	if (arg == 0.0) 
	{
		fxs = 1.0;
	} else 
	{
		fxs = pow((2.0*sasfit_bessj1(arg)/arg),2.0);
	}

	if (RL == 0.0) RL=1e-6;
	if (RRL == 0.0) RRL=1e-6;
	xsec = S0 * sq_kp_exv__(&q, &RL, &RRL,&i_exvol) * fxs;

	return xsec;
}

scalar sq_exv_app__(scalar * q)
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
scalar sq_kp_exv__(scalar *q, scalar *rl, scalar *rll, int *i_exvol)
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
/*      RLL = KUHN LENGTH */
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
		f_debye__ = sq_exv_app__(&r__1);
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

#define B		param->p[0]
#define L		param->p[1]
#define EXVOL	param->p[2]
#define CLB		param->p[MAXPAR-1]


scalar sasfit_sq_Pprime_WormPS1(scalar q, sasfit_param *param)
// J. S. Pedersen and P. Schurtenberger, 
// Scattering Functions of Semiflexible Polymers with and without Excluded Volume Effects, 
// Macromolecules 1996, 29, 7602-7612
// DOI: 10.1021/ma9607630
// here method 1 of the above paper is implemented
{
	double AEXP1, AEXP2;
	double Schain, Srod, SDebye, Swc, chi, xi, Rg2_0, nb, WGAMMA, q_mem;
	double a1[6][3], a2[6][3], b1[3][3], b2[3][3];
	int i,j;
	double AA[6]={0,0,0,0,0};
	double BB[3]={0,0,0};
double na1[6][3] = {
	{ 0.0,		 0.0,		 0.0	}, // a1[0][*]
	{ 0.0,		 0.0,		 0.0	}, // a1[1][*]
//	{ 0.3054,	 0.2316,	-22.779	}, // a1[2][*]
	{ 0.3054,	 0.2316,	-22.25	}, // a1[2][*]
//	{ 0.05777,	 2.6531,	 23.2457}, // a1[3][*]
	{ 0.05777,	 2.6531,	 23.25}, // a1[3][*]
	{-0.00604,	 0.3706,	 8.1092	}, // a1[4][*]
	{-0.03902,	-1.0081,	-3.3603	}  // a1[5][*]
};
double na2[6][3] = {
	{ 0.0,	 0.0,		 0.0	}, // a2[0][*]
	{ 0.0,	 0.0,		 0.0	}, // a2[1][*]
	{ 0.0,	-0.4963,	-0.4678	}, // a2[2][*]
	{ 0.0,	 0.03688,	 0.3365	}, // a2[3][*]
	{ 0.0,	 0.30570,	 0.4290	}, // a2[4][*]
	{ 0.0,	 0.39013,	 0.3737	}  // a2[5][*]
};
double nb1[3][3] = {
	{-0.0162,	-0.3565,	-0.3078	},
	{ 0.09046,	 0.1909,	 0.05176},
	{ 0.1213,	 0.15634,	 0.01568}
};
double nb2[3][3] = {
	{ 0.0,	-0.3946,	 1.1361	},
	{ 0.0,	-0.2231,	-0.01615},
	{ 0.0,	-0.2546,	-0.07606}
};

double xa1[6][3] = {
	{ 0.0,		 0.0,		 0.0	}, // xa1[0][*]
	{ 0.0,		 0.0,		 0.0	}, // xa1[1][*]
	{-0.1222,	 1.761,		-26.04	}, // xa1[2][*]
	{ 0.3051,	 2.252,		 20.00	}, // xa1[3][*]
	{-0.0711,	-1.291,		 4.382	}, // xa1[4][*]
	{ 0.0584,	 0.6994,	 1.594	}  // xa1[5][*]
};
double xa2[6][3] = {
	{ 0.0,	 0.0,		 0.0	}, // a2[0][*]
	{ 0.0,	 0.0,		 0.0	}, // a2[1][*]
	{ 0.0,	 0.1212,	 0.0170	}, // a2[2][*]
	{ 0.0,	-0.4169,	-0.4731	}, // a2[3][*]
	{ 0.0,	 0.1988,	 0.1869	}, // a2[4][*]
	{ 0.0,	 0.3435,	 0.3350	}  // a2[5][*]
};
double xb1[3][3] = {
	{-0.0699,	 0.1342,	-0.2020},
	{-0.0900,	 0.0138,	-0.0114},
	{ 0.2677,	 0.1898,	 0.0123}
};
double xb2[3][3] = {
	{ 0.0,	-0.5171,	 0.6950	},
	{ 0.0,	-0.2028,	-0.3238},
	{ 0.0,	-0.3112,	-0.5403}
};

SASFIT_ASSERT_PTR(param);

if (EXVOL >= 1) {
	for (i=0;i<6;i++){
		for (j=0;j<3;j++){
			a1[i][j] = xa1[i][j];
			a2[i][j] = xa2[i][j];
		}
	}
	for (i=0;i<3;i++){
		for (j=0;j<3;j++){
			b1[i][j] = xb1[i][j];
			b2[i][j] = xb2[i][j];
		}
	}
} else {
	for (i=0;i<6;i++){
		for (j=0;j<3;j++){
			a1[i][j] = na1[i][j];
			a2[i][j] = na2[i][j];
		}
	}
	for (i=0;i<3;i++){
		for (j=0;j<3;j++){
			b1[i][j] = nb1[i][j];
			b2[i][j] = nb2[i][j];
		}
	}
}

nb = L/B;

q_mem = q;
if (q_mem*B > 10.) {
	q = 10./B;
}


if (EXVOL < 1) {
	Rg2_0 = fRg2(L,B);
	xi = q*M_PI/(2.0*L)*Rg2_0;
	if (1.0/gsl_pow_5(xi) > 100) {
		chi = 0.0;
	} else {
		chi = exp(-1.0/gsl_pow_5(xi));
	}
} else {
	Rg2_0 = fRg2(L,B)*falpha2(L,B);
	xi = q*B*pow(M_PI*B/(1.103*L),3./2.) * pow(Rg2_0/(B*B),1.282);
	if (1.0/gsl_pow_5(xi) > 100) {
		chi = 0;
	} else {
		chi = exp(-1.0/gsl_pow_5(xi));
	}
}

// CORRECTION FUNCTION FGAMMA
// CALCULATE A'S

AEXP1 = 10./nb;
if (AEXP1 > 100) {
	AEXP1 = 0;
} else {
	AEXP1 = exp(-AEXP1);
}
AEXP2 = 2.*nb;
if (AEXP2 > 100) {
	AEXP2 = 0;
} else {
	AEXP2 = exp(-AEXP2);
}

for (i=2;i<=5;i++) {
	AA[i] = 0.0;
	for (j=0;j<=2;j++) {
		if (j==0) {
			AA[i] =   AA[i]
			        + a1[i][j]*pow(nb,-j)*AEXP1;
		} else {
			AA[i] =   AA[i]
			        + a1[i][j]*pow(nb,-j)*AEXP1 
				    + a2[i][j]*pow(nb,j)*AEXP2;
		}
	}
}


// CALCULATE B'S

for (i=0;i<=2;i++) {
	BB[i] = 0.0;
	for (j=0;j<=2;j++) {
		if (j==0) {
			BB[i] =   BB[i]
			        + b1[i][j]*pow(nb,-j);
		} else {
			BB[i] =   BB[i]
			        + b1[i][j]*pow(nb,-j) 
				    + b2[i][j]*pow(nb,j)*AEXP2;
		}
	}
}

// CALCULATE WGAMMA

WGAMMA = 1.0;
for (i=2;i<=5;i++) {
	WGAMMA = WGAMMA+(1.0-chi)*AA[i]*pow(xi,i);
}
for (i=0;i<=2;i++) {
	WGAMMA = WGAMMA+chi*BB[i]*pow(xi,-i);
}
// interpolated function
if (EXVOL < 1) {
	SDebye = IDebye(q,sqrt(Rg2_0));
	Schain = SDebye;
} else {
	CLB = 1;
	Schain = S_EXV(q,sqrt(Rg2_0),param);
}
Srod = IRod(q,L);

// The large argument expansion for q*B>10 has been done as in lsqres.for. However, this expansion 
// is not described in the original publication. 
// Without this expansion S(q) does not properly converges to the proper potential behaviour.
if (q_mem*B<=10) {
	Swc = ((1.0-chi)*Schain + chi*Srod)*WGAMMA;
} else {
	Swc = 100.*(((1.0-chi)*Schain + chi*Srod)*WGAMMA-M_PI/(10.*nb));
	Swc = M_PI/(q_mem*L)+Swc/(q_mem*q_mem*B*B);
}
return Swc*L*L;
}

#define B		param->p[0]
#define L		param->p[1]

scalar sasfit_sq_Pprime_WormPS2(scalar q, sasfit_param *param)
// J. S. Pedersen and P. Schurtenberger, 
// Scattering Functions of Semiflexible Polymers with and without Excluded Volume Effects, 
// Macromolecules 1996, 29, 7602-7612
// DOI: 10.1021/ma9607630
// here method 2 of the above paper is implemented. This method is only 
// without excluded volume effects
{
	double nb, S, SDebye, SSB, Slocal, q1, q2, p1, p2, a1, a2, Rg2;
	
    SASFIT_ASSERT_PTR(param);

	nb = L/B;

	if (nb > 2) {
		Rg2 = L*B/6.0;
		q1 = 5.53;
		p1 = 5.33;
		a1 = 1.0;
		Slocal = a1/(L*B*q*q)+M_PI/(L*q);
		SSB = S_SB(q,sqrt(Rg2),param);
		S =   SSB    *     exp(-pow(q*B/q1,p1))
			+ Slocal * (1.-exp(-pow(q*B/q1,p1)));
	} else {
		a1 = 0.0625;
		a2 = 11.7;
		p2 = 3.95;
		q2 = a2/nb;
		Rg2 = fRg2(L,B);
		SDebye = IDebye(q,sqrt(Rg2));
		Slocal = a1/(L*B*q*q)+M_PI/(L*q);
		S =   SDebye *     exp(-pow(q*B/q2,p2))
			+ Slocal * (1.-exp(-pow(q*B/q2,p2)));
	}
	return L*L*S;
}

#define B		param->p[0]
#define L		param->p[1]
#define EXVOL	param->p[2]

scalar sasfit_sq_Pprime_WormPS3(scalar q, sasfit_param *param)
// J. S. Pedersen and P. Schurtenberger, 
// Scattering Functions of Semiflexible Polymers with and without Excluded Volume Effects, 
// Macromolecules 1996, 29, 7602-7612
// DOI: 10.1021/ma9607630
// here method 3 of the above paper is implemented.
{
	scalar a1,a2,a3,a4,p1,p2,p3, q0, u, RG;
	scalar MapleGenVar1, MapleGenVar2, MapleGenVar3, MapleGenVar4, MapleGenVar5, MapleGenVar6;
	scalar C1, C2, C3, C4, C5, epsilon, nu;

	C1 = 1.2220;
	C2 = 0.4288;
	C3 = -1.651;
	epsilon = 0.170;
	nu = (1.0+epsilon)/2.0;
	C4 = 1.523;
	C5 = 0.1477;

	CLB = 1;
	a4 = 3.06;
	p3 = 0.44;
	a3 = 2.02;

	if (L>4.0*B) {
		q0 = 3.1;
	} else {
		if (EXVOL <=0) {
			q0=a3*B/sqrt(fRg2(L,B));
			if (q0<4.0) q0=4.0;
		} else {
			q0=a3*B/sqrt(fRg2(L,B)*falpha2(L,B));
			if (q0<3.0) q0=3.0;
		}
	}

	if (EXVOL <= 0) {
		if (L>4*B) {
			if (q*B<q0) {
				return L*L*S_SB(q,sqrt(L*B/6.0),param);
			} else {
				p1 = 4.95;
				p2 = 5.29;
				RG = sqrt(fRg2(L,B));
				RG = sqrt(L*B/6.0);
				a1 = -pow(q0,1.0*p1-4.0)*(-60.0*q0*q0*RG*RG*B*B*B*L+30.0*B*B*B*B*B*L*exp(
-q0*q0/(B*B)*RG*RG)*p2-30.0*B*B*B*B*B*L*p2-60.0*q0*q0*RG*RG*B*B*B*L*exp(-q0*q0/
(B*B)*RG*RG)+14.0*CLB*q0*q0*RG*RG*exp(-q0*q0/(B*B)*RG*RG)*B*B*B*B+22.0*CLB*q0*
q0*q0*q0*q0*q0*RG*RG*RG*RG*RG*RG*exp(-q0*q0/(B*B)*RG*RG)+14.0*CLB*q0*q0*q0*q0*
RG*RG*RG*RG*exp(-q0*q0/(B*B)*RG*RG)*B*B+15.0*B*B*q0*q0*q0*RG*RG*RG*RG*
0.3141592653589793E1-14.0*CLB*q0*q0*RG*RG*B*B*B*B+120.0*B*B*B*B*B*L-120.0*B*B*B
*B*B*L*exp(-q0*q0/(B*B)*RG*RG)+30.0*q0*q0*RG*RG*B*B*B*L*p2-15.0*B*B*q0*q0*q0*RG
*RG*RG*RG*0.3141592653589793E1*p2-11.0*CLB*q0*q0*q0*q0*RG*RG*RG*RG*exp(-q0*q0/(
B*B)*RG*RG)*B*B*p2+4.0*B*B*CLB*q0*q0*q0*q0*RG*RG*RG*RG*p2+7.0*CLB*q0*q0*RG*RG*B
*B*B*B*p2-7.0*CLB*q0*q0*RG*RG*exp(-q0*q0/(B*B)*RG*RG)*B*B*B*B*p2)/L/(RG*RG*RG*
RG)/B/(-p2+p1)/15.0;
				a2 = pow(q0,1.0*p2-4.0)*(-60.0*q0*q0*RG*RG*B*B*B*L*exp(-q0*q0/(B*B)*RG*RG
)-60.0*q0*q0*RG*RG*B*B*B*L-120.0*B*B*B*B*B*L*exp(-q0*q0/(B*B)*RG*RG)+120.0*B*B*
B*B*B*L-14.0*CLB*q0*q0*RG*RG*B*B*B*B+14.0*CLB*q0*q0*RG*RG*exp(-q0*q0/(B*B)*RG*
RG)*B*B*B*B+22.0*CLB*q0*q0*q0*q0*q0*q0*RG*RG*RG*RG*RG*RG*exp(-q0*q0/(B*B)*RG*RG
)+14.0*CLB*q0*q0*q0*q0*RG*RG*RG*RG*exp(-q0*q0/(B*B)*RG*RG)*B*B-15.0*B*B*p1*
0.3141592653589793E1*q0*q0*q0*RG*RG*RG*RG+15.0*B*B*q0*q0*q0*RG*RG*RG*RG*
0.3141592653589793E1+30.0*B*B*B*p1*L*q0*q0*RG*RG-30.0*L*p1*B*B*B*B*B+4.0*B*B*p1
*CLB*q0*q0*q0*q0*RG*RG*RG*RG+30.0*B*B*B*B*B*p1*L*exp(-q0*q0/(B*B)*RG*RG)-11.0*B
*B*p1*CLB*q0*q0*q0*q0*RG*RG*RG*RG*exp(-q0*q0/(B*B)*RG*RG)+7.0*B*B*B*B*p1*CLB*q0
*q0*RG*RG-7.0*B*B*B*B*p1*CLB*q0*q0*RG*RG*exp(-q0*q0/(B*B)*RG*RG))/B/(RG*RG*RG*
RG)/L/(-p2+p1)/15.0;
				return L*L*(a1/pow(q*B,p1)+a2/pow(q*B,p2)+M_PI/(q*L));
			}
		} else {
			if (q*B<q0) {
				return L*L*IDebye(q,sqrt(fRg2(L,B)));
			} else {
				RG = sqrt(fRg2(L,B));
				p1 = 5.13;
				p2 = 7.47;
				CLB = 1;
				a1 = B*pow(q0,1.0*p1-4.0)*(-2.0*B*L*q0*q0*RG*RG*p2+4.0*B*L*q0*q0*RG*RG*
exp(-q0*q0/(B*B)*RG*RG)+4.0*B*L*q0*q0*RG*RG+8.0*B*B*B*L*exp(-q0*q0/(B*B)*RG*RG)
-8.0*B*B*B*L-0.3141592653589793E1*q0*q0*q0*RG*RG*RG*RG+0.3141592653589793E1*q0*
q0*q0*RG*RG*RG*RG*p2-2.0*B*B*B*L*exp(-q0*q0/(B*B)*RG*RG)*p2+2.0*B*B*B*L*p2)/L/(
RG*RG*RG*RG)/(p1-p2);
				a2 = -B*pow(q0,1.0*p2-4.0)*(4.0*B*L*q0*q0*RG*RG*exp(-q0*q0/(B*B)*RG*RG)+
4.0*B*L*q0*q0*RG*RG+8.0*B*B*B*L*exp(-q0*q0/(B*B)*RG*RG)-8.0*B*B*B*L+p1*
0.3141592653589793E1*q0*q0*q0*RG*RG*RG*RG-0.3141592653589793E1*q0*q0*q0*RG*RG*
RG*RG-2.0*p1*B*L*q0*q0*RG*RG-2.0*p1*B*B*B*L*exp(-q0*q0/(B*B)*RG*RG)+2.0*p1*B*B*
B*L)/(RG*RG*RG*RG)/L/(p1-p2);
				return L*L*(a1/pow(q*B,p1)+a2/pow(q*B,p2)+M_PI/(q*L));
			}
		}
	} else { // with excluded volume effects: EXVOL > 0
		if (L>4*B) {
			RG = sqrt(falpha2(L,B)*L*B/6.0);
			if (L>10.0*B) {
				CLB = a4/pow(L/B,p3);
			} else {
				CLB = 1;
			}
			if (q*B<=q0) {
				u = gsl_pow_2(RG*q);
				return L*L*(S_EXV(q,RG,param) + CLB*(4./15.+7./(15*u)-(11./15.+7./(15.*u))*exp(-u))*B/L);
			} else {
				p1 = 4.12;
				p2 = 4.42;
				MapleGenVar5 = 4.0*pow(q0,1.0*p1+2.0)*RG*RG*B*B*B*C5*nu*L*exp(-2.0*(-q0*
RG+C4*B)/B/C5)-44.0/15.0*pow(q0,1.0*p1+6.0)*CLB*RG*RG*RG*RG*RG*RG*C5*nu*exp(-(
RG*RG*C5*q0*q0-2.0*B*q0*RG+2.0*C4*B*B)/(B*B)/C5)-pow(q0,1.0*p1+4.0)*B*C5*nu*L*
RG*RG*RG*RG*C1*pow(q0/B*RG,-1.0/nu)*exp(-2.0*(-q0*RG+C4*B)/B/C5)*p2-8.0/15.0*
pow(q0,1.0*p1+4.0)*B*B*C5*nu*exp(-2.0*(-q0*RG+C4*B)/B/C5)*CLB*RG*RG*RG*RG*p2
-14.0/15.0*pow(q0,1.0*p1+4.0)*CLB*RG*RG*RG*RG*C5*nu*exp(-q0*q0/(B*B)*RG*RG)*B*B
-22.0/15.0*pow(q0,1.0*p1+6.0)*CLB*RG*RG*RG*RG*RG*RG*C5*nu*exp(-(RG*RG*C5*q0*q0
-4.0*B*q0*RG+4.0*C4*B*B)/(B*B)/C5)+4.0*pow(q0,1.0*p1+1.0)*B*B*B*B*RG*nu*L*exp(-
(RG*RG*C5*q0*q0-2.0*B*q0*RG+2.0*C4*B*B)/(B*B)/C5);
      MapleGenVar4 = MapleGenVar5+4.0*pow(q0,1.0*p1+2.0)*RG*RG*B*B*B*C5*nu*L*
exp(-(RG*RG*C5*q0*q0-2.0*B*q0*RG+2.0*C4*B*B)/(B*B)/C5)-2.0*pow(q0,1.0*p1+2.0)*
RG*RG*B*B*B*C5*nu*L*p2+2.0*pow(q0,1.0*p1+3.0)*B*B*RG*RG*RG*RG*C5*nu*
0.3141592653589793E1*p2*exp(-2.0*(-q0*RG+C4*B)/B/C5)-4.0*pow(q0,1.0*p1+1.0)*B*B
*B*B*RG*nu*L*exp(-2.0*(-q0*RG+C4*B)/B/C5)-28.0/15.0*pow(q0,1.0*p1+4.0)*CLB*RG*
RG*RG*RG*C5*nu*B*B*exp(-(RG*RG*C5*q0*q0-2.0*B*q0*RG+2.0*C4*B*B)/(B*B)/C5)+22.0/
15.0*pow(q0,1.0*p1+4.0)*CLB*RG*RG*RG*RG*C5*nu*B*B*exp(-(RG*RG*C5*q0*q0-2.0*B*q0
*RG+2.0*C4*B*B)/(B*B)/C5)*p2+pow(q0,1.0*p1+3.0)*B*B*RG*RG*RG*RG*C5*nu*
0.3141592653589793E1*p2*exp(-4.0*(-q0*RG+C4*B)/B/C5)-pow(q0,1.0*p1+4.0)*B*C5*nu
*L*RG*RG*RG*RG*C1*pow(q0/B*RG,-1.0/nu)*exp(-4.0*(-q0*RG+C4*B)/B/C5)*p2;
      MapleGenVar5 = MapleGenVar4-B*B*RG*RG*RG*RG*C5*pow(q0,1.0*p1+3.0)*nu*
0.3141592653589793E1+2.0*pow(q0,1.0*p1)*B*B*B*B*B*C5*nu*L*p2-2.0*pow(q0,1.0*p1+
2.0)*RG*RG*B*B*B*C5*nu*L*exp(-2.0*(-q0*RG+C4*B)/B/C5)*p2-7.0/15.0*pow(q0,1.0*p1
+2.0)*CLB*RG*RG*C5*nu*B*B*B*B*exp(-4.0*(-q0*RG+C4*B)/B/C5)*p2+3.0*pow(q0,1.0*p1
+4.0)*RG*RG*RG*RG*C5*B*L*C3*pow(q0/B*RG,-3.0/nu)*exp(-4.0*(-q0*RG+C4*B)/B/C5)+
4.0*pow(q0,1.0*p1+2.0)*RG*RG*B*B*B*C5*nu*L+pow(q0,1.0*p1+3.0)*B*B*RG*RG*RG*RG*
C5*nu*0.3141592653589793E1*p2;
      MapleGenVar6 = MapleGenVar5-14.0/15.0*pow(q0,1.0*p1+2.0)*CLB*RG*RG*C5*nu*
B*B*B*B*exp(-(RG*RG*C5*q0*q0-4.0*B*q0*RG+4.0*C4*B*B)/(B*B)/C5)+2.0*pow(q0,1.0*
p1)*B*B*B*B*B*C5*nu*L*exp(-2.0*(-q0*RG+C4*B)/B/C5)*p2+14.0/15.0*pow(q0,1.0*p1+
2.0)*CLB*RG*RG*C5*nu*B*B*B*B*exp(-4.0*(-q0*RG+C4*B)/B/C5)-pow(q0,1.0*p1+4.0)*B*
C5*nu*L*RG*RG*RG*RG*C3*pow(q0/B*RG,-3.0/nu)*exp(-2.0*(-q0*RG+C4*B)/B/C5)*p2;
      MapleGenVar3 = MapleGenVar6-14.0/15.0*pow(q0,1.0*p1+2.0)*CLB*RG*RG*C5*nu*
B*B*B*B*exp(-2.0*(-q0*RG+C4*B)/B/C5)*p2-pow(q0,1.0*p1+4.0)*B*C5*nu*L*RG*RG*RG*
RG*C2*pow(q0/B*RG,-2.0/nu)*exp(-4.0*(-q0*RG+C4*B)/B/C5)*p2-4.0/15.0*pow(q0,1.0*
p1+4.0)*B*B*C5*nu*CLB*RG*RG*RG*RG*p2-pow(q0,1.0*p1+3.0)*B*B*RG*RG*RG*RG*C5*nu*
0.3141592653589793E1*exp(-4.0*(-q0*RG+C4*B)/B/C5)-2.0*pow(q0,1.0*p1+3.0)*B*B*RG
*RG*RG*RG*C5*nu*0.3141592653589793E1*exp(-2.0*(-q0*RG+C4*B)/B/C5);
      MapleGenVar5 = -28.0/15.0*pow(q0,1.0*p1+2.0)*CLB*RG*RG*C5*nu*B*B*B*B*exp(
-(RG*RG*C5*q0*q0-2.0*B*q0*RG+2.0*C4*B*B)/(B*B)/C5)-pow(q0,1.0*p1+4.0)*B*C5*nu*L
*RG*RG*RG*RG*C3*pow(q0/B*RG,-3.0/nu)*exp(-4.0*(-q0*RG+C4*B)/B/C5)*p2+7.0/15.0*
pow(q0,1.0*p1+2.0)*CLB*RG*RG*C5*nu*exp(-q0*q0/(B*B)*RG*RG)*B*B*B*B*p2+8.0*pow(
q0,1.0*p1)*B*B*B*B*B*C5*nu*L*exp(-(RG*RG*C5*q0*q0-2.0*B*q0*RG+2.0*C4*B*B)/(B*B)
/C5)-2.0*pow(q0,1.0*p1+5.0)*RG*RG*RG*RG*RG*nu*L*C1*pow(q0/B*RG,-1.0/nu)*exp(
-2.0*(-q0*RG+C4*B)/B/C5)-4.0/15.0*pow(q0,1.0*p1+4.0)*B*B*C5*nu*exp(-4.0*(-q0*RG
+C4*B)/B/C5)*CLB*RG*RG*RG*RG*p2-2.0*pow(q0,1.0*p1)*B*B*B*B*B*C5*nu*L*exp(-(RG*
RG*C5*q0*q0-2.0*B*q0*RG+2.0*C4*B*B)/(B*B)/C5)*p2+11.0/15.0*pow(q0,1.0*p1+4.0)*
CLB*RG*RG*RG*RG*C5*nu*exp(-q0*q0/(B*B)*RG*RG)*B*B*p2;
      MapleGenVar6 = MapleGenVar5-2.0*pow(q0,1.0*p1+5.0)*RG*RG*RG*RG*RG*nu*L*C3
*pow(q0/B*RG,-3.0/nu)*exp(-2.0*(-q0*RG+C4*B)/B/C5)+3.0*pow(q0,1.0*p1+4.0)*RG*RG
*RG*RG*C5*B*L*C3*pow(q0/B*RG,-3.0/nu)*exp(-2.0*(-q0*RG+C4*B)/B/C5)+2.0*pow(q0,
1.0*p1+4.0)*RG*RG*RG*RG*C5*B*L*C2*pow(q0/B*RG,-2.0/nu)*exp(-2.0*(-q0*RG+C4*B)/B
/C5);
      MapleGenVar4 = MapleGenVar6-2.0*pow(q0,1.0*p1)*B*B*B*B*B*C5*nu*L*exp(-q0*
q0/(B*B)*RG*RG)*p2+7.0/15.0*pow(q0,1.0*p1+2.0)*CLB*RG*RG*C5*nu*B*B*B*B*exp(-(RG
*RG*C5*q0*q0-4.0*B*q0*RG+4.0*C4*B*B)/(B*B)/C5)*p2+2.0*pow(q0,1.0*p1+4.0)*RG*RG*
RG*RG*C5*B*L*C2*pow(q0/B*RG,-2.0/nu)*exp(-4.0*(-q0*RG+C4*B)/B/C5)+pow(q0,1.0*p1
+4.0)*RG*RG*RG*RG*C5*B*L*C1*pow(q0/B*RG,-1.0/nu)*exp(-4.0*(-q0*RG+C4*B)/B/C5)+
11.0/15.0*pow(q0,1.0*p1+4.0)*CLB*RG*RG*RG*RG*C5*nu*B*B*exp(-(RG*RG*C5*q0*q0-4.0
*B*q0*RG+4.0*C4*B*B)/(B*B)/C5)*p2;
      MapleGenVar5 = MapleGenVar4-8.0*pow(q0,1.0*p1)*B*B*B*B*B*C5*nu*L+14.0/
15.0*pow(q0,1.0*p1+2.0)*CLB*RG*RG*C5*nu*B*B*B*B*exp(-(RG*RG*C5*q0*q0-2.0*B*q0*
RG+2.0*C4*B*B)/(B*B)/C5)*p2+14.0/15.0*pow(q0,1.0*p1+2.0)*CLB*RG*RG*C5*nu*B*B*B*
B+28.0/15.0*pow(q0,1.0*p1+2.0)*CLB*RG*RG*C5*nu*B*B*B*B*exp(-2.0*(-q0*RG+C4*B)/B
/C5)+4.0*pow(q0,1.0*p1+3.0)*B*B*RG*RG*RG*nu*L*exp(-2.0*(-q0*RG+C4*B)/B/C5)-22.0
/15.0*pow(q0,1.0*p1+6.0)*CLB*RG*RG*RG*RG*RG*RG*C5*nu*exp(-q0*q0/(B*B)*RG*RG)
-8.0*pow(q0,1.0*p1)*B*B*B*B*B*C5*nu*L*exp(-2.0*(-q0*RG+C4*B)/B/C5);
      MapleGenVar2 = MapleGenVar5+4.0*pow(q0,1.0*p1+2.0)*RG*RG*B*B*B*C5*nu*L*
exp(-q0*q0/(B*B)*RG*RG)+pow(q0,1.0*p1+4.0)*RG*RG*RG*RG*C5*B*L*C1*pow(q0/B*RG,
-1.0/nu)*exp(-2.0*(-q0*RG+C4*B)/B/C5)-2.0*pow(q0,1.0*p1+5.0)*RG*RG*RG*RG*RG*nu*
L*C2*pow(q0/B*RG,-2.0/nu)*exp(-2.0*(-q0*RG+C4*B)/B/C5)+8.0*B*B*B*B*B*C5*nu*L*
exp(-q0*q0/(B*B)*RG*RG)*pow(q0,1.0*p1)-14.0/15.0*pow(q0,1.0*p1+4.0)*CLB*RG*RG*
RG*RG*C5*nu*B*B*exp(-(RG*RG*C5*q0*q0-4.0*B*q0*RG+4.0*C4*B*B)/(B*B)/C5)-pow(q0,
1.0*p1+4.0)*B*C5*nu*L*RG*RG*RG*RG*C2*pow(q0/B*RG,-2.0/nu)*exp(-2.0*(-q0*RG+C4*B
)/B/C5)*p2-7.0/15.0*pow(q0,1.0*p1+2.0)*CLB*RG*RG*C5*nu*B*B*B*B*p2-14.0/15.0*pow
(q0,1.0*p1+2.0)*CLB*RG*RG*C5*nu*exp(-q0*q0/(B*B)*RG*RG)*B*B*B*B+MapleGenVar3;
      MapleGenVar3 = 1/(q0*q0*q0*q0)/L;
      MapleGenVar1 = MapleGenVar2*MapleGenVar3;
      MapleGenVar2 = 1/(RG*RG*RG*RG)/(p1-p2*exp(-4.0*(-q0*RG+C4*B)/B/C5)-2.0*p2
*exp(-2.0*(-q0*RG+C4*B)/B/C5)+p1*exp(-4.0*(-q0*RG+C4*B)/B/C5)-p2+2.0*p1*exp(
-2.0*(-q0*RG+C4*B)/B/C5))/nu/C5/B;
      a1 = MapleGenVar1*MapleGenVar2;

				MapleGenVar5 = -pow(q0,1.0*p2+3.0)*B*B*C5*nu*p1*0.3141592653589793E1*RG*
RG*RG*RG+7.0/15.0*pow(q0,1.0*p2+2.0)*B*B*B*B*C5*nu*p1*CLB*RG*RG-8.0*pow(q0,1.0*
p2)*B*B*B*B*B*C5*nu*L*exp(-(RG*RG*C5*q0*q0-2.0*B*q0*RG+2.0*C4*B*B)/(B*B)/C5)
-14.0/15.0*pow(q0,1.0*p2+2.0)*B*B*B*B*C5*nu*p1*CLB*RG*RG*exp(-(RG*RG*C5*q0*q0
-2.0*B*q0*RG+2.0*C4*B*B)/(B*B)/C5)-11.0/15.0*pow(q0,1.0*p2+4.0)*B*B*C5*nu*p1*
exp(-(RG*RG*C5*q0*q0-4.0*B*q0*RG+4.0*C4*B*B)/(B*B)/C5)*CLB*RG*RG*RG*RG+14.0/
15.0*pow(q0,1.0*p2+2.0)*CLB*RG*RG*C5*nu*B*B*B*B*exp(-(RG*RG*C5*q0*q0-4.0*B*q0*
RG+4.0*C4*B*B)/(B*B)/C5)-4.0*pow(q0,1.0*p2+1.0)*B*B*B*B*RG*nu*L*exp(-(RG*RG*C5*
q0*q0-2.0*B*q0*RG+2.0*C4*B*B)/(B*B)/C5);
      MapleGenVar4 = MapleGenVar5+44.0/15.0*pow(q0,1.0*p2+6.0)*CLB*RG*RG*RG*RG*
RG*RG*C5*nu*exp(-(RG*RG*C5*q0*q0-2.0*B*q0*RG+2.0*C4*B*B)/(B*B)/C5)+4.0*pow(q0,
1.0*p2+1.0)*B*B*B*B*RG*nu*L*exp(-2.0*(-q0*RG+C4*B)/B/C5)-14.0/15.0*pow(q0,1.0*
p2+2.0)*CLB*RG*RG*C5*nu*B*B*B*B*exp(-4.0*(-q0*RG+C4*B)/B/C5)+14.0/15.0*pow(q0,
1.0*p2+2.0)*B*B*B*B*C5*nu*p1*exp(-2.0*(-q0*RG+C4*B)/B/C5)*CLB*RG*RG+22.0/15.0*
pow(q0,1.0*p2+6.0)*CLB*RG*RG*RG*RG*RG*RG*C5*nu*exp(-q0*q0/(B*B)*RG*RG)+2.0*pow(
q0,1.0*p2+3.0)*B*B*RG*RG*RG*RG*C5*nu*0.3141592653589793E1*exp(-2.0*(-q0*RG+C4*B
)/B/C5)-pow(q0,1.0*p2+4.0)*RG*RG*RG*RG*C5*B*L*C1*pow(q0/B*RG,-1.0/nu)*exp(-2.0*
(-q0*RG+C4*B)/B/C5)+B*B*RG*RG*RG*RG*C5*pow(q0,1.0*p2+3.0)*nu*
0.3141592653589793E1;
      MapleGenVar5 = MapleGenVar4+22.0/15.0*pow(q0,1.0*p2+6.0)*CLB*RG*RG*RG*RG*
RG*RG*C5*nu*exp(-(RG*RG*C5*q0*q0-4.0*B*q0*RG+4.0*C4*B*B)/(B*B)/C5)+2.0*pow(q0,
1.0*p2+2.0)*B*B*B*C5*nu*p1*L*RG*RG+2.0*pow(q0,1.0*p2+5.0)*RG*RG*RG*RG*RG*nu*L*
C2*pow(q0/B*RG,-2.0/nu)*exp(-2.0*(-q0*RG+C4*B)/B/C5)+4.0/15.0*pow(q0,1.0*p2+4.0
)*B*B*C5*nu*p1*exp(-4.0*(-q0*RG+C4*B)/B/C5)*CLB*RG*RG*RG*RG+pow(q0,1.0*p2+4.0)*
B*C5*nu*p1*L*RG*RG*RG*RG*C2*pow(q0/B*RG,-2.0/nu)*exp(-2.0*(-q0*RG+C4*B)/B/C5)-
pow(q0,1.0*p2+3.0)*B*B*C5*nu*p1*exp(-4.0*(-q0*RG+C4*B)/B/C5)*
0.3141592653589793E1*RG*RG*RG*RG+14.0/15.0*pow(q0,1.0*p2+4.0)*CLB*RG*RG*RG*RG*
C5*nu*exp(-q0*q0/(B*B)*RG*RG)*B*B;
      MapleGenVar3 = MapleGenVar5-2.0*pow(q0,1.0*p2)*B*B*B*B*B*C5*nu*p1*exp(
-2.0*(-q0*RG+C4*B)/B/C5)*L+8.0*pow(q0,1.0*p2)*B*B*B*B*B*C5*nu*L*exp(-2.0*(-q0*
RG+C4*B)/B/C5)+2.0*pow(q0,1.0*p2)*B*B*B*B*B*C5*nu*p1*L*exp(-q0*q0/(B*B)*RG*RG)
-3.0*pow(q0,1.0*p2+4.0)*RG*RG*RG*RG*C5*B*L*C3*pow(q0/B*RG,-3.0/nu)*exp(-2.0*(-
q0*RG+C4*B)/B/C5)+2.0*pow(q0,1.0*p2+2.0)*B*B*B*C5*nu*p1*L*RG*RG*exp(-2.0*(-q0*
RG+C4*B)/B/C5)-8.0*B*B*B*B*B*C5*nu*L*exp(-q0*q0/(B*B)*RG*RG)*pow(q0,1.0*p2)+2.0
*pow(q0,1.0*p2+5.0)*RG*RG*RG*RG*RG*nu*L*C3*pow(q0/B*RG,-3.0/nu)*exp(-2.0*(-q0*
RG+C4*B)/B/C5)+2.0*pow(q0,1.0*p2)*B*B*B*B*B*C5*nu*p1*L*exp(-(RG*RG*C5*q0*q0-2.0
*B*q0*RG+2.0*C4*B*B)/(B*B)/C5)-11.0/15.0*pow(q0,1.0*p2+4.0)*B*B*C5*nu*p1*CLB*RG
*RG*RG*RG*exp(-q0*q0/(B*B)*RG*RG);
      MapleGenVar5 = MapleGenVar3-7.0/15.0*pow(q0,1.0*p2+2.0)*B*B*B*B*C5*nu*p1*
CLB*RG*RG*exp(-q0*q0/(B*B)*RG*RG)-2.0*pow(q0,1.0*p2+4.0)*RG*RG*RG*RG*C5*B*L*C2*
pow(q0/B*RG,-2.0/nu)*exp(-2.0*(-q0*RG+C4*B)/B/C5)+pow(q0,1.0*p2+3.0)*B*B*RG*RG*
RG*RG*C5*nu*0.3141592653589793E1*exp(-4.0*(-q0*RG+C4*B)/B/C5)-28.0/15.0*pow(q0,
1.0*p2+2.0)*CLB*RG*RG*C5*nu*B*B*B*B*exp(-2.0*(-q0*RG+C4*B)/B/C5)-7.0/15.0*pow(
q0,1.0*p2+2.0)*B*B*B*B*C5*nu*p1*exp(-(RG*RG*C5*q0*q0-4.0*B*q0*RG+4.0*C4*B*B)/(B
*B)/C5)*CLB*RG*RG+14.0/15.0*pow(q0,1.0*p2+2.0)*CLB*RG*RG*C5*nu*exp(-q0*q0/(B*B)
*RG*RG)*B*B*B*B-2.0*pow(q0,1.0*p2+3.0)*B*B*C5*nu*p1*exp(-2.0*(-q0*RG+C4*B)/B/C5
)*0.3141592653589793E1*RG*RG*RG*RG;
      MapleGenVar4 = MapleGenVar5+2.0*pow(q0,1.0*p2+5.0)*RG*RG*RG*RG*RG*nu*L*C1
*pow(q0/B*RG,-1.0/nu)*exp(-2.0*(-q0*RG+C4*B)/B/C5)-2.0*pow(q0,1.0*p2)*B*B*B*B*B
*C5*nu*p1*L-14.0/15.0*pow(q0,1.0*p2+2.0)*CLB*RG*RG*C5*nu*B*B*B*B-22.0/15.0*pow(
q0,1.0*p2+4.0)*B*B*C5*nu*p1*CLB*RG*RG*RG*RG*exp(-(RG*RG*C5*q0*q0-2.0*B*q0*RG+
2.0*C4*B*B)/(B*B)/C5)-pow(q0,1.0*p2+4.0)*RG*RG*RG*RG*C5*B*L*C1*pow(q0/B*RG,-1.0
/nu)*exp(-4.0*(-q0*RG+C4*B)/B/C5)+pow(q0,1.0*p2+4.0)*B*C5*nu*p1*exp(-4.0*(-q0*
RG+C4*B)/B/C5)*L*RG*RG*RG*RG*C3*pow(q0/B*RG,-3.0/nu)+4.0/15.0*pow(q0,1.0*p2+4.0
)*B*B*C5*nu*p1*CLB*RG*RG*RG*RG+14.0/15.0*pow(q0,1.0*p2+4.0)*CLB*RG*RG*RG*RG*C5*
nu*B*B*exp(-(RG*RG*C5*q0*q0-4.0*B*q0*RG+4.0*C4*B*B)/(B*B)/C5);
      MapleGenVar5 = MapleGenVar4-3.0*pow(q0,1.0*p2+4.0)*RG*RG*RG*RG*C5*B*L*C3*
pow(q0/B*RG,-3.0/nu)*exp(-4.0*(-q0*RG+C4*B)/B/C5)+8.0*pow(q0,1.0*p2)*B*B*B*B*B*
C5*nu*L-4.0*pow(q0,1.0*p2+2.0)*RG*RG*B*B*B*C5*nu*L*exp(-(RG*RG*C5*q0*q0-2.0*B*
q0*RG+2.0*C4*B*B)/(B*B)/C5)-2.0*pow(q0,1.0*p2+4.0)*RG*RG*RG*RG*C5*B*L*C2*pow(q0
/B*RG,-2.0/nu)*exp(-4.0*(-q0*RG+C4*B)/B/C5)-4.0*pow(q0,1.0*p2+3.0)*B*B*RG*RG*RG
*nu*L*exp(-2.0*(-q0*RG+C4*B)/B/C5)+28.0/15.0*pow(q0,1.0*p2+2.0)*CLB*RG*RG*C5*nu
*B*B*B*B*exp(-(RG*RG*C5*q0*q0-2.0*B*q0*RG+2.0*C4*B*B)/(B*B)/C5)-4.0*pow(q0,1.0*
p2+2.0)*RG*RG*B*B*B*C5*nu*L*exp(-q0*q0/(B*B)*RG*RG);
      MapleGenVar6 = MapleGenVar5+7.0/15.0*pow(q0,1.0*p2+2.0)*B*B*B*B*C5*nu*p1*
exp(-4.0*(-q0*RG+C4*B)/B/C5)*CLB*RG*RG+pow(q0,1.0*p2+4.0)*B*C5*nu*p1*exp(-4.0*(
-q0*RG+C4*B)/B/C5)*L*RG*RG*RG*RG*C2*pow(q0/B*RG,-2.0/nu)+8.0/15.0*pow(q0,1.0*p2
+4.0)*B*B*C5*nu*p1*exp(-2.0*(-q0*RG+C4*B)/B/C5)*CLB*RG*RG*RG*RG+pow(q0,1.0*p2+
4.0)*B*C5*nu*p1*L*RG*RG*RG*RG*C3*pow(q0/B*RG,-3.0/nu)*exp(-2.0*(-q0*RG+C4*B)/B/
C5);
      MapleGenVar2 = MapleGenVar6-4.0*pow(q0,1.0*p2+2.0)*RG*RG*B*B*B*C5*nu*L+
pow(q0,1.0*p2+4.0)*B*C5*nu*p1*L*RG*RG*RG*RG*C1*pow(q0/B*RG,-1.0/nu)*exp(-2.0*(-
q0*RG+C4*B)/B/C5)+28.0/15.0*pow(q0,1.0*p2+4.0)*CLB*RG*RG*RG*RG*C5*nu*B*B*exp(-(
RG*RG*C5*q0*q0-2.0*B*q0*RG+2.0*C4*B*B)/(B*B)/C5)-4.0*pow(q0,1.0*p2+2.0)*RG*RG*B
*B*B*C5*nu*L*exp(-2.0*(-q0*RG+C4*B)/B/C5)+pow(q0,1.0*p2+4.0)*B*C5*nu*p1*exp(
-4.0*(-q0*RG+C4*B)/B/C5)*L*RG*RG*RG*RG*C1*pow(q0/B*RG,-1.0/nu);
      MapleGenVar3 = 1/B/C5;
      MapleGenVar1 = MapleGenVar2*MapleGenVar3;
      MapleGenVar2 = 1/nu/(q0*q0*q0*q0)/L/(RG*RG*RG*RG)/(p1-p2*exp(-4.0*(-q0*RG
+C4*B)/B/C5)-2.0*p2*exp(-2.0*(-q0*RG+C4*B)/B/C5)+p1*exp(-4.0*(-q0*RG+C4*B)/B/C5
)-p2+2.0*p1*exp(-2.0*(-q0*RG+C4*B)/B/C5));
      a2 = MapleGenVar1*MapleGenVar2;

					return L*L*(a1/pow(q*B,p1)+a2/pow(q*B,p2)+M_PI/(q*L));
			}
		} else {
			RG = sqrt(fRg2(L,B)*falpha2(L,B));
			q0=a3*B/sqrt(fRg2(L,B));
			if (q0<4.0) {
				q0=4.0;
			}
			if (q*B<=q0) { 
				return L*L*IDebye(q,RG);
			} else {
				p1 = 5.36;
				p2 = 5.62;
				a1 = B*pow(q0,1.0*p1-4.0)*(-2.0*B*L*q0*q0*RG*RG*p2+4.0*B*L*q0*q0*RG*RG*
exp(-q0*q0/(B*B)*RG*RG)+4.0*B*L*q0*q0*RG*RG+8.0*B*B*B*L*exp(-q0*q0/(B*B)*RG*RG)
-8.0*B*B*B*L-0.3141592653589793E1*q0*q0*q0*RG*RG*RG*RG+0.3141592653589793E1*q0*
q0*q0*RG*RG*RG*RG*p2-2.0*B*B*B*L*exp(-q0*q0/(B*B)*RG*RG)*p2+2.0*B*B*B*L*p2)/L/(
RG*RG*RG*RG)/(p1-p2);
				a2 = -B*pow(q0,1.0*p2-4.0)*(4.0*B*L*q0*q0*RG*RG*exp(-q0*q0/(B*B)*RG*RG)+
4.0*B*L*q0*q0*RG*RG+8.0*B*B*B*L*exp(-q0*q0/(B*B)*RG*RG)-8.0*B*B*B*L+p1*
0.3141592653589793E1*q0*q0*q0*RG*RG*RG*RG-0.3141592653589793E1*q0*q0*q0*RG*RG*
RG*RG-2.0*p1*B*L*q0*q0*RG*RG-2.0*p1*B*B*B*L*exp(-q0*q0/(B*B)*RG*RG)+2.0*p1*B*B*
B*L)/(RG*RG*RG*RG)/L/(p1-p2);
				return L*L*(a1/pow(q*B,p1)+a2/pow(q*B,p2)+M_PI/(q*L));
			}
		}
	}
	return 9999999;
}

scalar S_SB(scalar q, scalar Rg, sasfit_param *param) 
// The scattering function calculated for the
// Daniels approximation by Sharp and Bloomfield
{
	scalar u, nb;
	nb = L/B;
	u = gsl_pow_2(q*Rg);
	return IDebye(q,Rg)+(4./15.+7./(15*u)-(11./15.+7./(15.*u))*exp(-u))/nb;
}

scalar S_EXV(scalar q, scalar Rg, sasfit_param *param)
/*
The expression given for the scattering function for worm like chains with excluded volume effects 
by Utiyama et al.21 consists of a low-q and a large-q expansion. In order to get good agreement at
intermediate q, a large number of terms is included in the high-q expansion and this makes the 
numerical calculations rather slow. An approximate expression has therefore been constructed using
this approximation:
*/
{
	scalar SDebye, C1,C2,C3,w,x,nu,epsilon;
    SDebye = IDebye(q,Rg);
	C1 = 1.2220;
	C2 = 0.4288;
	C3 = -1.651;
	epsilon = 0.170;
	nu = (1.0+epsilon)/2.0;
	x = Rg*q;
	w = (1.+tanh((x-1.523)/0.1477))/2.0;
	return  (1-w)* SDebye
		  + (  w)* (  C1*pow(x,-1.0/nu)
			         +C2*pow(x,-2.0/nu)
			         +C3*pow(x,-3.0/nu))*fcorr(q,Rg,param);
}


scalar IDebye(scalar q, scalar Rg)
// Scattering function (Debye function) for Gaussian Coils with radius of gyration Rg
{
	scalar u;
	u = gsl_pow_2(q*Rg);
	if (u < 0.01) {
		return 1.-1./3.*u;
	} else { 
		return 2.0*(exp(-u)+u-1.0)/gsl_pow_2(u);
	}
}

scalar IRod(scalar q, scalar H)
//  ROD SCATTERING FUNCTION
{
	return 2.0*gsl_sf_Si(q*H)/(q*H)-gsl_pow_2(sin(q*H/2.)/(q*H/2.));
}

scalar fRg2(scalar cl,scalar lb)
// the radius of gyration of ideal semiflexible chains.
{
	scalar nb;
	nb = cl/lb;
	return cl*lb/6.0*(1.-1.5/nb
		               +1.5/gsl_pow_2(nb)
					   -0.75/gsl_pow_3(nb)
					   *(1.-exp(-2*nb)));
}

scalar falpha2(scalar cl, scalar lb)
// 
// alpha^2(x) is the expansion coefficient originating from excluded-volume interactions
//
{
	scalar epsilon = 0.170;
	scalar nb;
	nb = cl/lb;
	return pow( 1.0
	              + gsl_pow_2(nb/3.12)
				  + gsl_pow_3(nb/8.67)
				 ,epsilon/3.0);
}

scalar fcorr(scalar q, scalar RG, sasfit_param *param)
{
/*	
	Correction function as described in the paper
	Wei-Ren Chen, Paul D. Butler, and Linda J. Magid
	Incorporating Intermicellar Interactions in the Fitting of SANS Data
	from Cationic Wormlike Micelles
	Langmuir 2006, 22, 6539-6548
	DOI: 10.1021/la0530440
	f_corr(Q) = 0 for dS(q)/dq > 0
	f_corr(Q) = 1 for dS(q)/dq <= 0
*/
	scalar dS;	
	scalar C1, C2, C3, C4, C5, epsilon, nu;

	C1 = 1.2220;
	C2 = 0.4288;
	C3 = -1.651;
	epsilon = 0.170;
	nu = (1.0+epsilon)/2.0;
	C4 = 1.523;
	C5 = 0.1477;

	dS = -(1.0-pow(tanh((q*RG-C4)/C5),2.0))/(RG*RG*RG)/C5*(2.0*exp(-q*q*RG*RG
)+2.0*q*q*RG*RG-2.0)/(q*q*q*q)/2.0+(1.0/2.0-tanh((q*RG-C4)/C5)/2.0)*(-4.0*q*RG*
RG*exp(-q*q*RG*RG)+4.0*q*RG*RG)/(q*q*q*q)/(RG*RG*RG*RG)-4.0*(1.0/2.0-tanh((q*RG
-C4)/C5)/2.0)*(2.0*exp(-q*q*RG*RG)+2.0*q*q*RG*RG-2.0)/(q*q*q*q*q)/(RG*RG*RG*RG)
+(1.0-pow(tanh((q*RG-C4)/C5),2.0))*RG/C5*(C1*pow(q*RG,-1.0/nu)+C2*pow(q*RG,-2.0
/nu)+C3*pow(q*RG,-3.0/nu))/2.0+(1.0/2.0+tanh((q*RG-C4)/C5)/2.0)*(-C1*pow(q*RG,
-1.0/nu)/nu/q-2.0*C2*pow(q*RG,-2.0/nu)/nu/q-3.0*C3*pow(q*RG,-3.0/nu)/nu/q)+CLB*
(-14.0/15.0/(q*q*q)/(RG*RG)+14.0/15.0/(q*q*q)/(RG*RG)*exp(-q*q*RG*RG)+2.0*(11.0
/15.0+7.0/15.0/(q*q)/(RG*RG))*q*RG*RG*exp(-q*q*RG*RG))*B/L;

	if (dS<0) {
		return 1;
	} else {
		return 0;
	}
}

