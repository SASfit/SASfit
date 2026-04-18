// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 * 		         Fast Fourier Transform
 *
 *		Return the transformation results in the form
 *			   the user wants them
 *
 * $Id: fft_output.cc,v 1.4 1998/12/20 23:08:36 oleg Exp oleg $
 *
 ************************************************************************
 */

#include "fft.h"

/*
 *-----------------------------------------------------------------------
 *	     Give a real/imaginaire part / absolute value
 *			of the complex transform
 */

void FFT::real(LAStreamOut& xf_re)		// must hold exactly N elements
{
  for(register const Complex * ap = A; ap < A_end; )
    xf_re.get() = std::real(*ap++);
  if( !xf_re.eof() )
    _error("The output stream was supposed to end after all %d points of the FFT "
	   "were placed into it",N);
}

void FFT::imag(LAStreamOut& xf_im)		// must hold exactly N elements
{
  for(register const Complex * ap = A; ap < A_end; )
    xf_im.get() = std::imag(*ap++);
  if( !xf_im.eof() )
    _error("The output stream was supposed to end after all %d points of the FFT "
	   "were placed into it",N);
}

void FFT::abs(LAStreamOut& xf_abs)		// must hold exactly N elements
{
  for(register const Complex * ap = A; ap < A_end; )
    xf_abs.get() = std::abs(*ap++);
  if( !xf_abs.eof() )
    _error("The output stream was supposed to end after all %d points of the FFT "
	   "were placed into it",N);
}

/*
 *-----------------------------------------------------------------------
 *	       Give only a half of the resulting transform
 */


void FFT::real_half(LAStreamOut& xf_re)		// must hold exactly N/2 elements
{
  for(register const Complex * ap = A; ap < A + N/2; )
    xf_re.get() = std::real(*ap++);
  if( !xf_re.eof() )
    _error("The output stream was supposed to end after all %d points of the FFT "
	   "were placed into it",N/2);
}

void FFT::imag_half(LAStreamOut& xf_im)		// must hold exactly N/2 elements
{
  for(register const Complex * ap = A; ap < A + N/2; )
    xf_im.get() = std::imag(*ap++);
  if( !xf_im.eof() )
    _error("The output stream was supposed to end after all %d points of the FFT "
	   "were placed into it",N/2);
}

void FFT::abs_half(LAStreamOut& xf_abs)		// must hold exactly N/2 elements
{
  for(register const Complex * ap = A; ap < A + N/2; )
    xf_abs.get() = std::abs(*ap++);
  if( !xf_abs.eof() )
    _error("The output stream was supposed to end after all %d points of the FFT "
	   "were placed into it",N/2);
}

/*
 *-----------------------------------------------------------------------
 *		Perform sin/cos transforms of a real function
 *  			as a postprocessing of FFT
 *
 * Sine-transform:   F(k) = Integrate[ f(x) sin(kx) dx ], x = 0..Infinity
 * Cosine-transform: F(k) = Integrate[ f(x) cos(kx) dx ], x = 0..Infinity
 * Inverse
 *   sin-transform:  f(x) = 2/pi Integrate[ F(k) sin(kx) dk ], k = 0..Infinity
 *   cos-transform:  f(x) = 2/pi Integrate[ F(k) cos(kx) dk ], k = 0..Infinity
 *
 * Function f(x) is tabulated over the uniform grid xj = j*dr, j=0..n-1
 * Function F(k) is tabulated over the uniform grid kj = j*dk, j=0..n-1
 * 							n=N/2
 * Source and destination arguments of the functions below may point to
 * the same vector (in that case, transform is computed inplace)
 */

void FFT::sin_transform(LAStreamOut& F, const Vector& f)
{
  input_pad0(f);

  for(register const Complex * ap = A; ap < A + N/2; )
    F.get() = - std::imag(*ap++) * dr;
  if( !F.eof() )
    _error("The output stream was supposed to end after all %d points of the sin-tr "
	   "were placed into it",N/2);
}

void FFT::cos_transform(LAStreamOut& F, const Vector& f)
{
  input_pad0(f);

  for(register const Complex * ap = A; ap < A + N/2; )
    F.get() = std::real(*ap++) * dr;
  if( !F.eof() )
    _error("The output stream was supposed to end after all %d points of the cos-tr "
	   "were placed into it",N/2);
}

void FFT::sin_inv_transform(LAStreamOut& f, const Vector& F)
{
  input_pad0(F);

  for(register const Complex * ap = A; ap < A + N/2; )
    f.get() = - std::imag(*ap++) * 4/N/dr;	// 2/pi * dk = 2/pi * 2pi/N/dr
  if( !f.eof() )
    _error("The output stream was supposed to end after all %d points of the inv-sin-tr "
	   "were placed into it",N/2);
}

void FFT::cos_inv_transform(LAStreamOut& f, const Vector& F)
{
  input_pad0(F);

  for(register const Complex * ap = A; ap < A + N/2; )
    f.get() = std::real(*ap++) * 4/N/dr;
  if( !f.eof() )
    _error("The output stream was supposed to end after all %d points of the inv-cos-tr "
	   "were placed into it",N/2);
}
