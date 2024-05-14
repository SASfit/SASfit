/*							gammal.c
 *
 *	Gamma function
 *
 *
 *
 * SYNOPSIS:
 *
 * long double x, y, gammal();
 * extern int sgngam;
 *
 * y = gammal( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns gamma function of the argument.  The result is
 * correctly signed, and the sign (+1 or -1) is also
 * returned in a global (extern) variable named sgngam.
 * This variable is also filled in by the logarithmic gamma
 * function lgam().
 *
 * Arguments |x| <= 13 are reduced by recurrence and the function
 * approximated by a rational function of degree 7/8 in the
 * interval (2,3).  Large arguments are handled by Stirling's
 * formula. Large negative arguments are made positive using
 * a reflection formula.  
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE     -40,+40      10000       3.6e-19     7.9e-20
 *    IEEE    -1755,+1755   10000       4.8e-18     6.5e-19
 *
 * Accuracy for large arguments is dominated by error in powl().
 *
 */
/*							lgaml()
 *
 *	Natural logarithm of gamma function
 *
 *
 *
 * SYNOPSIS:
 *
 * long double x, y, lgaml();
 * extern int sgngam;
 *
 * y = lgaml( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the base e (2.718...) logarithm of the absolute
 * value of the gamma function of the argument.
 * The sign (+1 or -1) of the gamma function is returned in a
 * global (extern) variable named sgngam.
 *
 * For arguments greater than 33, the logarithm of the gamma
 * function is approximated by the logarithmic version of
 * Stirling's formula using a polynomial approximation of
 * degree 4. Arguments between -33 and +33 are reduced by
 * recurrence to the interval [2,3] of a rational approximation.
 * The cosecant reflection formula is employed for arguments
 * less than -33.
 *
 * Arguments greater than MAXLGML (10^4928) return MAXNUML.
 *
 *
 *
 * ACCURACY:
 *
 *
 * arithmetic      domain        # trials     peak         rms
 *    IEEE         -40, 40        100000     2.2e-19     4.6e-20
 *    IEEE    10^-2000,10^+2000    20000     1.6e-19     3.3e-20
 * The error criterion was relative when the function magnitude
 * was greater than one but absolute when it was less than one.
 *
 */

/*							gamma.c	*/
/*	gamma function	*/

/*
Copyright 1994 by Stephen L. Moshier
*/


#include "mconf.h"
/*
gamma(x+2)  = gamma(x+2) P(x)/Q(x)
0 <= x <= 1
Relative error
n=7, d=8
Peak error =  1.83e-20
Relative error spread =  8.4e-23
*/
#if UNK
static long double P[8] = {
 4.212760487471622013093E-5L,
 4.542931960608009155600E-4L,
 4.092666828394035500949E-3L,
 2.385363243461108252554E-2L,
 1.113062816019361559013E-1L,
 3.629515436640239168939E-1L,
 8.378004301573126728826E-1L,
 1.000000000000000000009E0L,
};
static long double Q[9] = {
-1.397148517476170440917E-5L,
 2.346584059160635244282E-4L,
-1.237799246653152231188E-3L,
-7.955933682494738320586E-4L,
 2.773706565840072979165E-2L,
-4.633887671244534213831E-2L,
-2.243510905670329164562E-1L,
 4.150160950588455434583E-1L,
 9.999999999999999999908E-1L,
};
#endif
#if IBMPC
static short P[] = {
0x434a,0x3f22,0x2bda,0xb0b2,0x3ff0, XPD
0xf5aa,0xe82f,0x335b,0xee2e,0x3ff3, XPD
0xbe6c,0x3757,0xc717,0x861b,0x3ff7, XPD
0x7f43,0x5196,0xb166,0xc368,0x3ff9, XPD
0x9549,0x8eb5,0x8c3a,0xe3f4,0x3ffb, XPD
0x8d75,0x23af,0xc8e4,0xb9d4,0x3ffd, XPD
0x29cf,0x19b3,0x16c8,0xd67a,0x3ffe, XPD
0x0000,0x0000,0x0000,0x8000,0x3fff, XPD
};
static short Q[] = {
0x5473,0x2de8,0x1268,0xea67,0xbfee, XPD
0x334b,0xc2f0,0xa2dd,0xf60e,0x3ff2, XPD
0xbeed,0x1853,0xa691,0xa23d,0xbff5, XPD
0x296e,0x7cb1,0x5dfd,0xd08f,0xbff4, XPD
0x0417,0x7989,0xd7bc,0xe338,0x3ff9, XPD
0x3295,0x3698,0xd580,0xbdcd,0xbffa, XPD
0x75ef,0x3ab7,0x4ad3,0xe5bc,0xbffc, XPD
0xe458,0x2ec7,0xfd57,0xd47c,0x3ffd, XPD
0x0000,0x0000,0x0000,0x8000,0x3fff, XPD
};
#endif
#if MIEEE
static long P[24] = {
0x3ff00000,0xb0b22bda,0x3f22434a,
0x3ff30000,0xee2e335b,0xe82ff5aa,
0x3ff70000,0x861bc717,0x3757be6c,
0x3ff90000,0xc368b166,0x51967f43,
0x3ffb0000,0xe3f48c3a,0x8eb59549,
0x3ffd0000,0xb9d4c8e4,0x23af8d75,
0x3ffe0000,0xd67a16c8,0x19b329cf,
0x3fff0000,0x80000000,0x00000000,
};
static long Q[27] = {
0xbfee0000,0xea671268,0x2de85473,
0x3ff20000,0xf60ea2dd,0xc2f0334b,
0xbff50000,0xa23da691,0x1853beed,
0xbff40000,0xd08f5dfd,0x7cb1296e,
0x3ff90000,0xe338d7bc,0x79890417,
0xbffa0000,0xbdcdd580,0x36983295,
0xbffc0000,0xe5bc4ad3,0x3ab775ef,
0x3ffd0000,0xd47cfd57,0x2ec7e458,
0x3fff0000,0x80000000,0x00000000,
};
#endif
/*
static long double P[] = {
-3.01525602666895735709e0L,
-3.25157411956062339893e1L,
-2.92929976820724030353e2L,
-1.70730828800510297666e3L,
-7.96667499622741999770e3L,
-2.59780216007146401957e4L,
-5.99650230220855581642e4L,
-7.15743521530849602425e4L
};
static long double Q[] = {
 1.00000000000000000000e0L,
-1.67955233807178858919e1L,
 8.85946791747759881659e1L,
 5.69440799097468430177e1L,
-1.98526250512761318471e3L,
 3.31667508019495079814e3L,
 1.60577839621734713377e4L,
-2.97045081369399940529e4L,
-7.15743521530849602412e4L
};
*/
#define MAXGAML 1755.455L
/*static long double LOGPI = 1.14472988584940017414L;*/

/* Stirling's formula for the gamma function
gamma(x) = sqrt(2 pi) x^(x-.5) exp(-x) (1 + 1/x P(1/x))
z(x) = x
13 <= x <= 1024
Relative error
n=8, d=0
Peak error =  9.44e-21
Relative error spread =  8.8e-4
*/
#if UNK
static long double STIR[9] = {
 7.147391378143610789273E-4L,
-2.363848809501759061727E-5L,
-5.950237554056330156018E-4L,
 6.989332260623193171870E-5L,
 7.840334842744753003862E-4L,
-2.294719747873185405699E-4L,
-2.681327161876304418288E-3L,
 3.472222222230075327854E-3L,
 8.333333333333331800504E-2L,
};
#endif
#if IBMPC
static short STIR[] = {
0x6ede,0x69f7,0x54e3,0xbb5d,0x3ff4, XPD
0xc395,0x0295,0x4443,0xc64b,0xbfef, XPD
0xba6f,0x7c59,0x5e47,0x9bfb,0xbff4, XPD
0x5704,0x1a39,0xb11d,0x9293,0x3ff1, XPD
0x30b7,0x1a21,0x98b2,0xcd87,0x3ff4, XPD
0xbef3,0x7023,0x6a08,0xf09e,0xbff2, XPD
0x3a1c,0x5ac8,0x3478,0xafb9,0xbff6, XPD
0xc3c9,0x906e,0x38e3,0xe38e,0x3ff6, XPD
0xa1d5,0xaaaa,0xaaaa,0xaaaa,0x3ffb, XPD
};
#endif
#if MIEEE
static long STIR[27] = {
0x3ff40000,0xbb5d54e3,0x69f76ede,
0xbfef0000,0xc64b4443,0x0295c395,
0xbff40000,0x9bfb5e47,0x7c59ba6f,
0x3ff10000,0x9293b11d,0x1a395704,
0x3ff40000,0xcd8798b2,0x1a2130b7,
0xbff20000,0xf09e6a08,0x7023bef3,
0xbff60000,0xafb93478,0x5ac83a1c,
0x3ff60000,0xe38e38e3,0x906ec3c9,
0x3ffb0000,0xaaaaaaaa,0xaaaaa1d5,
};
#endif
#define MAXSTIR 1024.0L
static long double SQTPI = 2.50662827463100050242E0L;

/* 1/gamma(x) = z P(z)
 * z(x) = 1/x
 * 0 < x < 0.03125
 * Peak relative error 4.2e-23
 */
#if UNK
static long double S[9] = {
-1.193945051381510095614E-3L,
 7.220599478036909672331E-3L,
-9.622023360406271645744E-3L,
-4.219773360705915470089E-2L,
 1.665386113720805206758E-1L,
-4.200263503403344054473E-2L,
-6.558780715202540684668E-1L,
 5.772156649015328608253E-1L,
 1.000000000000000000000E0L,
};
#endif
#if IBMPC
static short S[] = {
0xbaeb,0xd6d3,0x25e5,0x9c7e,0xbff5, XPD
0xfe9a,0xceb4,0xc74e,0xec9a,0x3ff7, XPD
0x9225,0xdfef,0xb0e9,0x9da5,0xbff8, XPD
0x10b0,0xec17,0x87dc,0xacd7,0xbffa, XPD
0x6b8d,0x7515,0x1905,0xaa89,0x3ffc, XPD
0xf183,0x126b,0xf47d,0xac0a,0xbffa, XPD
0x7bf6,0x57d1,0xa013,0xa7e7,0xbffe, XPD
0xc7a9,0x7db0,0x67e3,0x93c4,0x3ffe, XPD
0x0000,0x0000,0x0000,0x8000,0x3fff, XPD
};
#endif
#if MIEEE
static long S[27] = {
0xbff50000,0x9c7e25e5,0xd6d3baeb,
0x3ff70000,0xec9ac74e,0xceb4fe9a,
0xbff80000,0x9da5b0e9,0xdfef9225,
0xbffa0000,0xacd787dc,0xec1710b0,
0x3ffc0000,0xaa891905,0x75156b8d,
0xbffa0000,0xac0af47d,0x126bf183,
0xbffe0000,0xa7e7a013,0x57d17bf6,
0x3ffe0000,0x93c467e3,0x7db0c7a9,
0x3fff0000,0x80000000,0x00000000,
};
#endif
/* 1/gamma(-x) = z P(z)
 * z(x) = 1/x
 * 0 < x < 0.03125
 * Peak relative error 5.16e-23
 * Relative error spread =  2.5e-24
 */
#if UNK
static long double SN[9] = {
 1.133374167243894382010E-3L,
 7.220837261893170325704E-3L,
 9.621911155035976733706E-3L,
-4.219773343731191721664E-2L,
-1.665386113944413519335E-1L,
-4.200263503402112910504E-2L,
 6.558780715202536547116E-1L,
 5.772156649015328608727E-1L,
-1.000000000000000000000E0L,
};
#endif
#if IBMPC
static short SN[] = {
0x5dd1,0x02de,0xb9f7,0x948d,0x3ff5, XPD
0x989b,0xdd68,0xc5f1,0xec9c,0x3ff7, XPD
0x2ca1,0x18f0,0x386f,0x9da5,0x3ff8, XPD
0x783f,0x41dd,0x87d1,0xacd7,0xbffa, XPD
0x7a5b,0xd76d,0x1905,0xaa89,0xbffc, XPD
0x7f64,0x1234,0xf47d,0xac0a,0xbffa, XPD
0x5e26,0x57d1,0xa013,0xa7e7,0x3ffe, XPD
0xc7aa,0x7db0,0x67e3,0x93c4,0x3ffe, XPD
0x0000,0x0000,0x0000,0x8000,0xbfff, XPD
};
#endif
#if MIEEE
static long SN[27] = {
0x3ff50000,0x948db9f7,0x02de5dd1,
0x3ff70000,0xec9cc5f1,0xdd68989b,
0x3ff80000,0x9da5386f,0x18f02ca1,
0xbffa0000,0xacd787d1,0x41dd783f,
0xbffc0000,0xaa891905,0xd76d7a5b,
0xbffa0000,0xac0af47d,0x12347f64,
0x3ffe0000,0xa7e7a013,0x57d15e26,
0x3ffe0000,0x93c467e3,0x7db0c7aa,
0xbfff0000,0x80000000,0x00000000,
};
#endif

int sgngaml = 0;
extern int sgngaml;
extern long double MAXLOGL, MAXNUML, PIL;
/* #define PIL 3.14159265358979323846L */
/* #define MAXNUML 1.189731495357231765021263853E4932L */

#ifdef ANSIPROT
extern long double fabsl ( long double );
extern long double lgaml ( long double );
extern long double logl ( long double );
extern long double expl ( long double );
extern long double gammal ( long double );
extern long double sinl ( long double );
extern long double floorl ( long double );
extern long double powl ( long double, long double );
extern long double polevll ( long double, void *, int );
extern long double p1evll ( long double, void *, int );
extern int isnanl ( long double );
extern int isfinitel ( long double );
static long double stirf ( long double );
#else
long double fabsl(), lgaml(), logl(), expl(), gammal(), sinl();
long double floorl(), powl(), polevll(), p1evll(), isnanl(), isfinitel();
static long double stirf();
#endif
#ifdef INFINITIES
extern long double INFINITYL;
#endif
#ifdef NANS
extern long double NANL;
#endif

/* Gamma function computed by Stirling's formula.
 */
static long double stirf(x)
long double x;
{
long double y, w, v;

w = 1.0L/x;
/* For large x, use rational coefficients from the analytical expansion.  */
if( x > 1024.0L )
	w = (((((6.97281375836585777429E-5L * w
		+ 7.84039221720066627474E-4L) * w
		- 2.29472093621399176955E-4L) * w
		- 2.68132716049382716049E-3L) * w
		+ 3.47222222222222222222E-3L) * w
		+ 8.33333333333333333333E-2L) * w
		+ 1.0L;
else
	w = 1.0L + w * polevll( w, STIR, 8 );
y = expl(x);
if( x > MAXSTIR )
	{ /* Avoid overflow in pow() */
	v = powl( x, 0.5L * x - 0.25L );
	y = v * (v / y);
	}
else
	{
	y = powl( x, x - 0.5L ) / y;
	}
y = SQTPI * y * w;
return( y );
}



long double gammal(x)
long double x;
{
long double p, q, z;
int i;

sgngaml = 1;
#ifdef NANS
if( isnanl(x) )
	return(NANL);
#endif
#ifdef INFINITIES
if(x == INFINITYL)
	return(INFINITYL);
#ifdef NANS
if(x == -INFINITYL)
	goto gamnan;
#endif
#endif
q = fabsl(x);

if( q > 13.0L )
	{
	if( q > MAXGAML )
		goto goverf;
	if( x < 0.0L )
		{
		p = floorl(q);
		if( p == q )
			{
gamnan:
#ifdef NANS
			mtherr( "gammal", DOMAIN );
			return (NANL);
#else
			goto goverf;
#endif
			}
		i = p;
		if( (i & 1) == 0 )
			sgngaml = -1;
		z = q - p;
		if( z > 0.5L )
			{
			p += 1.0L;
			z = q - p;
			}
		z = q * sinl( PIL * z );
		z = fabsl(z) * stirf(q);
		if( z <= PIL/MAXNUML )
			{
goverf:
#ifdef INFINITIES
			return( sgngaml * INFINITYL);
#else
			mtherr( "gammal", OVERFLOW );
			return( sgngaml * MAXNUML);
#endif
			}
		z = PIL/z;
		}
	else
		{
		z = stirf(x);
		}
	return( sgngaml * z );
	}

z = 1.0L;
while( x >= 3.0L )
	{
	x -= 1.0L;
	z *= x;
	}

while( x < -0.03125L )
	{
	z /= x;
	x += 1.0L;
	}

if( x <= 0.03125L )
	goto small;

while( x < 2.0L )
	{
	z /= x;
	x += 1.0L;
	}

if( x == 2.0L )
	return(z);

x -= 2.0L;
p = polevll( x, P, 7 );
q = polevll( x, Q, 8 );
z = z * p / q;
if( z < 0 )
	sgngaml = -1;
return z;

small:
if( x == 0.0L )
	{
	  goto gamnan;
	}
else
	{
	if( x < 0.0L )
		{
		x = -x;
		q = z / (x * polevll( x, SN, 8 ));
		sgngaml = -1;
		}
	else
		q = z / (x * polevll( x, S, 8 ));
	}
return q;
}



/* A[]: Stirling's formula expansion of log gamma
 * B[], C[]: log gamma function between 2 and 3
 */


/* log gamma(x) = ( x - 0.5 ) * log(x) - x + LS2PI + 1/x A(1/x^2)
 * x >= 8
 * Peak relative error 1.51e-21
 * Relative spread of error peaks 5.67e-21
 */
#if UNK
static long double A[7] = {
 4.885026142432270781165E-3L,
-1.880801938119376907179E-3L,
 8.412723297322498080632E-4L,
-5.952345851765688514613E-4L,
 7.936507795855070755671E-4L,
-2.777777777750349603440E-3L,
 8.333333333333331447505E-2L,
};
#endif
#if IBMPC
static short A[] = {
0xd984,0xcc08,0x91c2,0xa012,0x3ff7, XPD
0x3d91,0x0304,0x3da1,0xf685,0xbff5, XPD
0x3bdc,0xaad1,0xd492,0xdc88,0x3ff4, XPD
0x8b20,0x9fce,0x844e,0x9c09,0xbff4, XPD
0xf8f2,0x30e5,0x0092,0xd00d,0x3ff4, XPD
0x4d88,0x03a8,0x60b6,0xb60b,0xbff6, XPD
0x9fcc,0xaaaa,0xaaaa,0xaaaa,0x3ffb, XPD
};
#endif
#if MIEEE
static long A[21] = {
0x3ff70000,0xa01291c2,0xcc08d984,
0xbff50000,0xf6853da1,0x03043d91,
0x3ff40000,0xdc88d492,0xaad13bdc,
0xbff40000,0x9c09844e,0x9fce8b20,
0x3ff40000,0xd00d0092,0x30e5f8f2,
0xbff60000,0xb60b60b6,0x03a84d88,
0x3ffb0000,0xaaaaaaaa,0xaaaa9fcc,
};
#endif

/* log gamma(x+2) = x B(x)/C(x)
 * 0 <= x <= 1
 * Peak relative error 7.16e-22
 * Relative spread of error peaks 4.78e-20
 */
#if UNK
static long double B[7] = {
-2.163690827643812857640E3L,
-8.723871522843511459790E4L,
-1.104326814691464261197E6L,
-6.111225012005214299996E6L,
-1.625568062543700591014E7L,
-2.003937418103815175475E7L,
-8.875666783650703802159E6L,
};
static long double C[7] = {
/* 1.000000000000000000000E0L,*/
-5.139481484435370143617E2L,
-3.403570840534304670537E4L,
-6.227441164066219501697E5L,
-4.814940379411882186630E6L,
-1.785433287045078156959E7L,
-3.138646407656182662088E7L,
-2.099336717757895876142E7L,
};
#endif
#if IBMPC
static short B[] = {
0x9557,0x4995,0x0da1,0x873b,0xc00a, XPD
0xfe44,0x9af8,0x5b8c,0xaa63,0xc00f, XPD
0x5aa8,0x7cf5,0x3684,0x86ce,0xc013, XPD
0x259a,0x258c,0xf206,0xba7f,0xc015, XPD
0xbe18,0x1ca3,0xc0a0,0xf80a,0xc016, XPD
0x168f,0x2c42,0x6717,0x98e3,0xc017, XPD
0x2051,0x9d55,0x92c8,0x876e,0xc016, XPD
};
static short C[] = {
/*0x0000,0x0000,0x0000,0x8000,0x3fff, XPD*/
0xaa77,0xcf2f,0xae76,0x807c,0xc008, XPD
0xb280,0x0d74,0xb55a,0x84f3,0xc00e, XPD
0xa505,0xcd30,0x81dc,0x9809,0xc012, XPD
0x3369,0x4246,0xb8c2,0x92f0,0xc015, XPD
0x63cf,0x6aee,0xbe6f,0x8837,0xc017, XPD
0x26bb,0xccc7,0xb009,0xef75,0xc017, XPD
0x462b,0xbae8,0xab96,0xa02a,0xc017, XPD
};
#endif
#if MIEEE
static long B[21] = {
0xc00a0000,0x873b0da1,0x49959557,
0xc00f0000,0xaa635b8c,0x9af8fe44,
0xc0130000,0x86ce3684,0x7cf55aa8,
0xc0150000,0xba7ff206,0x258c259a,
0xc0160000,0xf80ac0a0,0x1ca3be18,
0xc0170000,0x98e36717,0x2c42168f,
0xc0160000,0x876e92c8,0x9d552051,
};
static long C[21] = {
/*0x3fff0000,0x80000000,0x00000000,*/
0xc0080000,0x807cae76,0xcf2faa77,
0xc00e0000,0x84f3b55a,0x0d74b280,
0xc0120000,0x980981dc,0xcd30a505,
0xc0150000,0x92f0b8c2,0x42463369,
0xc0170000,0x8837be6f,0x6aee63cf,
0xc0170000,0xef75b009,0xccc726bb,
0xc0170000,0xa02aab96,0xbae8462b,
};
#endif

/* log( sqrt( 2*pi ) ) */
static long double LS2PI  =  0.91893853320467274178L;
#define MAXLGM 1.04848146839019521116e+4928L


/* Logarithm of gamma function */


long double lgaml(x)
long double x;
{
long double p, q, w, z, f, nx;
int i;

sgngaml = 1;
#ifdef NANS
if( isnanl(x) )
	return(NANL);
#endif
#ifdef INFINITIES
if( !isfinitel(x) )
	return(INFINITYL);
#endif
if( x < -34.0L )
	{
	q = -x;
	w = lgaml(q); /* note this modifies sgngam! */
	p = floorl(q);
	if( p == q )
		{
#ifdef INFINITIES
		mtherr( "lgaml", SING );
		return (INFINITYL);
#else
		goto loverf;
#endif
		}
	i = p;
	if( (i & 1) == 0 )
		sgngaml = -1;
	else
		sgngaml = 1;
	z = q - p;
	if( z > 0.5L )
		{
		p += 1.0L;
		z = p - q;
		}
	z = q * sinl( PIL * z );
	if( z == 0.0L )
		goto loverf;
/*	z = LOGPI - logl( z ) - w; */
	z = logl( PIL/z ) - w;
	return( z );
	}

if( x < 13.0L )
	{
	z = 1.0L;
	nx = floorl( x +  0.5L );
	f = x - nx;
	while( x >= 3.0L )
		{
		nx -= 1.0L;
		x = nx + f;
		z *= x;
		}
	while( x < 2.0L )
		{
		if( fabsl(x) <= 0.03125 )
			goto lsmall;
		z /= nx +  f;
		nx += 1.0L;
		x = nx + f;
		}
	if( z < 0.0L )
		{
		sgngaml = -1;
		z = -z;
		}
	else
		sgngaml = 1;
	if( x == 2.0L )
		return( logl(z) );
	x = (nx - 2.0L) + f;
	p = x * polevll( x, B, 6 ) / p1evll( x, C, 7);
	return( logl(z) + p );
	}

if( x > MAXLGM )
	{
loverf:
#ifdef INFINITIES
	return( sgngaml * INFINITYL );
#else
	mtherr( "lgaml", OVERFLOW );
	return( sgngaml * MAXNUML );
#endif
	}

q = ( x - 0.5L ) * logl(x) - x + LS2PI;
if( x > 1.0e10L )
	return(q);
p = 1.0L/(x*x);
q += polevll( p, A, 6 ) / x;
return( q );


lsmall:
if( x == 0.0L )
	goto loverf;
if( x < 0.0L )
	{
	x = -x;
	q = z / (x * polevll( x, SN, 8 ));
	}
else
	q = z / (x * polevll( x, S, 8 ));
if( q < 0.0L )
	{
	sgngaml = -1;
	q = -q;
	}
else
	sgngaml = 1;
q = logl( q );
return(q);
}
