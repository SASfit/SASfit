// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 *  			    Fast Fourier Transform
 *
 * This package computes sums of the form
 *	(1) xf[k] = SUM{ x[j] *  exp(-2*PI*I/N j*k), j=0..N-1 }, k=0..N-1
 *	(2) x[j]  = 1/N SUM{ xf[k] * exp(+2*PI*I/N j*k), k=0..N-1 }, j=0..N-1
 *	(3) xf[k] = SUM{ x[j] *  exp(-2*PI*I/N j*k), j=0..N/2-1 }, k=0..N/2-1
 *
 * where N is an exact power of two.
 *
 * Formula (1) defines the classical Discrete Fourier transform, with x[j]
 * being a real or complex sequence. The result xf[k] is always a complex
 * sequence; the user however may choose to retrieve only its real or
 * imaginary part, or any combination thereof (for example, the power spectrum).
 * Formula (2) is the inverse formula for the DFT.
 * Formula (3) is nothing but a trapezoid rule approximation to a
 * Fourier integral; the trapezoid rule is the most stable one with respect
 * to the noise in the input data. Again, x[j] may be either a real or
 * a complex sequence. The mesh size other than 1 can be specified as well.
 *
 * $Id: fft.h,v 1.4 1998/12/22 20:50:38 oleg Exp oleg $
 *
 ************************************************************************
 */

#if !defined(__GNUC__)
#pragma once
#else
#pragma interface
#endif

#ifndef _fft_h
#define _fft_h 1

#include "LAStreams.h"
#include <math.h>
#if defined(__GNUC__)
#include <complex.h>
#if (__GNUC__ == 2) && (__GNUC_MINOR__ < 8)
#define std
#endif
#else
#include <complex>
#if defined(MSIPL_COMPLEX_H) && !defined(MSIPL_USING_NAMESPACE)
#define std
#else
using namespace std;
#endif
typedef std::complex<double> double_complex;
#endif

typedef double_complex Complex;

class FFT
{
  RWWatchDog ref_counter;		// To control read/write access
  					// to this class

  const int N;				// No of points the FFT packet
					// has been initialized for
  int logN;				// log2(N)
  const double dr;			// Mesh size in the r-space

  Complex * A;				// [0:N-1] work array
					// the transform is placed to

  Complex * A_end;         		// Ptr to the memory location next to
					// the last A element

  short * index_conversion_table;	// index_conversion_table[i]
					// is a bit-inverted i, i=0..N

  Complex * W;				// FFT weight factors
					// exp( -I j 2pi/N ), j=0..N-1

			// Private package procedures
  void fill_in_index_conversion_table(void);
  void fill_in_W(void);

  void complete_transform(void);


				// Those aren't implemented; but making them
				// private forbids the assignement
  FFT(const FFT&);
  void operator= (const FFT&);

public:

			// Constructor; n is the number of points to transform,
			// dr is the grid mesh in the r-space
  FFT(const int n, const double dr=1);	
  ~FFT(void);

			// Fundamental procedures,
			// Input the data and perform the transform
  void input(				// Preprocess the real input sequence
	     const Vector& x);		// Real [0:N-1] vector
  
  void input(				// Preprocess the complex input seq
	     const Vector& x_re,	// [0:N-1] vector - Re part of input
	     const Vector& x_im);	// [0:N-1] vector - Im part of input

			// Preprocess the input with zero padding
  void input_pad0(			// Preprocess the real input sequence
	     const Vector& x);		// Real [0:N/2-1] vector

  void input_pad0(			// Preprocess the complex input seq
	     const Vector& x_re,	// [0:N/2-1] vector - Re part of input
	     const Vector& x_im);	// [0:N/2-1] vector - Im part of input

			// Output results in the form the user wants them
  void real(				// Give only the Re part of the result
	    LAStreamOut& xf_re);	// [0:N-1] vector

  void imag(				// Give only the Im part of the result
	    LAStreamOut& xf_im);	// [0:N-1] vector
  
  void abs(				// Give only the abs value
	   LAStreamOut& xf_abs);	// [0:N-1] vector (power spectrum)

				// Return only the half of the result
				// (if the second half is unnecessary due
				// to the symmetry)
  void real_half(			// Give only the Re part of the result
		 LAStreamOut& xf_re);	// [0:N/2-1] vector

  void imag_half(			// Give only the Im part of the result
		 LAStreamOut& xf_im);	// [0:N/2-1] vector
  
  void abs_half(			// Give only the abs value
		 LAStreamOut& xf_abs);	// [0:N/2-1] vector


			// Perform sin/cos transforms of f: R+ -> R
			// Source and destination arguments of the functions
			// below may point to the same vector (in that case,
			// transform is computed inplace)

             			// Sine-transform of the function f(x)
				// Integrate[ f(x) sin(kx) dx], x=0..Infinity
  void sin_transform(		// j=0..n-1, n=N/2
	LAStreamOut& dest,		// F(k) tabulated at kj = j*dk
	const Vector& src		// f(x) tabulated at xj = j*dr
            );

             			// Cosine-transform of the function f(x)
				// Integrate[ f(x) cos(kx) dx], x=0..Infinity
  void cos_transform(		// j=0..n-1, n=N/2
	LAStreamOut& dest,		// F(k) tabulated at kj = j*dk
	const Vector& src		// f(x) tabulated at xj = j*dr
            );

             			// Inverse sine-transform of the function F(k)
				// 2/pi Integrate[ F(k) sin(kx) dk], k=0..Inf
  void sin_inv_transform(	// j=0..n-1, n=N/2
	LAStreamOut& dest,		// f(x) tabulated at xj = j*dr
	const Vector& src		// F(k) tabulated at kj = j*dk
            );

             			// Inverse cosine-transform of function F(k)
				// 2/pi Integrate[ F(k) cos(kx) dk], k=0..Inf
  void cos_inv_transform(	// j=0..n-1, n=N/2
	LAStreamOut& dest,		// f(x) tabulated at xj = j*dr
	const Vector& src		// F(k) tabulated at kj = j*dk
            );


             			// Inquires
  int q_N(void) const			{ return N; }
  int q_logN(void) const		{ return logN; }
  double q_dr(void) const		{ return dr;  }
  double q_dk(void) const		{ return 2*M_PI/N/dr; }
  double q_r_cutoff(void) const		{ return N/2 * dr; }
  double q_k_cutoff(void) const		{ return M_PI/dr; }

};

#endif


