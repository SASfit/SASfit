/*							cbrtl.c
 *
 *	Cube root, long double precision
 *
 *
 *
 * SYNOPSIS:
 *
 * long double x, y, cbrtl();
 *
 * y = cbrtl( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the cube root of the argument, which may be negative.
 *
 * Range reduction involves determining the power of 2 of
 * the argument.  A polynomial of degree 2 applied to the
 * mantissa, and multiplication by the cube root of 1, 2, or 4
 * approximates the root to within about 0.1%.  Then Newton's
 * iteration is used three times to converge to an accurate
 * result.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE     .125,8        80000      1.2e-34     3.8e-35
 *    IEEE    exp(+-707)    100000      1.3e-34     4.3e-35
 *
 */


/*
Cephes Math Library Release 2.2: January, 1991
Copyright 1984, 1991 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/


#include "mconf.h"

static long double CBRT2  =  1.259921049894873164767210607278228350570251L;
static long double CBRT4  =  1.587401051968199474751705639272308260391493L;
static long double CBRT2I =  0.7937005259840997373758528196361541301957467L;
static long double CBRT4I =  0.6299605249474365823836053036391141752851257L;


long double cbrtl(x)
long double x;
{
int e, rem, sign;
long double z;
long double frexpl(), ldexpl();

if( x == 0 )
	return( 0.0L );
if( x > 0 )
	sign = 1;
else
	{
	sign = -1;
	x = -x;
	}

z = x;
/* extract power of 2, leaving
 * mantissa between 0.5 and 1
 */
x = frexpl( x, &e );

/* Approximate cube root of number between .5 and 1,
 * peak relative error = 1.2e-6
 */
x = (((( 1.3584464340920900529734e-1L * x
       - 6.3986917220457538402318e-1L) * x
       + 1.2875551670318751538055e0L) * x
       - 1.4897083391357284957891e0L) * x
       + 1.3304961236013647092521e0L) * x
       + 3.7568280825958912391243e-1L;

/* exponent divided by 3 */
if( e >= 0 )
	{
	rem = e;
	e /= 3;
	rem -= 3*e;
	if( rem == 1 )
		x *= CBRT2;
	else if( rem == 2 )
		x *= CBRT4;
	}
else
	{ /* argument less than 1 */
	e = -e;
	rem = e;
	e /= 3;
	rem -= 3*e;
	if( rem == 1 )
		x *= CBRT2I;
	else if( rem == 2 )
		x *= CBRT4I;
	e = -e;
	}

/* multiply by power of 2 */
x = ldexpl( x, e );

/* Newton iteration */

x -= ( x - (z/(x*x)) )*0.3333333333333333333333333333333333333333L;
x -= ( x - (z/(x*x)) )*0.3333333333333333333333333333333333333333L;
x -= ( x - (z/(x*x)) )*0.3333333333333333333333333333333333333333L;

if( sign < 0 )
	x = -x;
return(x);
}
