#include <stdlib.h>
#include <math.h>
#include <float.h>

static double sr, si, tr, ti, pvr, pvi, are, mre, eta, infin;
static int nn;
static double *pr, *piw, *hr, *hi, *qpr, *qpi, *qhr, *qhi, *shr, *shi;

static void noshft( const int l1 );
static void fxshft( const int l2, double *zr, double *zi, int *conv );
static void vrshft( const int l3, double *zr, double *zi, int *conv );
static void calct( int *bol );
static void nexth( const int bol );
static void polyev( const int nn, const double sr, const double si, const double pr[], const double piw[], double qr[], double qi[], double *pvr, double *pvi );
static double errev( const int nn, const double qr[], const double qi[], const double ms, const double mp, const double are, const double mre );
static void cauchy( const int nn, double pt[], double q[], double *fn_val );
static double scale( const int nn, const double pt[], const double eta, const double infin, const double smalno, const double base );
static void cdivid( const double ar, const double ai, const double br, const double bi, double *cr, double *ci );
static double cmod( const double r, const double i );
static void mcon( double *eta, double *infiny, double *smalno, double *base );

int cpoly( const double *opr, const double *opi, int degree, double *zeror, double *zeroi )
{
	int cnt1, cnt2, idnn2, i, conv;
	double xx, yy, cosr, sinr, smalno, base, xxx, zr, zi, bnd;

	mcon( &eta, &infin, &smalno, &base );
	are = eta;
	mre = 2.0 * sqrt( 2.0 ) * eta;
	xx = 0.70710678;
	yy = -xx;
	cosr = -0.060756474;
	sinr = -0.99756405;
	nn = degree;

	if( opr[ 0 ] == 0 && opi[ 0 ] == 0 )
		return -1;

	pr = malloc( sizeof( double ) * ( degree + 1 ) );
	piw = malloc( sizeof( double ) * ( degree + 1 ) );
	hr =malloc( sizeof( double ) * ( degree + 1 ) );
	hi = malloc( sizeof( double ) * ( degree + 1 ) );
	qpr= malloc( sizeof( double ) * ( degree + 1 ) );
	qpi= malloc( sizeof( double ) * ( degree + 1 ) );
	qhr= malloc( sizeof( double ) * ( degree + 1 ) );
	qhi= malloc( sizeof( double ) * ( degree + 1 ) );
	shr= malloc( sizeof( double ) * ( degree + 1 ) );
	shi= malloc( sizeof( double ) * ( degree + 1 ) );

	while( opr[ nn ] == 0 && opi[ nn ] == 0 )
	{
		idnn2 = degree - nn;
		zeror[ idnn2 ] = 0;
		zeroi[ idnn2 ] = 0;
		nn--;
	}

	for( i = 0; i <= nn; i++ )
	{
		pr[ i ] = opr[ i ];
		piw[ i ] = opi[ i ];
		shr[ i ] = cmod( pr[ i ], piw[ i ] );
	}

	bnd = scale( nn, shr, eta, infin, smalno, base );
	if( bnd != 1 )
		for( i = 0; i <= nn; i++ )
		{
			pr[ i ] *= bnd;
			piw[ i ] *= bnd;
		}

search:
	if( nn <= 1 )
	{
		cdivid( -pr[ 1 ], -piw[ 1 ], pr[ 0 ], piw[ 0 ], &zeror[ degree-1 ], &zeroi[ degree-1 ] );
		goto finish;
	}

	for( i = 0; i<= nn; i++ )
		shr[ i ] = cmod( pr[ i ], piw[ i ] );

	cauchy( nn, shr, shi, &bnd );

	for( cnt1 = 1; cnt1 <= 2; cnt1++ )
	{
		noshft( 5 );

		for( cnt2 = 1; cnt2 <= 9; cnt2++ )
		{
			xxx = cosr * xx - sinr * yy;
			yy = sinr * xx + cosr * yy;
			xx = xxx;
			sr = bnd * xx;
			si = bnd * yy;

			fxshft( 10 * cnt2, &zr, &zi, &conv );
			if( conv )
            {
				idnn2 = degree - nn;
				zeror[ idnn2 ] = zr;
				zeroi[ idnn2 ] = zi;
				nn--;
				for( i = 0; i <= nn; i++ )
				{
					pr[ i ] = qpr[ i ];
					piw[ i ] = qpi[ i ];
				}
				goto search;
            }
		}
	}

	degree -= nn;

finish:
	free( pr );
	free( piw );
	free( hr );
	free( hi );
	free( qpr );
	free( qpi );
	free( qhr );
	free( qhi );
	free( shr );
	free( shi );

	return degree;
}

static void noshft( const int l1 )
{
	int i, j, jj, n, nm1;
	double xni, t1, t2;

	n = nn;
	nm1 = n - 1;
	for( i = 0; i < n; i++ )
	{
		xni = nn - i;
		hr[ i ] = xni * pr[ i ] / n;
		hi[ i ] = xni * piw[ i ] / n;
	}
	for( jj = 1; jj <= l1; jj++ )
	{
		if( cmod( hr[ n - 1 ], hi[ n - 1 ] ) > eta * 10 * cmod( pr[ n - 1 ], piw[ n - 1 ] ) )
		{
			cdivid( -pr[ nn ], -piw[ nn ], hr[ n - 1 ], hi[ n - 1 ], &tr, &ti );
			for( i = 0; i < nm1; i++ )
            {
				j = nn - i - 1;
				t1 = hr[ j - 1 ];
				t2 = hi[ j - 1 ];
				hr[ j ] = tr * t1 - ti * t2 + pr[ j ];
				hi[ j ] = tr * t2 + ti * t1 + piw[ j ];
            }
			hr[ 0 ] = pr[ 0 ];
			hi[ 0 ] = piw[ 0 ];
		}
		else
		{
			for( i = 0; i < nm1; i++ )
            {
				j = nn - i - 1;
				hr[ j ] = hr[ j - 1 ];
				hi[ j ] = hi[ j - 1 ];
            }
			hr[ 0 ] = 0;
			hi[ 0 ] = 0;
		}
	}
}

static void fxshft( const int l2, double *zr, double *zi, int *conv )
{
	int i, j, n;
	int test, pasd, bol;
	double otr, oti, svsr, svsi;

	n = nn;
	polyev( nn, sr, si, pr, piw, qpr, qpi, &pvr, &pvi );
	test = 1;
	pasd = 0;

	calct( &bol );

	for( j = 1; j <= l2; j++ )
	{
		otr = tr;
		oti = ti;

		nexth( bol );
		calct( &bol );
		*zr = sr + tr;
		*zi = si + ti;

		if( !( bol || !test || j == 12 ) )
			if( cmod( tr - otr, ti - oti ) < 0.5 * cmod( *zr, *zi ) )
            {
				if( pasd )
				{
					for( i = 0; i < n; i++ )
					{
						shr[ i ] = hr[ i ];
						shi[ i ] = hi[ i ];
					}
					svsr = sr;
					svsi = si;
					vrshft( 10, zr, zi, conv );
					if( *conv ) return;

					test = 0;
					for( i = 0; i < n; i++ )
					{
						hr[ i ] = shr[ i ];
						hi[ i ] = shi[ i ];
					}
					sr = svsr;
					si = svsi;
					polyev( nn, sr, si, pr, piw, qpr, qpi, &pvr, &pvi );
					calct( &bol );
					continue;
				}
				pasd = 1;
            }
			else
				pasd = 0;
	}

	vrshft( 10, zr, zi, conv );
}

static void vrshft( const int l3, double *zr, double *zi, int *conv )
{
	int b, bol;
	int i, j;
	double mp, ms, omp, relstp, r1, r2, tp;

	*conv = 0;
	b = 0;
	sr = *zr;
	si = *zi;

	for( i = 1; i <= l3; i++ )
	{
		polyev( nn, sr, si, pr, piw, qpr, qpi, &pvr, &pvi );
		mp = cmod( pvr, pvi );
		ms = cmod( sr, si );
		if( mp <= 20 * errev( nn, qpr, qpi, ms, mp, are, mre ) )
		{
			*conv = 1;
			*zr = sr;
			*zi = si;
			return;
		}
		if( i != 1 )
		{
			if( !( b || mp < omp || relstp >= 0.05 ) )
            {
				tp = relstp;
				b = 1;
				if( relstp < eta ) tp = eta;
				r1 = sqrt( tp );
				r2 = sr * ( 1 + r1 ) - si * r1;
				si = sr * r1 + si * ( 1 + r1 );
				sr = r2;
				polyev( nn, sr, si, pr, piw, qpr, qpi, &pvr, &pvi );
				for( j = 1; j <= 5; j++ )
				{
					calct( &bol );
					nexth( bol );
				}
				omp = infin;
				goto _20;
            }

			if( mp *0.1 > omp ) return;
		}

		omp = mp;

	_20:  calct( &bol );
		nexth( bol );
		calct( &bol );
		if( !bol )
		{
			relstp = cmod( tr, ti ) / cmod( sr, si );
			sr += tr;
			si += ti;
		}
	}
}

static void calct( int *bol )
{
	int n;
	double hvr, hvi;

	n = nn;

	polyev( n - 1, sr, si, hr, hi, qhr, qhi, &hvr, &hvi );
	*bol = cmod( hvr, hvi ) <= are * 10 * cmod( hr[ n - 1 ], hi[ n - 1 ] ) ? 1 : 0;
	if( !*bol )
	{
		cdivid( -pvr, -pvi, hvr, hvi, &tr, &ti );
		return;
	}

	tr = 0;
	ti = 0;
}

static void nexth( const int bol )
{
	int j, n;
	double t1, t2;

	n = nn;
	if( !bol )
	{
		for( j = 1; j < n; j++ )
		{
			t1 = qhr[ j - 1 ];
			t2 = qhi[ j - 1 ];
			hr[ j ] = tr * t1 - ti * t2 + qpr[ j ];
			hi[ j ] = tr * t2 + ti * t1 + qpi[ j ];
		}
		hr[ 0 ] = qpr[ 0 ];
		hi[ 0 ] = qpi[ 0 ];
		return;
	}

	for( j = 1; j < n; j++ )
	{
		hr[ j ] = qhr[ j - 1 ];
		hi[ j ] = qhi[ j - 1 ];
	}
	hr[ 0 ] = 0;
	hi[ 0 ] = 0;
}

static void polyev( const int nn, const double sr, const double si, const double pr[], const double piw[], double qr[], double qi[], double *pvr, double *pvi )
{
	int i;
	double t;

	qr[ 0 ] = pr[ 0 ];
	qi[ 0 ] = piw[ 0 ];
	*pvr = qr[ 0 ];
	*pvi = qi[ 0 ];

	for( i = 1; i <= nn; i++ )
	{
		t = ( *pvr ) * sr - ( *pvi ) * si + pr[ i ];
		*pvi = ( *pvr ) * si + ( *pvi ) * sr + piw[ i ];
		*pvr = t;
		qr[ i ] = *pvr;
		qi[ i ] = *pvi;
	}
}

static double errev( const int nn, const double qr[], const double qi[], const double ms, const double mp, const double are, const double mre )
{
	int i;
	double e;

	e = cmod( qr[ 0 ], qi[ 0 ] ) * mre / ( are + mre );
	for( i = 0; i <= nn; i++ )
		e = e * ms + cmod( qr[ i ], qi[ i ] );

	return e * ( are + mre ) - mp * mre;
}

static void cauchy( const int nn, double pt[], double q[], double *fn_val )
{
	int i, n;
	double x, xm, f, dx, df;

	pt[ nn ] = -pt[ nn ];

	n = nn;
	x = exp( log( -pt[ nn ] ) - log( pt[ 0 ] ) ) / n;
	if( pt[ n - 1 ] != 0 )
	{
		xm = -pt[ nn ] / pt[ n - 1 ];
		if( xm < x ) x = xm;
	}

	while(1)
	{
		xm = x * 0.1;
		f = pt[ 0 ];
		for( i = 1; i <= nn; i++ )
			f = f * xm + pt[ i ];
		if( f <= 0 )
			break;
		x = xm;
	}
	dx = x;

	while( fabs( dx / x ) > 0.005 )
	{
		q[ 0 ] = pt[ 0 ];
		for( i = 1; i <= nn; i++ )
			q[ i ] = q[ i - 1 ] * x + pt[ i ];
		f = q[ nn ];
		df = q[ 0 ];
		for( i = 1; i < n; i++ )
			df = df * x + q[ i ];
		dx = f / df;
		x -= dx;
	}

	*fn_val = x;
}

static double scale( const int nn, const double pt[], const double eta, const double infin, const double smalno, const double base )
{
	int i, l;
	double hi, lo, max, min, x, sc;
	double fn_val;

	hi = sqrt( infin );
	lo = smalno / eta;
	max = 0;
	min = infin;

	for( i = 0; i <= nn; i++ )
	{
		x = pt[ i ];
		if( x > max ) max = x;
		if( x != 0 && x < min ) min = x;
	}

	fn_val = 1;
	if( min >= lo && max <= hi ) return fn_val;
	x = lo / min;
	if( x <= 1 )
		sc = 1 / ( sqrt( max )* sqrt( min ) );
	else
	{
		sc = x;
		if( infin / sc > max ) sc = 1;
	}
	l = (int)( log( sc ) / log(base ) + 0.5 );
	fn_val = pow( base , l );
	return fn_val;
}

static void cdivid( const double ar, const double ai, const double br, const double bi, double *cr, double *ci )
{
	double r, d, t, infin;

	if( br == 0 && bi == 0 )
	{
		mcon( &t, &infin, &t, &t );
		*cr = infin;
		*ci = infin;
		return;
	}

	if( fabs( br ) < fabs( bi ) )
	{
		r = br/ bi;
		d = bi + r * br;
		*cr = ( ar * r + ai ) / d;
		*ci = ( ai * r - ar ) / d;
		return;
	}

	r = bi / br;
	d = br + r * bi;
	*cr = ( ar + ai * r ) / d;
	*ci = ( ai - ar * r ) / d;
}

static double cmod( const double r, const double i )
{
	double ar, ai;

	ar = fabs( r );
	ai = fabs( i );
	if( ar < ai )
		return ai * sqrt( 1.0 + pow( ( ar / ai ), 2.0 ) );

	if( ar > ai )
		return ar * sqrt( 1.0 + pow( ( ai / ar ), 2.0 ) );

	return ar * sqrt( 2.0 );
}

static void mcon( double *eta, double *infiny, double *smalno, double *base )
{
	*base = 10;
	*eta = DBL_EPSILON;
	*infiny = DBL_MAX;
	*smalno = DBL_MIN;
}
