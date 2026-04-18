// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 * 		         Fast Fourier Transform
 *
 *		      Processing of the Input Sequence
 *
 * The functions below handle different cases of the input (real/complex,
 * with/without zero padding), form the array A, and perform the radix 2 FFT
 * algorithm itself, taking advantage of the particular form of input.
 *
 * General radix 2 FFT algorithm is as follows
 *	Input: x_re[i], x_im[i]		the input sequence
 *	Output: Aj = SUM[ Xk * W^(k*j) ],  j,k = 0:N-1,
 *	where
 *		W = exp(-2pi I/N)
 *		Xk = ( k<N/2 ? x_re[k] + I x_im[k] : 0.0 ) with zero padding
 *		Xk = x_re[k] + I x_im[k], without zero padding
 *                                
 * Algorithm
 *	1.  Fill in the complex array A performing the permutation
 *	    of input data
 *		Ai = ( j < N/2 ? x_re[j] + I x_im[j] : 0.0 ), j = 0..N-1
 *		or
 *		Ai = ( x_re[j] + I x_im[j] ), j = 0..N-1
 *	    where 
 * 	    index i is the "inverse" of the index j (in the sense that
 *	    m-bit string representing the value of 'i' is read from right
 *	    to left the string corresponding to index 'j'; m = log2(N)
 *
 *	2. Transform itself
 *	   n2_l = 1;			// 2^l at l=0
 *	   for l=0 to m-1 do		// m = log2(N)
 *	       for k=0 by 2* n2_l to N-1 do
 *	           for j=0 to n2_l-1 do
 *			compl w = exp(- I j*pi/2^l );
 *		        i1 = j + k;
 *			i2 = i1 + n2_l;
 *			A[i1] = A[i1] + A[i2]*w		// Butterfly operation
 *			A[i2] = A[i1] - A[i2]*w	
 *		   od
 *             od
 *	       n2_l *= 2				// 2^l at the next l
 *	   od
 *
 * The first three steps of the algorithm above can be performed with
 * simplified formulas from
 *		G.Nussbaumer. Fast Fourier Transform and Convolution
 *		Algorithms, M., Radio i sviaz, 1985, p.135 (in Russian)
 * The formulas carry out the 8-point FFT for each set of 8 points
 * A[k+0], A[k+1], ... A[k+7], k=0,8,16..N-8 	
 *
 * The formulas are given below for references, x0-x7 being the source
 * data, y0-y7 the transformed data. Moreover, one has to keep in mind
 * that the source data x0-x7 have been already permuted (as opposite to
 * the book that assumes the source data to be arranged in the natural
 * order rather than permutted one)
 *
 *	t1 = x0 + x1       m0 = t7 + t8           s1 = m3 + m4    y0 = m0
 *	t2 = x2 + x3       m1 = t7 - t8           s2 = m3 - m4    y1 = s1 + s3
 *	t3 = x4 - x5       m2 = t1 - t2           s3 = m6 + m7    y2 = m2 + m5
 *	t4 = x4 + x5       m3 = x0 - x1           s4 = m6 - m7    y3 = s2 - s4
 *	t5 = x6 + x7       m4 = cu*(t3 - t6)                      y4 = m1
 *	t6 = x6 - x7       m5 = I * (t5 - t4)                     y5 = s2 + s4
 *	t7 = t1 + t2       m6 = I * (x3 - x2)                     y6 = m2 - m5
 *	t8 = t4 + t5       m7 = -Isu* (t3 + t6)                   y7 = s1 - s3
 *
 *	cu = su = sin( pi/4 )
 *
 * $Id: fft_input.cc,v 1.3 1998/12/20 23:06:18 oleg Exp oleg $
 *
 ************************************************************************
 */

#include "fft.h"

/*
 *-----------------------------------------------------------------------
 *			Perform the transform itself
 * on the complex array A _inplace_. The first three steps of the algorithm
 * 		   are assumed to have been already performed.
 *
 */

void FFT::complete_transform(void)
{
  register int n2_l = 8;		// 2^l at l=3
  register int inc = N/2/n2_l;		// Increment for the index in W

  for(; n2_l < N; n2_l <<= 1, inc >>= 1)
  {
    register Complex * ak;
    for(ak=A; ak < A_end; ak += 2*n2_l)
    {
      register Complex * aj1 = ak;
      register Complex * aj2 = aj1 + n2_l;
      register Complex * w = W + inc;	// Since j=0 case is handled explicitly

      Complex ai1  = *aj1;
      Complex ai2w = *aj2;     		// Butterfly operation in case of
      *aj1++ += ai2w;			// j=0 ==> W=1
      *aj2++ = ai1 - ai2w;
      for(; aj1 < ak+n2_l; w += inc)
      {
        Complex ai1 = *aj1;		// w = exp(-I 2pi/N j*inc) =
        Complex ai2w = *w * *aj2;   	// exp(-I j pi/2^l)
        *aj1++ += ai2w;
        *aj2++ = ai1 - ai2w;		// Butterfly operation
      }
    }
  }
}

/*
 *------------------------------------------------------------------------
 *	The most general case of complex input without zero padding
 *
 * The Nussbaumer formulas are applied as they are, no further simplification
 * seems possible.
 */

void FFT::input(
	     const Vector& x_re,	// [0:N-1] vector - Re part of input
	     const Vector& x_im)	// [0:N-1] vector - Im part of input
{
  are_compatible(x_re,x_im);
  if( x_re.q_lwb() != 0 || x_re.q_upb() != N-1 )
    _error("Sorry, vector [%d:%d] cannot be processed.\n"
	   "Only [0:%d] vectors are valid",
	   x_re.q_lwb(), x_re.q_upb(), N-1);

  register int k;
  register Complex * ak;

  for(ak=A,k=0; k < N; k +=8)
  {
	const double cu = std::real(W[ N/8 ]);			// cos( pi/4 )

    register int i;
    i = index_conversion_table[k];	// Index being "reverse" to k
    Complex x0(x_re(i),x_im(i));

    i = index_conversion_table[k+1];
    Complex x1(x_re(i),x_im(i));

    i = index_conversion_table[k+2];
    Complex x2(x_re(i),x_im(i));

    i = index_conversion_table[k+3];
    Complex x3(x_re(i),x_im(i));

    i = index_conversion_table[k+4];
    Complex x4(x_re(i),x_im(i));

    i = index_conversion_table[k+5];
    Complex x5(x_re(i),x_im(i));

    i = index_conversion_table[k+6];
    Complex x6(x_re(i),x_im(i));

    i = index_conversion_table[k+7];
    Complex x7(x_re(i),x_im(i));

    Complex t1 = x0 + x1;
    Complex t2 = x2 + x3;
    Complex t3 = x4 - x5;
    Complex t4 = x4 + x5;
    Complex t5 = x6 + x7;
    Complex t6 = x6 - x7;
    Complex t7 = t1 + t2;
    Complex t8 = t4 + t5;

#define m2 t1
    m2 -= t2;				// Forget t1 from now on
#define m3 x0
    m3 -= x1;				// Forget x0 from now on
    Complex m7 = t3 + t6; m7 = Complex(cu* std::imag(m7),-cu* std::real(m7));
#define m4 t3				// Forget t3 from now on
    m4 -= t6;
    m4 *= cu;
    Complex m5 = t5 - t4; m5 = Complex(- std::imag(m5), std::real(m5));
    Complex m6 = x3 - x2; m6 = Complex(- std::imag(m6), std::real(m6));

#define s1 m3
    Complex s2 = m3 - m4;		// Forget m3 from now on
    s1 += m4;
#define s3 m6				// Forget m6 from now on
    Complex s4 = m6 - m7;
    s3 += m7;

    *ak++ = t7 + t8; 				// y0  
    *ak++ = s1 + s3;				// y1
    *ak++ = m2 + m5;				// y2
    *ak++ = s2 - s4;				// y3
    *ak++ = t7 - t8;				// y4
    *ak++ = s2 + s4;				// y5
    *ak++ = m2 - m5;				// y6
    *ak++ = s1 - s3;				// y7
  }
  assert( ak == A_end );
  complete_transform();
}
#undef m2
#undef m3
#undef m4
#undef s1
#undef s3

/*
 *------------------------------------------------------------------------
 *		   Real input sequence without zero padding
 * When x[j]-s are all real, some intermediate results are either pure
 * real, or pure imaginaire, which are much cheaper to compute than
 * the complex ones. In Nussbauner's formulas above, t1 through t8,
 * m0 through m4, s1 and s2 are all real, whilest m5 through m7, s3, and
 * s4 are pure imaginaire.
 *
 */

void FFT::input(
	     const Vector& x_re)	// [0:N-1] vector - Re part of input
{
  x_re.is_valid();
  if( x_re.q_lwb() != 0 || x_re.q_upb() != N-1 )
    _error("Sorry, vector [%d:%d] cannot be processed.\n"
	   "Only [0:%d] vectors are valid",
	   x_re.q_lwb(), x_re.q_upb(), N-1);

  register int k;
  register Complex * ak;

  for(ak=A,k=0; k < N; k +=8)
  {
    const double cu = std::real(W[ N/8 ]);			// cos( pi/4 )

    register int i;
    i = index_conversion_table[k];	// Index being "reverse" to k
    double x0 = x_re(i);

    i = index_conversion_table[k+1];
    double x1 = x_re(i);

    i = index_conversion_table[k+2];
    double x2 = x_re(i);

    i = index_conversion_table[k+3];
    double x3 = x_re(i);

    i = index_conversion_table[k+4];
    double x4 = x_re(i);

    i = index_conversion_table[k+5];
    double x5 = x_re(i);

    i = index_conversion_table[k+6];
    double x6 = x_re(i);

    i = index_conversion_table[k+7];
    double x7 = x_re(i);

    double t1 = x0 + x1;
    double t2 = x2 + x3;
    double t3 = x4 - x5;
    double t4 = x4 + x5;
    double t5 = x6 + x7;
    double t6 = x6 - x7;
    double t7 = t1 + t2;
    double t8 = t4 + t5;

#define m2 t1
    m2 -= t2;				// Forget t1 from now on
#define m3 x0
    m3 -= x1;				// Forget x0 from now on
    double m7i = -cu*(t3 + t6);
#define m4 t3				// Forget t3 from now on
    m4 -= t6;
    m4 *= cu;
    double m5i = t5 - t4;
    double m6i = x3 - x2;

#define s1 m3
    double s2 = m3 - m4;		// Forget m3 from now on
    s1 += m4;
#define s3i m6i				// Forget m6 from now on
    double s4i = m6i - m7i;
    s3i += m7i;

    *ak++ = t7 + t8; 				// y0  
    *ak++ = Complex(s1,s3i);			// y1
    *ak++ = Complex(m2,m5i);			// y2
    *ak++ = Complex(s2,-s4i);			// y3
    *ak++ = t7 - t8;				// y4
    *ak++ = Complex(s2,s4i);			// y5
    *ak++ = Complex(m2,-m5i);			// y6
    *ak++ = Complex(s1,-s3i);			// y7
  }
  assert( ak == A_end );
  complete_transform();
}
#undef m2
#undef m3
#undef m4
#undef s1
#undef s3i

/*
 *------------------------------------------------------------------------
 *			Complex input with zero padding
 *
 * Note, if index j > N/2, its "inverse", index i is odd. Since the second
 * half of the input data is zero (and isn't specified), x1, x3, x5, and x7
 * in the Nussbaumer formulas above are zeros, and the formulas can be
 * simplified as follows
 *
 *	t1 = x0                                   s1 = x0 + m4    y0 = t7 + t8
 *	t2 = x2                                   s2 = x0 - m4    y1 = s1 + s3
 *	t3 = x4            m2 = x0 - x2           s3 = m6 + m7    y2 = m2 + m5
 *	t4 = x4            m3 = x0                s4 = m6 - m7    y3 = s2 - s4
 *	t5 = x6            m4 = cu*(x4 - x6)                      y4 = t7 - t8
 *	t6 = x6            m5 = -I * (x4 - x6)                    y5 = s2 + s4
 *	t7 = x0 + x2       m6 = -I * x2                           y6 = m2 - m5
 *	t8 = x4 + x6       m7 = -Isu* (x4 + x6)                   y7 = s1 - s3
 *
 *	cu = su = sin( pi/4 )
 */

void FFT::input_pad0(
	     const Vector& x_re,	// [0:N/2-1] vector - Re part of input
	     const Vector& x_im)	// [0:N/2-1] vector - Im part of input
{
  are_compatible(x_re,x_im);
  if( x_re.q_lwb() != 0 || x_re.q_upb() != N/2-1 )
    _error("Sorry, vector [%d:%d] cannot be processed.\n"
	   "Zero padding is assumed, only [0:%d] vectors are valid",
	   x_re.q_lwb(), x_re.q_upb(), N/2-1);

  register int k;
  register Complex * ak;

  for(ak=A,k=0; k < N; k +=8)
  {
    const double cu = std::real(W[ N/8 ]);			// cos( pi/4 )

    register int i;
    i = index_conversion_table[k];	// Index being "reverse" to k
    Complex x0(x_re(i),x_im(i));

    i = index_conversion_table[k+2];
    Complex x2(x_re(i),x_im(i));

    i = index_conversion_table[k+4];
    Complex x4(x_re(i),x_im(i));

    i = index_conversion_table[k+6];
    Complex x6(x_re(i),x_im(i));

    Complex t7 = x0 + x2;
    Complex t9 = x4 - x6;
#define t8 x4				// Forget x4 from now on
    t8 += x6;

    Complex m2 = x0 - x2;
    Complex m5(std::imag(t9),- std::real(t9));
    Complex m6(std::imag(x2),- std::real(x2));
    Complex m7(cu* std::imag(t8),-cu* std::real(t8));
#define m4 t9				// Forget t9 from now on
    m4 *= cu;

#define s1 x0
    Complex s2 = x0 - m4;		// Forget x0 from now on
    s1 += m4;
#define s3 m6				// Forget m6 from now on
    Complex s4 = m6 - m7;
    s3 += m7;

    *ak++ = t7 + t8; 				// y0  
    *ak++ = s1 + s3;				// y1
    *ak++ = m2 + m5;				// y2
    *ak++ = s2 - s4;				// y3
    *ak++ = t7 - t8;				// y4
    *ak++ = s2 + s4;				// y5
    *ak++ = m2 - m5;				// y6
    *ak++ = s1 - s3;				// y7
  }
  assert( ak == A_end );
  complete_transform();
}
#undef t8
#undef m4
#undef s1
#undef s3

/*
 *------------------------------------------------------------------------
 *		   Real input sequence with zero padding
 * Again, since the input is a real sequence, some intermediate results
 * are also real, which makes things simpler.
 */

void FFT::input_pad0(
	     const Vector& x_re)	// [0:N/2-1] vector - Re part of input
{
  x_re.is_valid();
  if( x_re.q_lwb() != 0 || x_re.q_upb() != N/2-1 )
    _error("Sorry, vector [%d:%d] cannot be processed.\n"
	   "Zero padding is assumed, only [0:%d] vectors are valid",
	   x_re.q_lwb(), x_re.q_upb(), N/2-1);

  register int k;
  register Complex * ak;

  for(ak=A,k=0; k < N; k +=8)
  {
    const double cu = std::real(W[ N/8 ]);			// cos( pi/4 )

    register int i;
    i = index_conversion_table[k];	// Index being "reverse" to k
    double x0 = x_re(i);

    i = index_conversion_table[k+2];
    double x2 = x_re(i);

    i = index_conversion_table[k+4];
    double x4 = x_re(i);

    i = index_conversion_table[k+6];
    double x6 = x_re(i);

    double t7 = x0 + x2;
    double t9 = x4 - x6;
#define t8 x4				// Forget x4 from now on
    t8 += x6;

    double m2 = x0 - x2;
    double m5i = -t9;
    double m7i = -cu*t8;
#define m4 t9				// Forget t9 from now on
    m4 *= cu;

#define s1 x0
    double s2 = x0 - m4;		// Forget x0 from now on
    s1 += m4;
    double s3i = -x2 + m7i;
    double s4i = -x2 - m7i;

    *ak++ = t7 + t8; 				// y0  
    *ak++ = Complex(s1,s3i);			// y1
    *ak++ = Complex(m2,m5i);			// y2
    *ak++ = Complex(s2,-s4i);			// y3
    *ak++ = t7 - t8;				// y4
    *ak++ = Complex(s2,s4i);			// y5
    *ak++ = Complex(m2,-m5i);			// y6
    *ak++ = Complex(s1,-s3i);			// y7
  }
  assert( ak == A_end );
  complete_transform();
}
#undef t8
#undef m4
#undef s1
