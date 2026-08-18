#include "include/2Y_PairCorrelation.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int PairCorrelation( double phi, double dq, double* Sq, double* dr, double* gr, int N )
{
	double* data = malloc( sizeof(double) * N * 2);
	int nn,error,k;
	double alpha,real,imag;
	double Pi = 3.14159265358979323846264338327950288;

	for ( nn = 0; nn < N; nn++ ) {
		data[2*nn] = nn * ( Sq[nn] - 1 );
		data[2*nn+1] = 0;
	}

	error  = 1;
	dfour1( data-1, N, 1 );

	if ( error == 1 )
	{
		alpha = N * pow( dq, 3 ) / ( 24 * Pi * Pi * phi );

		*dr = 2 * Pi / ( N * dq );
		for ( k = 0; k < N; k++ )
		{
			if ( 2*k == 0 || 2*k == 2*N / 2)
			{
				real = data[2*k];
				imag = 0;
			}
			else if ( 2*k < 2*N / 2 )
			{
				real = data[2*k];
				imag = data[2*k+1];
			}
			else if ( 2*k > 2*N / 2 )
			{
				real =  data[2*k];
				imag = -data[2*k+1];
			}

			if ( k == 0 )
				gr[k] = 0;
			else
				gr[k] = 1. + alpha / k * (imag);
		}
	}

	free( data );

	return error;
}

#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr

void dfour1(double data[], unsigned long nn, int isign)
{
	unsigned long n,mmax,m,j,istep,i;
	double wtemp,wr,wpr,wpi,wi,theta;
	double tempr,tempi;

	n=nn << 1;
	j=1;
	for (i=1;i<n;i+=2) {
		if (j > i) {
			SWAP(data[j],data[i]);
			SWAP(data[j+1],data[i+1]);
		}
		m=n >> 1;
		while (m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	mmax=2;
	while (n > mmax) {
		istep=mmax << 1;
		theta=isign*(6.28318530717959/mmax);
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) {
			for (i=m;i<=n;i+=istep) {
				j=i+mmax;
				tempr=wr*data[j]-wi*data[j+1];
				tempi=wr*data[j+1]+wi*data[j];
				data[j]=data[i]-tempr;
				data[j+1]=data[i+1]-tempi;
				data[i] += tempr;
				data[i+1] += tempi;
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}
}
#undef SWAP
