/*
 * src/sasfit_common/f2c_utils/sasfit_struve_h0h1.c
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

// this file contains converted FORTRAN code (by f2c)

#include "../include/sasfit_utils_f2c.h"

doublereal sasfit_cheval(integer * n, doublereal * a, doublereal * t);
doublereal sasfit_d1mach(integer *i__);

/* Table of constant values */

static integer c__3 = 3;
static integer c__4 = 4;
static integer c__1 = 1;

doublereal sasfit_struve_h0(doublereal *xvalue)
{
    /* Initialized data */

//    static char fnname[6+1] = "STRVH0";
    static doublereal sixtp5 = 60.5;
    static doublereal two62 = 262.;
    static doublereal thr2p5 = 302.5;
    static doublereal piby4 = .78539816339744830962;
    static doublereal rt2bpi = .79788456080286535588;
    static doublereal twobpi = .63661977236758134308;
    static doublereal arrh0[20] = { .2869648739901322574,
	    -.25405332681618352305,.20774026739323894439,
	    -.2036402956038658514,.12888469086866186016,
	    -.04825632815622261202,.01168629347569001242,
	    -.00198118135642418416,2.4899138512421286e-4,-2.41882791378595e-5,
	    1.87437547993431e-6,-1.1873346074362e-7,6.26984943346e-9,
	    -2.8045546793e-10,1.076941205e-11,-3.5904793e-13,1.049447e-14,
	    -2.7119e-16,6.24e-18,-1.3e-19 };
    static doublereal arrh0a[21] = { 1.99291885751992305515,
	    -.00384232668701456887,-3.287199371235305e-4,
	    -2.941181203703409e-5,-2.67315351987066e-6,-2.4681031075013e-7,
	    -2.295014861143e-8,-2.15682231833e-9,-2.0303506483e-10,
	    -1.934575509e-11,-1.82773144e-12,-1.7768424e-13,-1.643296e-14,
	    -1.71569e-15,-1.3368e-16,-2.077e-17,2e-20,-5.5e-19,1e-19,-4e-20,
	    1e-20 };
    static doublereal ay0asp[13] = { 1.99944639402398271568,
	    -2.8650778647031958e-4,-1.00507279743762e-5,-3.5835941002463e-7,
	    -1.287965120531e-8,-4.6609486636e-10,-1.693769454e-11,
	    -6.1852269e-13,-2.261841e-14,-8.3268e-16,-3.042e-17,-1.15e-18,
	    -4e-20 };
    static doublereal ay0asq[14] = { 1.99542681386828604092,
	    -.00236013192867514472,-7.601538908502966e-5,-2.56108871456343e-6,
	    -8.750292185106e-8,-3.04304212159e-9,-1.0621428314e-10,
	    -3.77371479e-12,-1.3213687e-13,-4.88621e-15,-1.5809e-16,-7.62e-18,
	    -3e-20,-3e-20 };
//    static char errmsg[26+1] = "ARGUMENT TOO LARGE IN SIZE";
    static doublereal zero = 0.;
    static doublereal half = .5;
    static doublereal one = 1.;
    static doublereal eight = 8.;
    static doublereal eleven = 11.;
    static doublereal twenty = 20.;
    static doublereal onehun = 100.;

    /* System generated locals */
    doublereal ret_val, d__1;

    /* Builtin functions */
    double sqrt(), sin(), cos();

    /* Local variables */
    static doublereal xlow, y0val, t, x, xhigh;
    static integer nterm1, nterm2, nterm3, nterm4;
    static integer indsgn;
//    extern /* Subroutine */ int errprn_();
    static doublereal y0p, y0q, xsq, h0as, xmp4;



/*   DESCRIPTION: */

/*      This function calculates the value of the Struve function */
/*      of order 0, denoted H0(x), for the argument XVALUE, defined */

/*         STRVHO(x) = (2/pi) integral{0 to pi/2} sin(x cos(t)) dt */

/*      H0 also satisfies the second-order equation */

/*                 x*D(Df) + Df + x*f = 2x/pi */

/*      The code uses Chebyshev expansions whose coefficients are */
/*      given to 20D. */


/*   ERROR RETURNS: */

/*      As the asymptotic expansion of H0 involves the Bessel function */
/*      of the second kind Y0, there is a problem for large x, since */
/*      we cannot accurately calculate the value of Y0. An error message 
*/
/*      is printed and STRVH0 returns the value 0.0. */


/*   MACHINE-DEPENDENT CONSTANTS: */

/*      NTERM1 - The no. of terms to be used in the array ARRH0. The */
/*               recommended value is such that */
/*                      ABS(ARRH0(NTERM1)) < EPS/100. */

/*      NTERM2 - The no. of terms to be used in the array ARRH0A. The */
/*               recommended value is such that */
/*                      ABS(ARRH0A(NTERM2)) < EPS/100. */

/*      NTERM3 - The no. of terms to be used in the array AY0ASP. The */
/*               recommended value is such that */
/*                      ABS(AY0ASP(NTERM3)) < EPS/100. */

/*      NTERM4 - The no. of terms to be used in the array AY0ASQ. The */
/*               recommended value is such that */
/*                      ABS(AY0ASQ(NTERM4)) < EPS/100. */

/*      XLOW - The value for which H0(x) = 2*x/pi to machine precision, if
 */
/*             abs(x) < XLOW. The recommended value is */
/*                      XLOW = 3 * SQRT(EPSNEG) */

/*      XHIGH - The value above which we are unable to calculate Y0 with 
*/
/*              any reasonable accuracy. An error message is printed and 
*/
/*              STRVH0 returns the value 0.0. The recommended value is */
/*                      XHIGH = 1/EPS. */

/*      For values of EPS and EPSNEG refer to the file MACHCON.TXT. */

/*      The machine-dependent constants are computed internally by */
/*      using the D1MACH subroutine. */


/*   INTRINSIC FUNCTIONS USED: */

/*      ABS, COS, SIN, SQRT. */


/*   OTHER MISCFUN SUBROUTINES USED: */

/*          CHEVAL , ERRPRN, D1MACH */


/*   AUTHOR: */
/*          ALLAN J. MACLEOD */
/*          DEPT. OF MATHEMATICS AND STATISTICS */
/*          UNIVERSITY OF PAISLEY */
/*          HIGH ST. */
/*          PAISLEY */
/*          SCOTLAND */
/*          PA1 2BE */

/*          (e-mail: macl_ms0@paisley.ac.uk ) */


/*   LATEST REVISION: */
/*                   23 January, 1996 */



/*   Start computation */

    x = *xvalue;
    indsgn = 1;
    if (x < zero) {
	x = -x;
	indsgn = -1;
    }

/*   Compute the machine-dependent constants. */

    h0as = sasfit_d1mach(&c__3);
    xhigh = one / sasfit_d1mach(&c__4);

/*   Error test */

    if (abs(*xvalue) > xhigh) {
//	errprn_(fnname, errmsg, 6L, 26L);
	ret_val = zero;
	return ret_val;
    }

/*   continue with machine constants */

    t = h0as / onehun;
    if (x <= eleven) {
	for (nterm1 = 19; nterm1 >= 0; --nterm1) {
	    if ((d__1 = arrh0[nterm1], abs(d__1)) > t) {
		goto L19;
	    }
/* L10: */
	}
L19:
	y0p = sqrt(h0as);
	xlow = y0p + y0p + y0p;
    } else {
	for (nterm2 = 20; nterm2 >= 0; --nterm2) {
	    if ((d__1 = arrh0a[nterm2], abs(d__1)) > t) {
		goto L49;
	    }
/* L40: */
	}
L49:
	for (nterm3 = 12; nterm3 >= 0; --nterm3) {
	    if ((d__1 = ay0asp[nterm3], abs(d__1)) > t) {
		goto L59;
	    }
/* L50: */
	}
L59:
	for (nterm4 = 13; nterm4 >= 0; --nterm4) {
	    if ((d__1 = ay0asq[nterm4], abs(d__1)) > t) {
		goto L69;
	    }
/* L60: */
	}
L69:
	;
    }

/*   Code for abs(x) <= 11 */

    if (x <= eleven) {
	if (x < xlow) {
	    ret_val = twobpi * x;
	} else {
	    t = x * x / sixtp5 - half - half;
	    ret_val = twobpi * x * sasfit_cheval(&nterm1, arrh0, &t);
	}
    } else {

/*   Code for abs(x) > 11 */

	xsq = x * x;
	t = (two62 - xsq) / (twenty + xsq);
	y0p = sasfit_cheval(&nterm3, ay0asp, &t);
	y0q = sasfit_cheval(&nterm4, ay0asq, &t) / (eight * x);
	xmp4 = x - piby4;
	y0val = y0p * sin(xmp4) - y0q * cos(xmp4);
	y0val = y0val * rt2bpi / sqrt(x);
	t = (thr2p5 - xsq) / (sixtp5 + xsq);
	h0as = twobpi * sasfit_cheval(&nterm2, arrh0a, &t) / x;
	ret_val = y0val + h0as;
    }
    if (indsgn == -1) {
	ret_val = -ret_val;
    }
    return ret_val;
}

doublereal sasfit_struve_h1(doublereal *xvalue)
{
    /* Initialized data */

//    static char fnname[6+1] = "STRVH1";
//    static char errmsg[26+1] = "ARGUMENT TOO LARGE IN SIZE";
    static doublereal zero = 0.;
    static doublereal half = .5;
    static doublereal eight = 8.;
    static doublereal nine = 9.;
    static doublereal fiften = 15.;
    static doublereal twenty = 20.;
    static doublereal onehun = 100.;
    static doublereal fortp5 = 40.5;
    static doublereal one82 = 182.;
    static doublereal tw02p5 = 202.5;
    static doublereal rt2bpi = .79788456080286535588;
    static doublereal thpby4 = 2.35619449019234492885;
    static doublereal twobpi = .63661977236758134308;
    static doublereal arrh1[18] = { .17319061083675439319,
	    -.12606917591352672005,.079085761604953575,-.0319649322232187082,
	    .00808040581404918834,-.00136000820693074148,
	    1.6227148619889471e-4,-1.442352451485929e-5,9.9219525734072e-7,
	    -5.44162804918e-8,2.43631662563e-9,-9.077071338e-11,
	    2.85926585e-12,-7.716975e-14,1.80489e-15,-3.694e-17,6.7e-19,
	    -1e-20 };
    static doublereal arrh1a[22] = { 2.01083504951473379407,
	    .00592218610036099903,5.527432269841413e-4,5.269873856311036e-5,
	    5.06374522140969e-6,4.9028736420678e-7,4.763540023525e-8,
	    4.65258652283e-9,4.5465166081e-10,4.472462193e-11,4.37308292e-12,
	    4.3568368e-13,4.18219e-14,4.41044e-15,3.6391e-16,5.558e-17,-4e-20,
	    1.63e-18,-3.4e-19,1.3e-19,-4e-20,1e-20 };
    static doublereal ay1asp[15] = { 2.00135240045889396402,
	    7.1104241596461938e-4,3.665977028232449e-5,1.91301568657728e-6,
	    1.0046911389777e-7,5.30401742538e-9,2.8100886176e-10,
	    1.493886051e-11,7.957842e-13,4.252363e-14,2.27195e-15,1.2216e-16,
	    6.5e-18,3.6e-19,2e-20 };
    static doublereal ay1asq[16] = { 5.99065109477888189116,
	    -.00489593262336579635,-2.3238321307070626e-4,
	    -1.144734723857679e-5,-5.7169926189106e-7,-2.895516716917e-8,
	    -1.47513345636e-9,-7.596537378e-11,-3.90658184e-12,-2.0464654e-13,
	    -1.042636e-14,-5.7702e-16,-2.55e-17,-2.1e-18,2e-20,-2e-20 };

    /* System generated locals */
    doublereal ret_val, d__1;

    /* Builtin functions */
    double sqrt(), sin(), cos();

    /* Local variables */
    static doublereal xm3p4, y1val, xlow1, xlow2, t, x, xhigh;
    static integer nterm1, nterm2, nterm3, nterm4;
//    extern /* Subroutine */ int errprn_();
    static doublereal y1p, y1q, xsq, h1as;



/*   DESCRIPTION: */
/*      This function calculates the value of the Struve function */
/*      of order 1, denoted H1(x), for the argument XVALUE, defined as */

/*                                                                  2 */
/*        STRVH1(x) = (2x/pi) integral{0 to pi/2} sin( x cos(t))*sin t dt 
*/

/*      H1 also satisfies the second-order differential equation */

/*                    2   2                   2            2 */
/*                   x * D f  +  x * Df  +  (x - 1)f  =  2x / pi */

/*      The code uses Chebyshev expansions with the coefficients */
/*      given to 20D. */


/*   ERROR RETURNS: */
/*      As the asymptotic expansion of H1 involves the Bessel function */
/*      of the second kind Y1, there is a problem for large x, since */
/*      we cannot accurately calculate the value of Y1. An error message 
*/
/*      is printed and STRVH1 returns the value 0.0. */


/*   MACHINE-DEPENDENT CONSTANTS: */

/*      NTERM1 - The no. of terms to be used in the array ARRH1. The */
/*               recommended value is such that */
/*                      ABS(ARRH1(NTERM1)) < EPS/100. */

/*      NTERM2 - The no. of terms to be used in the array ARRH1A. The */
/*               recommended value is such that */
/*                      ABS(ARRH1A(NTERM2)) < EPS/100. */

/*      NTERM3 - The no. of terms to be used in the array AY1ASP. The */
/*               recommended value is such that */
/*                      ABS(AY1ASP(NTERM3)) < EPS/100. */

/*      NTERM4 - The no. of terms to be used in the array AY1ASQ. The */
/*               recommended value is such that */
/*                      ABS(AY1ASQ(NTERM4)) < EPS/100. */

/*      XLOW1 - The value of x, below which H1(x) set to zero, if */
/*              abs(x)<XLOW1. The recommended value is */
/*                      XLOW1 = 1.5 * SQRT(XMIN) */

/*     XLOW2 - The value for which H1(x) = 2*x*x/pi to machine precision, 
if*/
/*              abs(x) < XLOW2. The recommended value is */
/*                      XLOW2 = SQRT(15*EPSNEG) */

/*      XHIGH - The value above which we are unable to calculate Y1 with 
*/
/*              any reasonable accuracy. An error message is printed and 
*/
/*              STRVH1 returns the value 0.0. The recommended value is */
/*                      XHIGH = 1/EPS. */

/*      For values of EPS, EPSNEG and XMIN refer to the file MACHCON.TXT. 
*/

/*      The machine-dependent constants are computed internally by */
/*      using the D1MACH subroutine. */


/*   INTRINSIC FUNCTIONS USED: */

/*      ABS, COS, SIN, SQRT. */


/*   OTHER MISCFUN SUBROUTINES USED: */

/*          CHEVAL , ERRPRN, D1MACH */


/*   AUTHOR: */
/*          ALLAN J. MACLEOD */
/*          DEPT. OF MATHEMATICS AND STATISTICS */
/*          UNIVERSITY OF PAISLEY */
/*          HIGH ST. */
/*          PAISLEY */
/*          SCOTLAND */
/*          PA1 2BE */

/*          (e-mail: macl_ms0@paisley.ac.uk) */


/*   LATEST REVISION: */
/*                   23 January, 1996 */



/*   Start computation */

    x = abs(*xvalue);

/*   Compute the machine-dependent constants. */

    xhigh = (half + half) / sasfit_d1mach(&c__4);

/*   Error test */

    if (x > xhigh) {
//	errprn_(fnname, errmsg, 6L, 26L);
	ret_val = zero;
	return ret_val;
    }

/*   continue with machine constants */

    h1as = sasfit_d1mach(&c__3);
    t = h1as / onehun;
    if (x <= nine) {
	for (nterm1 = 17; nterm1 >= 0; --nterm1) {
	    if ((d__1 = arrh1[nterm1], abs(d__1)) > t) {
		goto L19;
	    }
/* L10: */
	}
L19:
	xlow1 = half * sqrt(sasfit_d1mach(&c__1));
	xlow1 = xlow1 + xlow1 + xlow1;
	xlow2 = sqrt(fiften * h1as);
    } else {
	for (nterm2 = 21; nterm2 >= 0; --nterm2) {
	    if ((d__1 = arrh1a[nterm2], abs(d__1)) > t) {
		goto L49;
	    }
/* L40: */
	}
L49:
	for (nterm3 = 14; nterm3 >= 0; --nterm3) {
	    if ((d__1 = ay1asp[nterm3], abs(d__1)) > t) {
		goto L59;
	    }
/* L50: */
	}
L59:
	for (nterm4 = 15; nterm4 >= 0; --nterm4) {
	    if ((d__1 = ay1asq[nterm4], abs(d__1)) > t) {
		goto L69;
	    }
/* L60: */
	}
L69:
	;
    }

/*   Code for abs(x) <= 9 */

    if (x <= nine) {
	if (x < xlow1) {
	    ret_val = zero;
	} else {
	    xsq = x * x;
	    if (x < xlow2) {
		ret_val = twobpi * xsq;
	    } else {
		t = xsq / fortp5 - half - half;
		ret_val = twobpi * xsq * sasfit_cheval(&nterm1, arrh1, &t);
	    }
	}
    } else {

/*   Code for abs(x) > 9 */

	xsq = x * x;
	t = (one82 - xsq) / (twenty + xsq);
	y1p = sasfit_cheval(&nterm3, ay1asp, &t);
	y1q = sasfit_cheval(&nterm4, ay1asq, &t) / (eight * x);
	xm3p4 = x - thpby4;
	y1val = y1p * sin(xm3p4) + y1q * cos(xm3p4);
	y1val = y1val * rt2bpi / sqrt(x);
	t = (tw02p5 - xsq) / (fortp5 + xsq);
	h1as = twobpi * sasfit_cheval(&nterm2, arrh1a, &t);
	ret_val = y1val + h1as;
    }
    return ret_val;
} 

doublereal sasfit_d1mach(integer *i__)
{
    /* Initialized data */

    static struct {
	integer e_1[10];
	doublereal fill_2[1];
	doublereal e_3;
	} equiv_4 = { {1048576, 0, 2146435071, -1, 1017118720, 0, 1018167296, 
		0, 1070810131, 1352628735}, {0}, 0. };


    /* System generated locals */
    doublereal ret_val;

    /* Local variables */
#define log10 ((integer *)&equiv_4 + 8)
#define dmach ((doublereal *)&equiv_4)
#define large ((integer *)&equiv_4 + 2)
#ifdef small
#undef small
#endif
#define small ((integer *)&equiv_4)
#define diver ((integer *)&equiv_4 + 6)
#define right ((integer *)&equiv_4 + 4)


/*  DOUBLE-PRECISION MACHINE CONSTANTS */

/*  D1MACH( 1) = B**(EMIN-1), THE SMALLEST POSITIVE MAGNITUDE. */

/*  D1MACH( 2) = B**EMAX*(1 - B**(-T)), THE LARGEST MAGNITUDE. */

/*  D1MACH( 3) = B**(-T), THE SMALLEST RELATIVE SPACING. */

/*  D1MACH( 4) = B**(1-T), THE LARGEST RELATIVE SPACING. */

/*  D1MACH( 5) = LOG10(B) */

/*  TO ALTER THIS FUNCTION FOR A PARTICULAR ENVIRONMENT, */
/*  THE DESIRED SET OF DATA STATEMENTS SHOULD BE ACTIVATED BY */
/*  REMOVING THE C FROM COLUMN 1. */

/*  WHERE POSSIBLE, OCTAL OR HEXADECIMAL CONSTANTS HAVE BEEN USED */
/*  TO SPECIFY THE CONSTANTS EXACTLY WHICH HAS IN SOME CASES */
/*  REQUIRED THE USE OF EQUIVALENT INTEGER ARRAYS. */




/*     MACHINE CONSTANTS FOR THE BURROUGHS 1700 SYSTEM. */

/*     DATA SMALL(1) / ZC00800000 / */
/*     DATA SMALL(2) / Z000000000 / */

/*     DATA LARGE(1) / ZDFFFFFFFF / */
/*     DATA LARGE(2) / ZFFFFFFFFF / */

/*     DATA RIGHT(1) / ZCC5800000 / */
/*     DATA RIGHT(2) / Z000000000 / */

/*     DATA DIVER(1) / ZCC6800000 / */
/*     DATA DIVER(2) / Z000000000 / */

/*     DATA LOG10(1) / ZD00E730E7 / */
/*     DATA LOG10(2) / ZC77800DC0 / */

/*     MACHINE CONSTANTS FOR THE BURROUGHS 5700 SYSTEM. */

/*     DATA SMALL(1) / O1771000000000000 / */
/*     DATA SMALL(2) / O0000000000000000 / */

/*     DATA LARGE(1) / O0777777777777777 / */
/*     DATA LARGE(2) / O0007777777777777 / */

/*     DATA RIGHT(1) / O1461000000000000 / */
/*     DATA RIGHT(2) / O0000000000000000 / */

/*     DATA DIVER(1) / O1451000000000000 / */
/*     DATA DIVER(2) / O0000000000000000 / */

/*     DATA LOG10(1) / O1157163034761674 / */
/*     DATA LOG10(2) / O0006677466732724 / */

/*     MACHINE CONSTANTS FOR THE BURROUGHS 6700/7700 SYSTEMS. */

/*     DATA SMALL(1) / O1771000000000000 / */
/*     DATA SMALL(2) / O7770000000000000 / */

/*     DATA LARGE(1) / O0777777777777777 / */
/*     DATA LARGE(2) / O7777777777777777 / */

/*     DATA RIGHT(1) / O1461000000000000 / */
/*     DATA RIGHT(2) / O0000000000000000 / */

/*     DATA DIVER(1) / O1451000000000000 / */
/*     DATA DIVER(2) / O0000000000000000 / */

/*     DATA LOG10(1) / O1157163034761674 / */
/*     DATA LOG10(2) / O0006677466732724 / */

/*     MACHINE CONSTANTS FOR THE CDC 6000/7000 SERIES. */

/*     DATA SMALL(1) / 00604000000000000000B / */
/*     DATA SMALL(2) / 00000000000000000000B / */

/*     DATA LARGE(1) / 37767777777777777777B / */
/*     DATA LARGE(2) / 37167777777777777777B / */

/*     DATA RIGHT(1) / 15604000000000000000B / */
/*     DATA RIGHT(2) / 15000000000000000000B / */

/*     DATA DIVER(1) / 15614000000000000000B / */
/*     DATA DIVER(2) / 15010000000000000000B / */

/*     DATA LOG10(1) / 17164642023241175717B / */
/*     DATA LOG10(2) / 16367571421742254654B / */

/*     MACHINE CONSTANTS FOR THE CRAY 1 */

/*     DATA SMALL(1) / 200004000000000000000B / */
/*     DATA SMALL(2) / 00000000000000000000B / */

/*     DATA LARGE(1) / 577777777777777777777B / */
/*     DATA LARGE(2) / 000007777777777777777B / */

/*     DATA RIGHT(1) / 377214000000000000000B / */
/*     DATA RIGHT(2) / 000000000000000000000B / */

/*     DATA DIVER(1) / 377224000000000000000B / */
/*     DATA DIVER(2) / 000000000000000000000B / */

/*     DATA LOG10(1) / 377774642023241175717B / */
/*     DATA LOG10(2) / 000007571421742254654B / */

/*     MACHINE CONSTANTS FOR THE DATA GENERAL ECLIPSE S/200 */

/*     NOTE - IT MAY BE APPROPRIATE TO INCLUDE THE FOLLOWING CARD - */
/*     STATIC DMACH(5) */

/*     DATA SMALL/20K,3*0/,LARGE/77777K,3*177777K/ */
/*     DATA RIGHT/31420K,3*0/,DIVER/32020K,3*0/ */
/*     DATA LOG10/40423K,42023K,50237K,74776K/ */

/*     MACHINE CONSTANTS FOR THE HARRIS 220 */

/*     DATA SMALL(1),SMALL(2) / '20000000, '00000201 / */
/*     DATA LARGE(1),LARGE(2) / '37777777, '37777577 / */
/*     DATA RIGHT(1),RIGHT(2) / '20000000, '00000333 / */
/*     DATA DIVER(1),DIVER(2) / '20000000, '00000334 / */
/*     DATA LOG10(1),LOG10(2) / '23210115, '10237777 / */

/*     MACHINE CONSTANTS FOR THE HONEYWELL 600/6000 SERIES. */

/*     DATA SMALL(1),SMALL(2) / O402400000000, O000000000000 / */
/*     DATA LARGE(1),LARGE(2) / O376777777777, O777777777777 / */
/*     DATA RIGHT(1),RIGHT(2) / O604400000000, O000000000000 / */
/*     DATA DIVER(1),DIVER(2) / O606400000000, O000000000000 / */
/*     DATA LOG10(1),LOG10(2) / O776464202324, O117571775714 / */

/*     MACHINE CONSTANTS FOR THE IBM 360/370 SERIES, */
/*     THE XEROX SIGMA 5/7/9 AND THE SEL SYSTEMS 85/86. */

/*     DATA SMALL(1),SMALL(2) / Z00100000, Z00000000 / */
/*     DATA LARGE(1),LARGE(2) / Z7FFFFFFF, ZFFFFFFFF / */
/*     DATA RIGHT(1),RIGHT(2) / Z33100000, Z00000000 / */
/*     DATA DIVER(1),DIVER(2) / Z34100000, Z00000000 / */
/*     DATA LOG10(1),LOG10(2) / Z41134413, Z509F79FF / */

/*     MACHINE CONSTANTS FOR IEEE ARITHMETIC MACHINES, SUCH AS IBM PC */

/*     DATA SMALL(1),SMALL(2) / 0 , 1048576 / */
/*     DATA LARGE(1),LARGE(2) / -1 , 2146435071 / */
/*     DATA RIGHT(1),RIGHT(2) / 0 , 1017118720 / */
/*     DATA DIVER(1),DIVER(2) / 0 , 1018167296 / */
/*     DATA LOG10(1),LOG10(2) / 1352628735 , 1070810131 / */

/*     MACHINE CONSTANTS FOR IEEE ARITHMETIC MACHINES, SUCH AS SUN SPARC 
*/


/*     MACHINE CONSTANTS FOR THE PDP-10 (KA PROCESSOR). */

/*     DATA SMALL(1),SMALL(2) / "033400000000, "000000000000 / */
/*     DATA LARGE(1),LARGE(2) / "377777777777, "344777777777 / */
/*     DATA RIGHT(1),RIGHT(2) / "113400000000, "000000000000 / */
/*     DATA DIVER(1),DIVER(2) / "114400000000, "000000000000 / */
/*     DATA LOG10(1),LOG10(2) / "177464202324, "144117571776 / */

/*     MACHINE CONSTANTS FOR THE PDP-10 (KI PROCESSOR). */

/*     DATA SMALL(1),SMALL(2) / "000400000000, "000000000000 / */
/*     DATA LARGE(1),LARGE(2) / "377777777777, "377777777777 / */
/*     DATA RIGHT(1),RIGHT(2) / "103400000000, "000000000000 / */
/*     DATA DIVER(1),DIVER(2) / "104400000000, "000000000000 / */
/*     DATA LOG10(1),LOG10(2) / "177464202324, "476747767461 / */

/*     MACHINE CONSTANTS FOR PDP-11 FORTRAN'S SUPPORTING */
/*     32-BIT INTEGERS (EXPRESSED IN INTEGER AND OCTAL). */

/*     DATA SMALL(1),SMALL(2) /    8388608,           0 / */
/*     DATA LARGE(1),LARGE(2) / 2147483647,          -1 / */
/*     DATA RIGHT(1),RIGHT(2) /  612368384,           0 / */
/*     DATA DIVER(1),DIVER(2) /  620756992,           0 / */
/*     DATA LOG10(1),LOG10(2) / 1067065498, -2063872008 / */

/*     DATA SMALL(1),SMALL(2) / O00040000000, O00000000000 / */
/*     DATA LARGE(1),LARGE(2) / O17777777777, O37777777777 / */
/*     DATA RIGHT(1),RIGHT(2) / O04440000000, O00000000000 / */
/*     DATA DIVER(1),DIVER(2) / O04500000000, O00000000000 / */
/*     DATA LOG10(1),LOG10(2) / O07746420232, O20476747770 / */

/*     MACHINE CONSTANTS FOR PDP-11 FORTRAN'S SUPPORTING */
/*     16-BIT INTEGERS (EXPRESSED IN INTEGER AND OCTAL). */

/*     DATA SMALL(1),SMALL(2) /    128,      0 / */
/*     DATA SMALL(3),SMALL(4) /      0,      0 / */

/*     DATA LARGE(1),LARGE(2) /  32767,     -1 / */
/*     DATA LARGE(3),LARGE(4) /     -1,     -1 / */

/*     DATA RIGHT(1),RIGHT(2) /   9344,      0 / */
/*     DATA RIGHT(3),RIGHT(4) /      0,      0 / */

/*     DATA DIVER(1),DIVER(2) /   9472,      0 / */
/*     DATA DIVER(3),DIVER(4) /      0,      0 / */

/*     DATA LOG10(1),LOG10(2) /  16282,   8346 / */
/*     DATA LOG10(3),LOG10(4) / -31493, -12296 / */

/*     DATA SMALL(1),SMALL(2) / O000200, O000000 / */
/*     DATA SMALL(3),SMALL(4) / O000000, O000000 / */

/*     DATA LARGE(1),LARGE(2) / O077777, O177777 / */
/*     DATA LARGE(3),LARGE(4) / O177777, O177777 / */

/*     DATA RIGHT(1),RIGHT(2) / O022200, O000000 / */
/*     DATA RIGHT(3),RIGHT(4) / O000000, O000000 / */

/*     DATA DIVER(1),DIVER(2) / O022400, O000000 / */
/*     DATA DIVER(3),DIVER(4) / O000000, O000000 / */

/*     DATA LOG10(1),LOG10(2) / O037632, O020232 / */
/*     DATA LOG10(3),LOG10(4) / O102373, O147770 / */

/*     MACHINE CONSTANTS FOR THE UNIVAC 1100 SERIES. */

/*     DATA SMALL(1),SMALL(2) / O000040000000, O000000000000 / */
/*     DATA LARGE(1),LARGE(2) / O377777777777, O777777777777 / */
/*     DATA RIGHT(1),RIGHT(2) / O170540000000, O000000000000 / */
/*     DATA DIVER(1),DIVER(2) / O170640000000, O000000000000 / */
/*     DATA LOG10(1),LOG10(2) / O177746420232, O411757177572 / */



    ret_val = dmach[*i__ - 1];
    return ret_val;
} 

#undef right
#undef diver
#undef small
#undef large
#undef dmach
#undef log10


doublereal sasfit_cheval(integer * n, doublereal * a, doublereal * t)
{
    /* Initialized data */

    static doublereal zero = 0.;
    static doublereal half = .5;
    static doublereal test = .6;
    static doublereal two = 2.;

    /* System generated locals */
    doublereal ret_val;

    /* Local variables */
    static integer i__;
    static doublereal d1, d2, u0, u1, u2, tt;


/*   This function evaluates a Chebyshev series, using the */
/*   Clenshaw method with Reinsch modification, as analysed */
/*   in the paper by Oliver. */

/*   INPUT PARAMETERS */

/*       N - INTEGER - The no. of terms in the sequence */

/*      A - DOUBLE PRECISION ARRAY, dimension 0 to N - The coefficients of
*/
/*           the Chebyshev series */

/*       T - DOUBLE PRECISION - The value at which the series is to be */
/*           evaluated */


/*   REFERENCES */

/*        "An error analysis of the modified Clenshaw method for */
/*         evaluating Chebyshev and Fourier series" J. Oliver, */
/*         J.I.M.A., vol. 20, 1977, pp379-391 */


/* MACHINE-DEPENDENT CONSTANTS: NONE */


/* INTRINSIC FUNCTIONS USED; */

/*    ABS */


/* AUTHOR:  Dr. Allan J. MacLeod, */
/*          Dept. of Mathematics and Statistics, */
/*          University of Paisley , */
/*          High St., */
/*          PAISLEY, */
/*          SCOTLAND */


/* LATEST MODIFICATION:   21 December , 1992 */


    u1 = zero;

/*   If ABS ( T )  < 0.6 use the standard Clenshaw method */

    if (abs(*t) < test) {
	u0 = zero;
	tt = *t + *t;
	for (i__ = *n; i__ >= 0; --i__) {
	    u2 = u1;
	    u1 = u0;
	    u0 = tt * u1 + a[i__] - u2;
/* L100: */
	}
	ret_val = (u0 - u2) / two;
    } else {

/*   If ABS ( T )  > =  0.6 use the Reinsch modification */

	d1 = zero;

/*   T > =  0.6 code */

	if (*t > zero) {
	    tt = *t - half - half;
	    tt += tt;
	    for (i__ = *n; i__ >= 0; --i__) {
		d2 = d1;
		u2 = u1;
		d1 = tt * u2 + a[i__] + d2;
		u1 = d1 + u2;
/* L200: */
	    }
	    ret_val = (d1 + d2) / two;
	} else {

/*   T < =  -0.6 code */

	    tt = *t + half + half;
	    tt += tt;
	    for (i__ = *n; i__ >= 0; --i__) {
		d2 = d1;
		u2 = u1;
		d1 = tt * u2 + a[i__] - d2;
		u1 = d1 - u2;
/* L300: */
	    }
	    ret_val = (d1 - d2) / two;
	}
    }
    return ret_val;
}

