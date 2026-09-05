#include "include/2Y_OneYukawa.h"
#include "include/2Y_cpoly.h"
#include "include/2Y_utility.h"
#include "include/2Y_PairCorrelation.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

double Y_sigma( double s, double Z, double a, double b, double c, double d )
{
	return -(a / 2. + b + c * exp( -Z )) / s + a * pow( s, -3 ) + b * pow( s, -2 ) + ( c + d ) * pow( s + Z, -1 );
}

double Y_tau( double s, double Z, double a, double b, double c )
{
	return b * pow( s, -2 ) + a * ( pow( s, -3 ) + pow( s, -2 ) ) - pow( s, -1 ) * c * Z * exp( -Z ) * pow( s + Z, -1 );
}

double Y_q( double s, double Z, double a, double b, double c, double d )
{
	return Y_sigma(s, Z, a, b, c, d ) - exp( -s ) * Y_tau( s, Z, a, b, c );
}

double Y_g( double s, double phi, double Z, double a, double b, double c, double d )
{
	return s * Y_tau( s, Z, a, b, c ) * exp( -s ) / ( 1 - 12 * phi * Y_q( s, Z, a, b, c, d ) );
}

double Y_hq( double q, double Z, double K, double v )
{
	double t1, t2, t3, t4;

	if ( q == 0)
	{
		return (exp(-2*Z)*(v + (v*(-1 + Z) - 2*K*Z)*exp(Z))*(-(v*(1 + Z)) + (v + 2*K*Z*(1 + Z))*exp(Z))*pow(K,-1)*pow(Z,-4))/4.;
	}
	else
	{

		t1 = ( 1 - v / ( 2 * K * Z * exp( Z ) ) ) * ( ( 1 - cos( q ) ) / ( q*q ) - 1 / ( Z*Z + q*q ) );
		t2 = ( v*v * ( q * cos( q ) - Z * sin( q ) ) ) / ( 4 * K * Z*Z * q * ( Z*Z + q*q ) );
		t3 = ( q * cos( q ) + Z * sin( q ) ) / ( q * ( Z*Z + q*q ) );
		t4 = v / ( Z * exp( Z ) ) - v*v / ( 4 * K * Z*Z * exp( 2 * Z ) ) - K;

		return v / Z * t1 - t2 + t3 * t4;
	}
}

double Y_pc( double q,
			double Z, double K, double phi,
			double a, double b, double c, double d )
{
	double v = 24 * phi * K * exp( Z ) * Y_g( Z, phi, Z, a, b, c, d );

	double a0 = a * a;
	double b0 = -12 * phi *( pow( a + b,2 ) / 2 + a * c * exp( -Z ) );

	double t1, t2, t3, t4;

	if ( q == 0 )
	{
		t1 = a0 / 3;
		t2 = b0 / 4;
		t3 = a0 * phi / 12;
	}
	else
	{
		t1 = a0 * ( sin( q ) - q * cos( q ) ) / pow( q, 3 );
		t2 = b0 * ( 2 * q * sin( q ) - ( q * q - 2 ) * cos( q ) - 2 ) / pow( q, 4 );
		t3 = a0 * phi * ( ( q*q - 6 ) * 4 * q * sin( q ) - ( pow( q, 4 ) - 12 * q*q + 24) * cos( q ) + 24 ) / ( 2 * pow( q, 6 ) );
	}
	t4 = Y_hq( q, Z, K, v );
	return -24 * phi * ( t1 + t2 + t3 + t4 );
}

double SqOneYukawa( double q,
				 double Z, double K, double phi,
				 double a, double b, double c, double d )
{
	return 1. / ( 1. - Y_pc( q, Z, K, phi, a, b, c, d ) );
}

double Y_LinearEquation_1( double Z, double K, double phi, double a, double b, double c, double d )
{
	return b - 12*phi*(-a/8. - b/6. + d*pow(Z,-2) + c*(pow(Z,-2) - exp(-Z)*(0.5 + (1 + Z)*pow(Z,-2))));
}

double Y_LinearEquation_2( double Z, double K, double phi, double a, double b, double c, double d )
{
	return 1 - a - 12*phi*(-a/3. - b/2. + d*pow(Z,-1) + c*(pow(Z,-1) - (1 + Z)*exp(-Z)*pow(Z,-1)));
}

double Y_LinearEquation_3( double Z, double K, double phi, double a, double b, double c, double d )
{
	return K*exp(Z) - Z*d*(1-12*phi*Y_q(Z, Z, a, b, c, d));
}

double Y_NonlinearEquation( double Z, double K, double phi, double a, double b, double c, double d )
{
	return c + d - 12*phi*((c + d)*Y_sigma(Z, Z, a, b, c, d) - c*exp(-Z)*Y_tau(Z, Z, a, b, c));
}

int Y_CheckSolution( double Z, double K, double phi,
					 double a, double b, double c, double d )
{
	double eq_1 = chop( Y_LinearEquation_1 ( Z, K, phi, a, b, c, d ) );
	double eq_2 = chop( Y_LinearEquation_2 ( Z, K, phi, a, b, c, d ) );
	double eq_3 = chop( Y_LinearEquation_3 ( Z, K, phi, a, b, c, d ) );
	double eq_4 = chop( Y_NonlinearEquation( Z, K,  phi, a, b, c, d ) );

	return eq_1 == 0 && eq_2 == 0 && eq_3 == 0 && eq_4 == 0;
}

int Y_SolveEquations( double Z, double K, double phi, double* a, double* b, double* c, double* d, int debug )
{
	double sol_a[4], sol_b[4], sol_c[4], sol_d[4];

	double m11 = (3*phi)/2.;
	double m13 = 6*phi*exp(-Z)*(2 + Z*(2 + Z) - 2*exp(Z))*pow(Z,-2);
	double m23 = -12*phi*exp(-Z)*(-1 - Z + exp(Z))*pow(Z,-1);
	double m31 = -6*phi*exp(-Z)*pow(Z,-2)*(2*(1 + Z) + exp(Z)*(-2 + pow(Z,2)));
	double m32 = -12*phi*(-1 + Z + exp(-Z))*pow(Z,-1);
	double m33 = 6*phi*exp(-2*Z)*pow(-1 + exp(Z),2);

	double delta = m23*m31 - m13*m32 + m11*(-4*m13*m31 + (4*m23*m31)/3. + (8*m13*m32)/3. - m23*m32) + m33 + (4*(-3 + m11)*m11*m33)/9.;
	double a1 = -(K*(m23 + (4*m11*(-3*m13 + m23))/3.)*exp(Z));
	double a2 = -(m13*(m32 + 4*m11*Z)) + ((3 + 4*m11)*(m33 + m23*Z))/3.;
	double a3 = -2*phi*pow(Z,-2)*(6*m23*m32 - 24*m11*m33 + 2*Z*((3 + 4*m11)*m33 - 3*m13*(m32 + 2*m11*Z)) + (3 + 4*m11)*m23*pow(Z,2));

	double b1 = -(K*((-3 + 8*m11)*m13 - 3*m11*m23)*exp(Z))/3.;
	double b2 = m13*(m31 - Z + (8*m11*Z)/3.) - m11*(m33 + m23*Z);
	double b3 = 2*phi*pow(Z,-2)*(m13*Z*(-6*m31 + 3*Z - 8*m11*Z) + 2*m33*(3 - 8*m11 + 3*m11*Z) + 3*m23*(2*m31 + m11*pow(Z,2)));

	double c1 = -(K*exp(Z)*pow(3 - 2*m11,2))/9.;
	double c2 = -((3 + 4*m11)*m31)/3. + m11*m32 + Z + (4*(-3 + m11)*m11*Z)/9.;
	double c3 = (-2*phi*pow(Z,-2)*(6*(12*m11*m31 + 3*m32 - 8*m11*m32) - 6*((3 + 4*m11)*m31 - 3*m11*m32)*Z + pow(3 - 2*m11,2)*pow(Z,2)))/3.;

	double real_coefficient[5];
	double imag_coefficient[5];

	double real_root[4];
	double imag_root[4];

	double zeta = 24*phi*pow(-6*phi*Z*cosh(Z/2.) + (12*phi + (-1 + phi)*pow(Z,2))*sinh(Z/2.),2);
	double A[5];
	int degree,i,j,n_roots;
	double x,y;
	int n,selected_root;
	double qmax,q,dq,min,sum,dr;
	double *sq,*gr;

	A[0] = -(exp(3*Z)*pow(K,2)*pow(-1 + phi,2)*pow(Z,3) / zeta );
	A[1] = K*Z*exp(Z)*(6*phi*(2 + 4*phi + (2 + phi)*Z) + exp(Z)*
					   ((-24 + Z*(18 + (-6 + Z)*Z))*pow(phi,2) - 2*phi*(6 + (-3 + Z)*pow(Z,2)) + pow(Z,3))) / zeta;
	A[2] = -12*K*phi*exp(Z)*pow(-1 + phi,2)*pow(Z,3)/zeta;
	A[3] = 6*phi*Z*exp(-Z)*(-12*phi*(1 + 2*phi)*(-1 + exp(Z)) + 6*phi*Z*(3*phi + (2 + phi)*exp(Z)) +
							6*(-1 + phi)*phi*pow(Z,2) + pow(-1 + phi,2)*pow(Z,3))/zeta;
	A[4] = -36*exp(-Z)*pow(-1 + phi,2)*pow(phi,2)*pow(Z,3)/zeta;

	degree = 4;

	for ( i = 0; i <= degree; i++ )
	{
		real_coefficient[i] = A[4-i];
		imag_coefficient[i] = 0.;
	}

	cpoly( real_coefficient, imag_coefficient, degree, real_root, imag_root );

	j = 0;
	for ( i = 0; i < degree; i++ )
	{
		x = real_root[i];
		y = imag_root[i];

		if ( chop( y ) == 0 )
		{
			sol_a[j] = ( a1 + a2 * x + a3 * x * x ) / ( delta * x );
			sol_b[j] = ( b1 + b2 * x + b3 * x * x ) / ( delta * x );
			sol_c[j] = ( c1 + c2 * x + c3 * x * x ) / ( delta * x );
			sol_d[j] = x;

			j++;
		}
	}

	n_roots = j;

	if ( n_roots > 1 )
	{
		n = 1 << 14;

		qmax = 1000.;
		dq = qmax / ( n - 1 );

		sq = malloc( sizeof( double ) * n );
		gr = malloc( sizeof( double ) * n );

		min = 1e99;
		selected_root=0;

		for ( j = 0; j < n_roots; j++)
		{
			for ( i = 0; i < n ; i++)
			{
				q = dq * i;
				sq[i] = SqOneYukawa( q, Z, K, phi, sol_a[j], sol_b[j], sol_c[j], sol_d[j] );
			}

			PairCorrelation( phi, dq, sq, &dr, gr, n );

			sum = 0;
			for (i = 0; i < floor( 1. / dr ); i++ )
			{
				sum += fabs( gr[i] );
			}

			if ( sum < min )
			{
				min = sum;
				selected_root = j;
			}
		}
		free( gr );
		free( sq );

		*a = sol_a[ selected_root ];
		*b = sol_b[ selected_root ];
		*c = sol_c[ selected_root ];
		*d = sol_d[ selected_root ];

		return 1;
	}
	else if ( n_roots == 1 )
	{
		*a = sol_a[0];
		*b = sol_b[0];
		*c = sol_c[0];
		*d = sol_d[0];

		return 1;
	}
	return 0;
}
