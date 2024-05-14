/*							log1pl.c
 *
 *      Relative error logarithm
 *	Natural logarithm of 1+x, 128-bit long double precision
 *
 *
 *
 * SYNOPSIS:
 *
 * long double x, y, log1pl();
 *
 * y = log1pl( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the base e (2.718...) logarithm of 1+x.
 *
 * The argument 1+x is separated into its exponent and fractional
 * parts.  If the exponent is between -1 and +1, the logarithm
 * of the fraction is approximated by
 *
 *     log(1+x) = x - 0.5 x^2 + x^3 P(x)/Q(x).
 *
 * Otherwise, setting  z = 2(x-1)/x+1),
 * 
 *     log(x) = z + z^3 P(z)/Q(z).
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -1, 8       100000      1.9e-34     4.3e-35
 */

/*
Cephes Math Library Release 2.8:  April, 2001
Copyright 2001 by Stephen L. Moshier
*/

#include "mconf.h"

static char *fname = "log1pl";

/* Coefficients for log(1+x) = x - x^2 / 2 + x^3 P(x)/Q(x)
 * 1/sqrt(2) <= 1+x < sqrt(2)
 * Theoretical peak relative error = 5.3e-37,
 * relative peak error spread = 2.3e-14
 */
static long double P[13] = {
 1.538612243596254322971797716843006400388E-6L,
 4.998469661968096229986658302195402690910E-1L,
 2.321125933898420063925789532045674660756E1L,
 4.114517881637811823002128927449878962058E2L,
 3.824952356185897735160588078446136783779E3L,
 2.128857716871515081352991964243375186031E4L,
 7.594356839258970405033155585486712125861E4L,
 1.797628303815655343403735250238293741397E5L,
 2.854829159639697837788887080758954924001E5L,
 3.007007295140399532324943111654767187848E5L,
 2.014652742082537582487669938141683759923E5L,
 7.771154681358524243729929227226708890930E4L,
 1.313572404063446165910279910527789794488E4L
};
static long double Q[12] = {
/* 1.000000000000000000000000000000000000000E0L, */
 4.839208193348159620282142911143429644326E1L,
 9.104928120962988414618126155557301584078E2L,
 9.147150349299596453976674231612674085381E3L,
 5.605842085972455027590989944010492125825E4L,
 2.248234257620569139969141618556349415120E5L,
 6.132189329546557743179177159925690841200E5L,
 1.158019977462989115839826904108208787040E6L,
 1.514882452993549494932585972882995548426E6L,
 1.347518538384329112529391120390701166528E6L,
 7.777690340007566932935753241556479363645E5L,
 2.626900195321832660448791748036714883242E5L,
 3.940717212190338497730839731583397586124E4L
};

/* Coefficients for log(x) = z + z^3 P(z^2)/Q(z^2),
 * where z = 2(x-1)/(x+1)
 * 1/sqrt(2) <= x < sqrt(2)
 * Theoretical peak relative error = 1.1e-35,
 * relative peak error spread 1.1e-9
 */
static long double R[6] = {
-8.828896441624934385266096344596648080902E-1L,
 8.057002716646055371965756206836056074715E1L,
-2.024301798136027039250415126250455056397E3L,
 2.048819892795278657810231591630928516206E4L,
-8.977257995689735303686582344659576526998E4L,
 1.418134209872192732479751274970992665513E5L
};
static long double S[6] = {
/* 1.000000000000000000000000000000000000000E0L, */
-1.186359407982897997337150403816839480438E2L,
 3.998526750980007367835804959888064681098E3L,
-5.748542087379434595104154610899551484314E4L,
 4.001557694070773974936904547424676279307E5L,
-1.332535117259762928288745111081235577029E6L,
 1.701761051846631278975701529965589676574E6L
};
/* C1 + C2 = ln 2 */
static long double C1 = 6.93145751953125E-1L;
static long double C2 = 1.428606820309417232121458176568075500134E-6L;

static long double SQRTH = 0.7071067811865475244008443621048490392848L;

extern long double MINLOGL;
long double frexpl(), ldexpl(), polevll(), p1evll();


long double
log1pl(long double xm1)
{
long double x, y, z;
int e;

x = xm1 + 1.0L;

/* Test for domain errors.  */
if( x <= 0.0L )
	{
	if( x == 0.0L )
		mtherr( fname, SING );
	else
		mtherr( fname, DOMAIN );
	return( MINLOGL );
	}

/* Separate mantissa from exponent.  */

/* Use frexp used so that denormal numbers will be handled properly.  */
x = frexpl( x, &e );

/* Logarithm using log(x) = z + z^3 P(z^2)/Q(z^2),
   where z = 2(x-1)/x+1).  */
if( (e > 2) || (e < -2) )
{
if( x < SQRTH )
	{ /* 2( 2x-1 )/( 2x+1 ) */
	e -= 1;
	z = x - 0.5L;
	y = 0.5L * z + 0.5L;
	}	
else
	{ /*  2 (x-1)/(x+1)   */
	z = x - 0.5L;
	z -= 0.5L;
	y = 0.5L * x  + 0.5L;
	}
x = z / y;
z = x*x;
z = x * ( z * polevll( z, R, 5 ) / p1evll( z, S, 6 ) );
z = z + e * C2;
z = z + x;
z = z + e * C1;
return( z );
}


/* Logarithm using log(1+x) = x - .5x^2 + x^3 P(x)/Q(x). */

if( x < SQRTH )
	{
	e -= 1;
	if (e != 0)
	  x = 2.0L * x - 1.0L; /*  2x - 1  */
	else
	  x = xm1;
	}	
else
	{
	  if (e != 0)
	    x = x - 1.0L;
	  else
	    x = xm1;
	}
z = x*x;
y = x * ( z * polevll( x, P, 12 ) / p1evll( x, Q, 12 ) );
y = y + e * C2;
z = y - 0.5L * z;
z = z + x;
z = z + e * C1;
return( z );
}
