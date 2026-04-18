// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 * 		         Fast Fourier Transform
 *
 *		     Initialization of the FFT packet
 *
 *  The file defines the constructor and the destructor for the FFT class,
 *  allocates global arrays and initializes global data
 *
 ************************************************************************
 */

#ifdef __GNUC__
#pragma implementation "fft.h"
#endif
#include "fft.h"


/*
 *------------------------------------------------------------------------
 *			Constructor and Destructor
 */

FFT::FFT(const int n, const double dr)
     : N(n), dr(dr)
{
  assure( n > 3, "At least 4 points must be given for transforms");
  assure( dr > 0, "Grid mesh in the r-space, dr, must be positive");

  register int i;
  for(i=1, logN=0; i < N; i *= 2, logN++)
    ;
  assure( i == N, "No. of points has to be the exact power of two");

  A = new Complex[N];
  W = new Complex[N];
  index_conversion_table = new short[N];
  A_end = A + N;
  
  fill_in_W();
  fill_in_index_conversion_table();
}

FFT::~FFT(void)
{
  assert( N > 0 && A != 0 && W != 0 && A_end == A + N );
  delete A;
  delete W;
  delete index_conversion_table;
}

/*
 *------------------------------------------------------------------------
 *		Fill in the index_conversion_table so that
 *  if j = J[m-1] * 2^(m-1) +  J[m-2] * 2^(m-2) + ... + J1 * 2^1 + J0 * 2^0
 *  then index_conversion_table[j] =
 *	   J0 * 2^(m-1) + J1 * 2^(m-2) + ... + J[m-2] * 2^1 + J[m-1] * 2^0
 *
 *  m being Log2(N)
 */

void FFT::fill_in_index_conversion_table(void)
{
  register int j;			// j counts 0,1,2,... N-1
  register int jp;			// Reverse counter:
					// N, N/2, N/4, N/4+N/2, N/8, N/8+N/2,
					// with N identical to 0 (mod N)
  register short * tablj = index_conversion_table;
  register int k;

  for(j=0, jp=0; *tablj++ = jp, ++j < N;)
  {
    for(k=N/2; k <= jp; jp -= k, k /= 2)
      ;
    jp += k;
  }
}


				// Fill in the W array
				// W[j] = exp( -I 2pi/N * j ), j=0..N-1
void FFT::fill_in_W(void)
{
  register Complex * wj = W;
  register int j;
  for(j=0; j<N; j++)
  {
    Complex arg = Complex(0,-2*M_PI/N * j);
    *wj++ = exp(arg);
  }
}
