// This may look like C code, but it is really -*- C++ -*-

/* 
Copyright (C) 1988, 1992 Free Software Foundation
    written by Doug Lea (dl@rocky.oswego.edu)

This file is part of the GNU C++ Library.  This library is free
software; you can redistribute it and/or modify it under the terms of
the GNU Library General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.  This library is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
  arithmetic, etc. functions on built in types
*/


#ifndef _builtin_h
#ifdef __GNUG__
#pragma interface
#endif
#define _builtin_h 1

#include <stddef.h>
#include "std.h"
#include <math.h>

#ifdef __GNUG__
#include <cmath>
#endif

#ifdef __GNUG__
#define _VOLATILE_VOID volatile void
#else
#define _VOLATILE_VOID void
#endif

long         gcd(long, long);
long         lg(unsigned long); 

				// Otherwise, see myenv.h
#if defined(__GNUC__) && (__GNUC__ == 2) && (__GNUC_MINOR__ < 8)
extern "C" double       start_timer();
extern "C" double       return_elapsed_time(double last_time = 0.0);
#endif

#if defined(linux)
static inline div_t div(int numer, int denom)
{ const div_t res = { numer/denom, numer%denom}; return res;}
#endif

char*        dtoa(double x, char cvt = 'g', int width = 0, int prec = 6);

unsigned int hashpjw(const char*);
unsigned int multiplicativehash(int);
unsigned int foldhash(double);


#if !defined(IV)

#ifndef __GNUG__		// has been defined in <cmath>
inline double abs(double arg) 
{
  return (arg < 0.0)? -arg : arg;
}

inline float abs(float arg) 
{
  return (arg < 0.0)? -arg : arg;
}
#endif

inline short abs(short arg) 
{
  return (arg < 0)? -arg : arg;
}

#ifndef __GNUG__		// has been defined in <cstdlib>
inline long abs(long arg) 
{
  return (arg < 0)? -arg : arg;
}
#endif

inline int sign(long arg)
{
  return (arg == 0) ? 0 : ( (arg > 0) ? 1 : -1 );
}

inline int sign(double arg)
{
  return (arg == 0.0) ? 0 : ( (arg > 0.0) ? 1 : -1 );
}

static inline long sqr(long arg)
{
  return arg * arg;
}

#if ! _G_MATH_H_INLINES /* hpux and SCO define this in math.h */
static inline double sqr(double arg)
{
  return arg * arg;
}
#endif

inline int even(long arg)
{
  return !(arg & 1);
}

inline int odd(long arg)
{
  return (arg & 1);
}

inline long lcm(long x, long y)
{
  return x / gcd(x, y) * y;
}

inline void (setbit)(long& x, long b)
{
  x |= (1 << b);
}

inline void clearbit(long& x, long b)
{
  x &= ~(1 << b);
}

inline int testbit(long x, long b)
{
  return ((x & (1 << b)) != 0);
}

#if !defined(M_PI)
#  define M_PI          3.14159265358979323846
#endif

#endif
#endif
