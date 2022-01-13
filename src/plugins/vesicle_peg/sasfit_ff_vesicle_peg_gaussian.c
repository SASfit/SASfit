/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>
# include <float.h>
# include <math.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

/******************************************************************************/

void zero_rc ( double a, double b, double t, double *arg, int *status,
  double value )

/******************************************************************************/
/*
  Purpose:

    zero_rc() seeks a root of a function F(X) using reverse communication.

  Discussion:

    The interval [A,B] must be a change of sign interval for F.
    That is, F(A) and F(B) must be of opposite signs.  Then
    assuming that F is continuous implies the existence of at least
    one value C between A and B for which F(C) = 0.

    The location of the zero is determined to within an accuracy
    of 4 * EPSILON * abs ( C ) + 2 * T.

    The routine is a revised version of the Brent zero finder
    algorithm, using reverse communication.

    Thanks to Thomas Secretin for pointing out a transcription error in the
    setting of the value of P, 11 February 2013.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    13 July 2021

  Author:

    John Burkardt

  Reference:

    Richard Brent,
    Algorithms for Minimization Without Derivatives,
    Dover, 2002,
    ISBN: 0-486-41998-3,
    LC: QA402.5.B74.

  Input:

    double A, B, the endpoints of the change of sign interval.

    double T, a positive error tolerance.

    int *STATUS, used to communicate between the user and the routine.
    The user only sets STATUS to zero on the first call, to indicate
    that this is a startup call.

    double VALUE, the function value at ARG, as requested
    by the routine on the previous call.  No value needs to be input
    on the first call.

  Output:

    double *ARG, the currently considered point.  For the next call,
    the user is requested to evaluate the function at ARG, and return
    the value in VALUE.  On return with STATUS zero, ARG is the routine's
    estimate for the function's zero.

    int *STATUS, used to communicate between the user and the routine.
    The routine returns STATUS positive to request that the function be
    evaluated at ARG, or returns STATUS as 0, to indicate that the
    iteration is complete and that ARG is the estimated zero.
*/
{
  static double c;
  static double d;
  static double e;
  static double fa;
  static double fb;
  static double fc;
  double m;
  double p;
  double q;
  double r;
  double s;
  static double sa;
  static double sb;
  double tol;
/*
  Input STATUS = 0.
  Initialize, request F(A).
*/
  if ( *status == 0 )
  {
    sa = a;
    sb = b;
    e = sb - sa;
    d = e;

    *status = 1;
    *arg = a;
    return;
  }
/*
  Input STATUS = 1.
  Receive F(A), request F(B).
*/
  else if ( *status == 1 )
  {
    fa = value;
    *status = 2;
    *arg = sb;
    return;
  }
/*
  Input STATUS = 2
  Receive F(B).
*/
  else if ( *status == 2 )
  {
    fb = value;

    if ( 0.0 < fa * fb )
    {
      *status = -1;
      return;
    }
    c = sa;
    fc = fa;
  }
  else
  {
    fb = value;

    if ( ( 0.0 < fb && 0.0 < fc ) || ( fb <= 0.0 && fc <= 0.0 ) )
    {
      c = sa;
      fc = fa;
      e = sb - sa;
      d = e;
    }
  }
/*
  Compute the next point at which a function value is requested.
*/
  if ( fabs ( fc ) < fabs ( fb ) )
  {
    sa = sb;
    sb = c;
    c = sa;
    fa = fb;
    fb = fc;
    fc = fa;
  }

  tol = 2.0 * DBL_EPSILON * fabs ( sb ) + t;
  m = 0.5 * ( c - sb );

  if ( fabs ( m ) <= tol || fb == 0.0 )
  {
    *status = 0;
    *arg = sb;
    return;
  }

  if ( fabs ( e ) < tol || fabs ( fa ) <= fabs ( fb ) )
  {
    e = m;
    d = e;
  }
  else
  {
    s = fb / fa;

    if ( sa == c )
    {
      p = 2.0 * m * s;
      q = 1.0 - s;
    }
    else
    {
      q = fa / fc;
      r = fb / fc;
      p = s * ( 2.0 * m * q * ( q - r ) - ( sb - sa ) * ( r - 1.0 ) );
      q = ( q - 1.0 ) * ( r - 1.0 ) * ( s - 1.0 );
    }

    if ( 0.0 < p )
    {
      q = - q;
    }
    else
    {
      p = - p;
    }
    s = e;
    e = d;

    if ( 2.0 * p < 3.0 * m * q - fabs ( tol * q ) &&
         p < fabs ( 0.5 * s * q ) )
    {
      d = p / q;
    }
    else
    {
      e = m;
      d = e;
    }
  }

  sa = sb;
  fa = fb;

  if ( tol < fabs ( d ) )
  {
    sb = sb + d;
  }
  else if ( 0.0 < m )
  {
    sb = sb + tol;
  }
  else
  {
    sb = sb - tol;
  }

  *arg = sb;
  *status = *status + 1;

  return;
}

double root_rc ( double x, double fx, double *ferr, double *xerr, double q[9], int *ierr, char *Buffer )

/******************************************************************************/
/*
  Purpose:

    ROOT_RC solves a single nonlinear equation using reverse communication.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    22 January 2013

  Author:

    Original FORTRAN77 version by Gaston Gonnet.
    C version by John Burkardt.

  Reference:

    Gaston Gonnet,
    On the Structure of Zero Finders,
    BIT Numerical Mathematics,
    Volume 17, Number 2, June 1977, pages 170-183.

  Parameters:

    Input, double X, an estimate for the root.  On the first
    call, this must be a value chosen by the user.  Thereafter, it may
    be a value chosen by the user, or the value of ROOT returned on the
    previous call to the function.

    Input, double FX, the value of the function at X.

    Output, double *FERR, the smallest value of F encountered.

    Output, double *XERR, the width of the change-in-sign interval,
    if one was encountered.

    Input/output, double Q[9], storage needed by the function.
    Before the first call, the user must set Q(1) to 0.

    Output, double ROOT_RC, an improved estimate for the root.
*/
{
  double d;
  double decr;
  int i;
  double p;
  double r;
  double u;
  double v;
  double w;
  double xnew;
  double z;
  *ierr =0;
/*
  If we found an exact zero, there is nothing more to do.
*/
  if ( fx == 0.0 )
  {
    *ferr = 0.0;
    *xerr = 0.0;
    xnew = x;
    return xnew;
  }

  *ferr = fabs ( fx );
/*
  If this is the first time, initialize, estimate the first root, and exit.
*/
  if ( q[0] == 0.0 )
  {
    q[0] = fx;
    q[1] = x;
    for ( i = 2; i < 9; i++ )
    {
      q[i] = 0.0;
    }
    xnew = x + 20*x;
    *xerr = DBL_MAX;
    return xnew;
  }
/*
  This is not the first call.
*/
  q[8] = q[8] + 1.0;
/*
  Check for too many iterations.
*/
  if ( 80.0 < q[8] )
  {
    *ierr = 1;
	sprintf(Buffer,"\nROOT_RC - Fatal error!\n  Number of iterations = %d\n",( int ) q[8]);
	return x;
    printf ( "\n" );
    printf ( "ROOT_RC - Fatal error!\n" );
    printf ( "  Number of iterations = %d\n",( int ) q[8] );
    exit ( 1 );
  }
/*
  Check for a repeated X value.
*/
  if ( ( 2.0 <= q[8] && x == q[3] ) || x == q[1] )
  {
    *ierr = 2;
	sprintf(Buffer,"\nROOT_RC - Fatal error!\n   Value of X has been input before.\n");
	return x;
    printf ( "\n" );
    printf ( "ROOT_RC - Fatal error!\n" );
    printf ( "  Value of X has been input before.\n" );
    exit ( 1 );
  }
/*
  Push X -> A -> B -> C
*/
  for ( i = 5; 2 <= i; i-- )
  {
    q[i] = q[i-2];
  }
  q[0] = fx;
  q[1] = x;
/*
  If we have a change-in-sign interval, store the opposite value.
*/
  if ( GSL_SIGN ( q[0] ) != GSL_SIGN ( q[2] ) )
  {
    q[6] = q[2];
    q[7] = q[3];
  }
/*
  Calculate XERR.
*/
  if ( q[6] != 0.0 )
  {
    *xerr = fabs ( q[7] - q[1] );
  }
  else
  {
    *xerr = DBL_MAX;
  }
/*
  If more than 30 iterations, and we have change-in-sign interval, bisect.
*/
  if ( 30.0 < q[8] && q[6] != 0.0 )
  {
    xnew = q[1] + ( q[7] - q[1] ) / 2.0;
    return xnew;
  }

  v = ( q[2] - q[0] ) / ( q[3] - q[1] );
/*
  If 3 or more points, try Muller.
*/
  if ( q[4] != 0.0 )
  {
    u = ( q[4] - q[2] ) / ( q[5] - q[3] );
    w = q[3] - q[1];
    z = ( q[5] - q[1] ) / w;
    r = ( z + 1.0 ) * v - u;

    if ( r != 0.0 )
    {
      p = 2.0 * z * q[0] / r;
      d = 2.0 * p / ( w * r ) * ( v - u );
      if ( -1.0 <= d )
      {
        xnew = q[1] - p / ( 1.0 + sqrt ( 1.0 + d ) );
        if ( q[6] == 0.0 ||
             ( q[1] < xnew && xnew < q[7] ) ||
             ( q[7] < xnew && xnew < q[1] ) )
        {
          return xnew;
        }
      }
    }
  }
/*
  Try the secant step.
*/
  if ( q[0] != q[2] || q[6] == 0.0 )
  {
    if ( q[0] == q[2] )
    {
      *ierr = 3;
	  sprintf(Buffer,"\nROOT_RC - Fatal error!\n  Cannot apply any method.\n");
	  return x;
      printf ( "\n" );
      printf ( "ROOT_RC - Fatal error!\n" );
      printf ( "  Cannot apply any method.\n" );
      exit ( 1 );
    }
    decr = q[0] / v;
    if ( fabs ( decr ) * 4.6E+18 < fabs ( q[1] ) )
    {
      decr = 1.74E-18 * fabs ( q[1] ) * GSL_SIGN ( decr );
    }
    xnew = q[1] - decr;
    if ( q[6] == 0.0 ||
        ( q[1] < xnew && xnew < q[7] ) ||
        ( q[7] < xnew && xnew < q[1] ) )
    {
      return xnew;
    }
  }
/*
  Apply bisection.
*/
  xnew = q[1] + ( q[7] - q[1] ) / 2.0;

  return xnew;
}
/******************************************************************************/


// define shortcuts for local parameters/variables
#define R	param->p[0]
#define SIGMA_R	fabs(param->p[1])
#define T	param->p[2]
#define SIGMA_T	fabs(param->p[3])
#define SIGMA_IH	fabs(param->p[4])
#define SIGMA_OH	fabs(param->p[5])
#define RG	param->p[6]
#define F_PEG_I	param->p[7]
#define F_PEG_O	param->p[8]
#define V_PEG	param->p[9]
#define ETA_PEG	param->p[10]
#define V_HEAD	param->p[11]
#define ETA_HEAD	param->p[12]
#define V_TAIL	param->p[13]
#define ETA_TAIL	param->p[14]
#define ETA_SOL	param->p[15]

#define Q   param->p[MAXPAR-1]
#define HO   param->p[MAXPAR-2]
#define HI   param->p[MAXPAR-3]
#define RINT param->p[MAXPAR-4]
#define TINT param->p[MAXPAR-5]
#define RIT   (RINT-TINT/2.)
#define ROT   (RINT+TINT/2.)
#define RIH   (RINT-TINT/2.-HI)
#define ROH   (RINT+TINT/2.+HO)

scalar Vbilayer(scalar Ri, scalar Ro, scalar sigmai, scalar sigmao, sasfit_param *param)
{
    if (sigmai==sigmao) {
        return (M_PI*(exp(-(Ri + Ro)/sigmai)*3*gsl_pow_int(sigmai,3) - 48*exp(Ro/sigmai-(Ri + Ro)/sigmai)*gsl_pow_int(sigmai,3) -
            16*exp((Ri + Ro)/sigmai-(Ri + Ro)/sigmai)*(Ri - Ro)*
            (gsl_pow_int(Ri,2) + Ri*Ro + gsl_pow_int(Ro,2) + 6*gsl_pow_int(sigmai,2)) +
            exp((2*Ri)/sigmai-(Ri + Ro)/sigmai)*(-4*gsl_pow_int(Ri,3) + 4*gsl_pow_int(Ro,3) +
            18*(gsl_pow_int(Ri,2) + gsl_pow_int(Ro,2))*sigmai +
            42*(-Ri + Ro)*gsl_pow_int(sigmai,2) + 90*gsl_pow_int(sigmai,3))))/
            (12.);
    }
  return -(M_PI*
     (6*exp(Ro/sigmai+(-((Ri + Ro)/sigmai) - Ro/sigmao))*gsl_pow_int(sigmai,3)*gsl_pow_int(sigmao,3)*
        gsl_pow_int(-sigmai + sigmao,3) +
       12*exp(Ro*(1/sigmai + 1/sigmao)+(-((Ri + Ro)/sigmai) - Ro/sigmao))*
        gsl_pow_int(gsl_pow_int(sigmai,3) - sigmai*gsl_pow_int(sigmao,2),3) -
       6*exp((2*Ri)/sigmai + Ro/sigmao+(-((Ri + Ro)/sigmai) - Ro/sigmao))*gsl_pow_int(sigmai,3)*
        (gsl_pow_int(sigmai,4)*(gsl_pow_int(Ro,2) + 2*Ro*sigmai + 2*gsl_pow_int(sigmai,2)) -
          2*gsl_pow_int(sigmai,2)*(Ro + sigmai)*(Ro + 3*sigmai)*gsl_pow_int(sigmao,2) +
          (gsl_pow_int(Ro,2) + 6*Ro*sigmai + 12*gsl_pow_int(sigmai,2))*gsl_pow_int(sigmao,4)) +
       4*exp((Ri + Ro)/sigmai + Ro/sigmao+(-((Ri + Ro)/sigmai) - Ro/sigmao))*gsl_pow_int(sigmai - sigmao,3)*
        gsl_pow_int(sigmai + sigmao,3)*
        (gsl_pow_int(Ri,3) + 6*Ri*gsl_pow_int(sigmai,2) -
          Ro*(gsl_pow_int(Ro,2) + 6*gsl_pow_int(sigmao,2))) +
       6*exp((Ri + Ro)/sigmai + Ri/sigmao+(-((Ri + Ro)/sigmai) - Ro/sigmao))*gsl_pow_int(sigmao,3)*
        (gsl_pow_int(Ri,2)*gsl_pow_int(gsl_pow_int(sigmai,2) - gsl_pow_int(sigmao,2),2) -
          2*Ri*(3*gsl_pow_int(sigmai,4)*sigmao -
             4*gsl_pow_int(sigmai,2)*gsl_pow_int(sigmao,3) + gsl_pow_int(sigmao,5)) +
          2*(6*gsl_pow_int(sigmai,4)*gsl_pow_int(sigmao,2) -
             3*gsl_pow_int(sigmai,2)*gsl_pow_int(sigmao,4) + gsl_pow_int(sigmao,6)))))
             /(3*gsl_pow_int((sigmai - sigmao)*(sigmai + sigmao),3));
}

scalar Fbilayer(scalar q, scalar Ri, scalar Ro, scalar sigmai, scalar sigmao, sasfit_param *param)
{
    scalar q2, q3, q4, q2sigmai2Pone, q2sigmao2Pone, sigmaiMsigmao, sigmaiPsigmao,
           sigmai2, sigmai3, sigmai4, sigmai5, sigmai6, sigmai7, sigmai8, sigmai9,
           sigmao2, sigmao3, sigmao4, sigmao5, sigmao6, sigmao7, sigmao8, sigmao9;
    q2=gsl_pow_int(q,2);
    q3=q*q2;
    q4=q*q3;
    sigmai2=gsl_pow_int(sigmai,2);
    sigmai3=sigmai2*sigmai;
    sigmai4=sigmai3*sigmai;
    sigmai5=sigmai4*sigmai;
    sigmai6=sigmai5*sigmai;
    sigmai7=sigmai6*sigmai;
    sigmai8=sigmai7*sigmai;
    sigmai9=sigmai8*sigmai;

    sigmao2=gsl_pow_int(sigmao,2);
    sigmao3=sigmao2*sigmao;
    sigmao4=sigmao3*sigmao;
    sigmao5=sigmao4*sigmao;
    sigmao6=sigmao5*sigmao;
    sigmao7=sigmao6*sigmao;
    sigmao8=sigmao7*sigmao;
    sigmao9=sigmao8*sigmao;

    q2sigmai2Pone = 1 + q2*sigmai2;
    q2sigmao2Pone = 1 + q2*sigmao2;
    sigmaiMsigmao = sigmai*sigmao;
    sigmaiPsigmao = sigmai+sigmao;

    if (q*(Ro+sigmao*5) < 1e-1) {
        if (sigmai=sigmao) {
            return (M_PI*(-exp(-(Ri + Ro)/sigmao)*15*sigmao3*(-2 + q2*sigmao2)

		+

       480*exp(Ro/sigmao-(Ri + Ro)/sigmao)*sigmao3*
        (-1 + 2*q2*sigmao2)

		+

       16*
        (-10*gsl_pow_int(Ri,3) + q2*gsl_pow_int(Ri,5) + 10*gsl_pow_int(Ro,3) -
          q2*gsl_pow_int(Ro,5) +
          20*(-3*Ri + q2*gsl_pow_int(Ri,3) + 3*Ro - q2*gsl_pow_int(Ro,3))*
           sigmao2 + 120*q2*(Ri - Ro)*sigmao4)

		   +

       2*exp((2*Ri)/sigmao-(Ri + Ro)/sigmao)*
        (-20*gsl_pow_int(Ri,3) + 2*q2*gsl_pow_int(Ri,5) + 20*gsl_pow_int(Ro,3) -
          2*q2*gsl_pow_int(Ro,5) -
          15*(-6*gsl_pow_int(Ri,2) + q2*gsl_pow_int(Ri,4) - 6*gsl_pow_int(Ro,2) +
             q2*gsl_pow_int(Ro,4))*sigmao +
          70*(-3*Ri + q2*gsl_pow_int(Ri,3) + 3*Ro - q2*gsl_pow_int(Ro,3))*
           sigmao2 - 225*
           (-2 + q2*(gsl_pow_int(Ri,2) + gsl_pow_int(Ro,2)))*sigmao3 +
          465*q2*(Ri - Ro)*sigmao4 -
          945*q2*sigmao5)))/(120.);
        }
        return (M_PI*(60*exp((2*Ro*(sigmai + sigmao))/(sigmai*sigmao)-(Ri*sigmao + 2*Ro*(sigmai + sigmao))/(sigmai*sigmao))*sigmai3*
        (-1 + 2*q2*sigmai2)*
        gsl_pow_int(sigmai2 - sigmao2,5)

		-

       30*exp(Ro*(2/sigmai + 1/sigmao)-(Ri*sigmao + 2*Ro*(sigmai + sigmao))/(sigmai*sigmao))*sigmai3*
        gsl_pow_int(sigmai - sigmao,5)*sigmao3*
        (-2*sigmai*sigmao - sigmao2 +
          sigmai2*(-1 + 2*q2*sigmao2))

		+

       2*exp((Ri + 2*Ro)/sigmai + (2*Ro)/sigmao-(Ri*sigmao + 2*Ro*(sigmai + sigmao))/(sigmai*sigmao))*gsl_pow_int(sigmai - sigmao,5)*
        gsl_pow_int(sigmai + sigmao,5)*
        (q2*gsl_pow_int(Ri,5) + 10*gsl_pow_int(Ro,3) +
          10*gsl_pow_int(Ri,3)*(-1 + 2*q2*sigmai2) +
          60*Ri*sigmai2*(-1 + 2*q2*sigmai2) +
          60*Ro*sigmao2 -
          q2*(gsl_pow_int(Ro,5) + 20*gsl_pow_int(Ro,3)*sigmao2 +
             120*Ro*sigmao4))

		+

       5*exp((Ri + 2*Ro)/sigmai + (Ri + Ro)/sigmao-(Ri*sigmao + 2*Ro*(sigmai + sigmao))/(sigmai*sigmao))*sigmao3*
        (q2*gsl_pow_int(Ri,4)*gsl_pow_int(sigmai2 - sigmao2,4) -
          4*q2*gsl_pow_int(Ri,3)*sigmao*
           (-3*sigmai2 + sigmao2)*
           gsl_pow_int(-sigmai2 + sigmao2,3) +
          6*gsl_pow_int(Ri,2)*gsl_pow_int(sigmai - sigmao,2)*gsl_pow_int(sigmai + sigmao,2)*
           (-sigmai4 + 2*sigmai2*
              (1 + 6*q2*sigmai2)*sigmao2 -
             (1 + 6*q2*sigmai2)*sigmao4 +
             2*q2*sigmao6) +
          12*sigmao2*(5*sigmai2*sigmao6*
              (1 - 2*q2*sigmao2) +
             5*sigmai6*sigmao2*
              (3 - 2*q2*sigmao2) +
             sigmao8*(-1 + 2*q2*sigmao2) +
             6*sigmai8*(-1 + 5*q2*sigmao2) +
             sigmai4*sigmao4*
              (-13 + 20*q2*sigmao2)) -
          12*Ri*(sigmai - sigmao)*sigmao*(sigmai + sigmao)*
           (sigmao6 - 2*q2*sigmao8 +
             sigmai4*sigmao2*
              (7 - 10*q2*sigmao2) +
             sigmai2*sigmao4*
              (-5 + 8*q2*sigmao2) +
             sigmai6*(-3 + 20*q2*sigmao2)))

		-

       5*exp((2*Ri + Ro)/sigmai + (2*Ro)/sigmao-(Ri*sigmao + 2*Ro*(sigmai + sigmao))/(sigmai*sigmao))*sigmai3*
        (4*q2*gsl_pow_int(Ro,3)*sigmai*
           (sigmai2 - 3*sigmao2)*
           gsl_pow_int(sigmai2 - sigmao2,3) +
          q2*gsl_pow_int(Ro,4)*gsl_pow_int(sigmai2 - sigmao2,4) +
          12*Ro*sigmai*(sigmai - sigmao)*(sigmai + sigmao)*
           (2*q2*sigmai8 + 3*sigmao6 -
             sigmai6*(1 + 8*q2*sigmao2) -
             sigmai2*sigmao4*
              (7 + 20*q2*sigmao2) +
             5*sigmai4*(sigmao2 +
                2*q2*sigmao4)) +
          6*gsl_pow_int(Ro,2)*gsl_pow_int(sigmai2 - sigmao2,2)*
           (2*q2*sigmai6 - sigmao4 -
             sigmai4*(1 + 6*q2*sigmao2) +
             2*sigmai2*(sigmao2 +
                6*q2*sigmao4)) +
          12*sigmai2*(2*q2*gsl_pow_int(sigmai,10) -
             6*sigmao8 -
             sigmai8*(1 + 10*q2*sigmao2) -
             sigmai4*sigmao4*
              (13 + 10*q2*sigmao2) +
             5*sigmai6*(sigmao2 +
                4*q2*sigmao4) +
             15*sigmai2*
              (sigmao6 + 2*q2*sigmao8)))))/
   (15.*gsl_pow_int(sigmai - sigmao,5)*gsl_pow_int(sigmai + sigmao,5));

        return Vbilayer(Ri, Ro, sigmai, sigmao, param);
    }

    return (


  2*M_PI*q3*sigmai3*gsl_pow_int(q2sigmao2Pone,2)*
      gsl_pow_int(-2*sigmai*sigmao + sigmao2 +
        sigmai2*q2sigmao2Pone,2)*
      (exp(Ro*(2/sigmai + 1/sigmao)-(Ri*sigmao + 2*Ro*sigmaiPsigmao)/sigmaiMsigmao)*
         gsl_pow_int(q2sigmai2Pone,2)*sigmao3*
         sigmaiPsigmao
		 -
		 2*exp(
          (2*Ro*sigmaiPsigmao)/sigmaiMsigmao-(Ri*sigmao + 2*Ro*sigmaiPsigmao)/sigmaiMsigmao)*
         gsl_pow_int(2*sigmai*sigmao + sigmao2 +
           sigmai2*q2sigmao2Pone,2)
	  )

		   +

     2*M_PI*(

	 q*q2sigmai2Pone*
         (2*exp((2*Ro*sigmaiPsigmao)/sigmaiMsigmao+Ri/sigmai-(Ri*sigmao + 2*Ro*sigmaiPsigmao)/sigmaiMsigmao)*Ri*
            gsl_pow_int(q2sigmao2Pone,2)*
            gsl_pow_int(-2*sigmai*sigmao + sigmao2 +
              sigmai2*q2sigmao2Pone,2)*
            gsl_pow_int(2*sigmai*sigmao + sigmao2 +
              sigmai2*q2sigmao2Pone,2)
			  -
           exp((2*Ro)/sigmai + (Ri + Ro)/sigmao+Ri/sigmai-(Ri*sigmao + 2*Ro*sigmaiPsigmao)/sigmaiMsigmao)*q2*
            q2sigmai2Pone*sigmao4*
            (-2*sigmao7 -
              12*sigmai6*sigmao*(-1 +q2*sigmao2)*
               gsl_pow_int(q2sigmao2Pone,2) -
              2*sigmai4*sigmao3*
               (9 + 2*q2*sigmao2 +
                 9*q4*sigmao4) +
              sigmai2*(8*sigmao5 -
                 8*q2*sigmao7) +
              Ri*q2sigmao2Pone*
               (sigmao2 +
                 sigmai2*(-3 +q2*sigmao2))*
               (-2*sigmai*sigmao + sigmao2 +
                 sigmai2*q2sigmao2Pone)*
               (2*sigmai*sigmao + sigmao2 +
                 sigmai2*q2sigmao2Pone)))*cos(q*Ri)


          -


		  (2*exp((2*Ro*sigmaiPsigmao)/sigmaiMsigmao+Ri/sigmai-(Ri*sigmao + 2*Ro*sigmaiPsigmao)/sigmaiMsigmao)*
            (1 + 3*q2*sigmai2)*
            gsl_pow_int(q2sigmao2Pone,2)*
            gsl_pow_int(-2*sigmai*sigmao + sigmao2 +
              sigmai2*q2sigmao2Pone,2)*
            gsl_pow_int(2*sigmai*sigmao + sigmao2 +
              sigmai2*q2sigmao2Pone,2)
			  -
           exp((2*Ro)/sigmai + (Ri + Ro)/sigmao+Ri/sigmai-(Ri*sigmao + 2*Ro*sigmaiPsigmao)/sigmaiMsigmao)*
            gsl_pow_int(q +q3*sigmai2,2)*sigmao3*
            (-sigmao7 +q2*sigmao9 +
              3*sigmai6*sigmao*
               gsl_pow_int(q2sigmao2Pone,2)*
               (1 - 6*q2*sigmao2 + q4*sigmao4)
                + sigmai2*sigmao5*
               (5 - 6*q2*sigmao2 +
                 5*q4*sigmao4) +
              sigmai4*sigmao3*
               (-7 + 9*q2*sigmao2 -
                 9*q4*sigmao4 + 7*gsl_pow_int(q,6)*sigmao6)\
               + Ri*q2sigmao2Pone*
               (-2*sigmai*sigmao + sigmao2 +
                 sigmai2*q2sigmao2Pone)*
               (2*sigmai*sigmao + sigmao2 +
                 sigmai2*q2sigmao2Pone)*
               (sigmao2 +
                 sigmai2*(-1 + 3*q2*sigmao2))))*
         sin(q*Ri)


		 +


         (-(q*q2sigmao2Pone*
              (2*exp(Ro/sigmai+Ro*(1/sigmai + 2/sigmao)+Ri/sigmai-(Ri*sigmao + 2*Ro*sigmaiPsigmao)/sigmaiMsigmao)*Ro*
                 gsl_pow_int(sigmai4 +q2*sigmai6 +
                   2*sigmai2*(-1 + q4*sigmai4)*
                    sigmao2 +
                   gsl_pow_int(q2sigmai2Pone,3)*sigmao4,2)\
                 -
				 exp(Ri/sigmai+Ro*(1/sigmai + 2/sigmao)+Ri/sigmai-(Ri*sigmao + 2*Ro*sigmaiPsigmao)/sigmaiMsigmao)*q2*sigmai4*
                 q2sigmao2Pone*
                 (Ro*q2sigmai2Pone*
                    (sigmai2 - 2*sigmai*sigmao + sigmao2 +
                     q2*sigmai2*sigmao2)*
                    (sigmai2 + 2*sigmai*sigmao + sigmao2 +
                     q2*sigmai2*sigmao2)*
                    (sigmai2 +
                      (-3 +q2*sigmai2)*sigmao2) +
                   2*sigmai*(sigmai6 +
                      4*sigmai4*(-1 +q2*sigmai2)*
                       sigmao2 +
                      sigmai2*
                       (9 + 2*q2*sigmai2 +
                         9*q4*sigmai4)*sigmao4 +
                      6*(-1 +q2*sigmai2)*
                       gsl_pow_int(q2sigmai2Pone,2)*sigmao6
                      )))*cos(q*Ro))

					  +

           (2*exp(Ro/sigmai+Ro*(1/sigmai + 2/sigmao)+Ri/sigmai-(Ri*sigmao + 2*Ro*sigmaiPsigmao)/sigmaiMsigmao)*(1 + 3*q2*sigmao2)*
               gsl_pow_int(sigmai4 +q2*sigmai6 +
                 2*sigmai2*(-1 + q4*sigmai4)*
                  sigmao2 +
                 gsl_pow_int(q2sigmai2Pone,3)*sigmao4,2)
				 +
              exp(Ri/sigmai+Ro*(1/sigmai + 2/sigmao)+Ri/sigmai-(Ri*sigmao + 2*Ro*sigmaiPsigmao)/sigmaiMsigmao)*sigmai3*
               gsl_pow_int(q +q3*sigmao2,2)*
               (sigmai7 -q2*sigmai9 +
                 sigmai5*
                  (-5 + 6*q2*sigmai2 -
                    5*q4*sigmai4)*sigmao2 +
                 sigmai3*
                  (7 - 9*q2*sigmai2 +
                    9*q4*sigmai4 -
                    7*gsl_pow_int(q,6)*sigmai6)*sigmao4 -
                 3*sigmai*gsl_pow_int(q2sigmai2Pone,2)*
                  (1 - 6*q2*sigmai2 +
                    q4*sigmai4)*sigmao6 +
                 Ro*q2sigmai2Pone*
                  (sigmai2 - 2*sigmai*sigmao + sigmao2 +
                   q2*sigmai2*sigmao2)*
                  (sigmai2 + 2*sigmai*sigmao + sigmao2 +
                   q2*sigmai2*sigmao2)*
                  (sigmai2 +
                    (-1 + 3*q2*sigmai2)*sigmao2)))*
            sin(q*Ro))))/
   (q3*
     gsl_pow_int(q2sigmai2Pone,2)*
     gsl_pow_int(q2sigmao2Pone,2)*
     gsl_pow_int(-2*sigmai*sigmao + sigmao2 +
       sigmai2*q2sigmao2Pone,2)*
     gsl_pow_int(2*sigmai*sigmao + sigmao2 +
       sigmai2*q2sigmao2Pone,2));
}



scalar vesicle_pegulated_gauss(const double* v, size_t n, void* p)
{
    scalar Phi, ni, no, N, Ri, Ro, Vtail,Vhead, Vho,Vhi,tmp, Ichain,Iii,Ioo,Iio,Ivi,Ivo;
    sasfit_param * param;
    double qrc[9];
    char sBuffer[256];
    int ierr,i;
    double ferr, xerr;
    param = (sasfit_param *) p;
    SASFIT_CHECK_COND1((n != 2), param, "n(%d) !=2 0",n);

    RINT = v[0];
    TINT = v[1];

    Vtail = Vbilayer(RIT,ROT,SIGMA_IH,SIGMA_OH,param);
	no = 0.5*Vtail/V_TAIL;
	ni = no;
	Vho = no*V_HEAD;
	Vhi = ni*V_HEAD;
// need to find HI and HO so that profile is consistent with Vho and Vhi
// initial guess of HI
    HI = TINT*V_HEAD/V_TAIL/20.0;
    for ( i = 0; i < 9; i++ )
    {
        qrc[i] = 0.0;
    }
    do {
        Vhead = Vbilayer(RIH,RIT,SIGMA_IH,SIGMA_IH,param);
        HI=root_rc(HI, -(Vhi-Vhead), &ferr, &xerr, qrc, &ierr, sBuffer);
//        sasfit_out("Vhead:%lg, Vhi: %lg,  HI:%lg, V_HEAD:%lg V_TAIL:%lg\n",Vhead,Vhi,HI,V_HEAD,V_TAIL);
    } while (fabs(xerr/HI)>1e-6&&ierr==0);
    if (ierr>0) {
        sasfit_err("%s\n",sBuffer);
        return 0;
    }
// initial guess of HO
    HO = TINT*V_HEAD/V_TAIL/20.;
    for ( i = 0; i < 9; i++ )
    {
        qrc[i] = 0.0;
    }
    do {
        Vhead = Vbilayer(ROT,ROH,SIGMA_OH,SIGMA_OH,param);
        HO=root_rc(HO, -(Vho-Vhead), &ferr, &xerr, qrc, &ierr, sBuffer);
//        sasfit_out("Vhead:%lg, Vho: %lg, HO:%lg\n",Vhead,Vho,HO);
    } while (fabs(xerr/HO)>1e-6&&ierr==0);
    if (ierr>0) {
        sasfit_err("%s\n",sBuffer);
        return 0;
    }
	no=no*F_PEG_O;
	ni=ni*F_PEG_I;
    Phi =(ETA_HEAD-ETA_SOL)*Fbilayer(Q,RIH,RIT,SIGMA_IH,SIGMA_IH,param)
        +(ETA_TAIL-ETA_SOL)*Fbilayer(Q,RIT,ROT,SIGMA_IH,SIGMA_OH,param)
        +(ETA_HEAD-ETA_SOL)*Fbilayer(Q,ROT,ROH,SIGMA_OH,SIGMA_OH,param);
//    sasfit_out("PHI:%lg\n",Phi);
	N = (ni+no);
	Ichain = N      *gsl_pow_2((ETA_PEG-ETA_SOL)*V_PEG)*sasfit_gauss_fc(Q,RG);
	Ioo =  no*(no-1)*gsl_pow_2((ETA_PEG-ETA_SOL)*V_PEG*sasfit_rwbrush_w(Q,RG) *gsl_sf_sinc(Q*(ROH+RG)/M_PI));
	Iii =  ni*(ni-1)*gsl_pow_2((ETA_PEG-ETA_SOL)*V_PEG*sasfit_rwbrush_w(Q,RG) *gsl_sf_sinc(Q*(RIH-RG)/M_PI));
	Iio = 2*ni*no   *gsl_pow_2((ETA_PEG-ETA_SOL)*V_PEG*sasfit_rwbrush_w(Q,RG))*gsl_sf_sinc(Q*(ROH+RG)/M_PI)
	                                                                          *gsl_sf_sinc(Q*(RIH-RG)/M_PI);
	Ivi = 2*ni*(ETA_PEG-ETA_SOL)*V_PEG*sasfit_rwbrush_w(Q,RG)*Phi*gsl_sf_sinc(Q*(RIH-RG)/M_PI);
	Ivo = 2*no*(ETA_PEG-ETA_SOL)*V_PEG*sasfit_rwbrush_w(Q,RG)*Phi*gsl_sf_sinc(Q*(ROH+RG)/M_PI);
	return Phi*Phi+Ichain+Iii+Ioo+Iio+Ivi+Ivo;
}

scalar vesicle_pegulated_gauss_kernel(const double* x, size_t n, void* p)
{
    sasfit_param * param;
	scalar LNdistrR,LNdistrT;
	sasfit_param subParam;

    param = (sasfit_param *) p;
    SASFIT_CHECK_COND1((n != 2), param, "n(%d) !=2 0",n);

	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_T;
	subParam.p[2] = 1.0;
	subParam.p[3] = T;

	LNdistrT = sasfit_sd_LogNorm(x[1], &subParam);

	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_R;
	subParam.p[2] = 1.0;
	subParam.p[3] = R;

	LNdistrR = sasfit_sd_LogNorm(x[0], &subParam);

    return vesicle_pegulated_gauss(x,n,p)*LNdistrT*LNdistrR;
}

scalar vesicle_pegulated_gauss_kernel_dQR(scalar x, sasfit_param * param)
{
    scalar v[2];
	scalar LNdistr;
	sasfit_param subParam;
	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_R;
	subParam.p[2] = 1.0;
	subParam.p[3] = R;

	LNdistr = sasfit_sd_LogNorm(x, &subParam);

    v[0]=x;
    v[1]=T;
    return vesicle_pegulated_gauss(v,2,param)*LNdistr;
}

scalar vesicle_pegulated_gauss_kernel_dQT(scalar x, sasfit_param * param)
{
    scalar v[2];
	scalar LNdistr;
	sasfit_param subParam;
	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_T;
	subParam.p[2] = 1.0;
	subParam.p[3] = T;

	LNdistr = sasfit_sd_LogNorm(x, &subParam);

    v[0]=R;
    v[1]=x;
    subParam.p[0] = 1.0;
    return vesicle_pegulated_gauss(v,2,param)*LNdistr;
}


scalar sasfit_ff_vesicle_peg_gaussian(scalar q, sasfit_param * param)
{
    int lenaw = 4000;
    scalar *aw;
    scalar res, err;
    double v[2],intstart[2],intend[2], res2D, err2D;
    scalar RStart,REnd, TStart,TEnd;
    int nintervals;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((F_PEG_I < 0.0), param, "f_PEG_i(%lg) < 0",F_PEG_I); // modify condition to your needs
	SASFIT_CHECK_COND1((F_PEG_I > 1.0), param, "f_PEG_i(%lg) > 1",F_PEG_I); // modify condition to your needs
	SASFIT_CHECK_COND1((F_PEG_O < 0.0), param, "f_PEG_o(%lg) < 0",F_PEG_O); // modify condition to your needs
	SASFIT_CHECK_COND1((F_PEG_O > 1.0), param, "f_PEG_o(%lg) > 1",F_PEG_O); // modify condition to your needs
	SASFIT_CHECK_COND1((V_PEG < 0.0), param, "V_PEG(%lg) < 0",V_PEG); // modify condition to your needs
	SASFIT_CHECK_COND1((V_HEAD < 0.0), param, "V_head(%lg) < 0",V_HEAD); // modify condition to your needs
	SASFIT_CHECK_COND1((V_TAIL < 0.0), param, "V_tail(%lg) < 0",V_TAIL); // modify condition to your needs

	// insert your code here
	Q=q;
	v[0]=R;
	v[1]=T;
	if (SIGMA_R==0) {
        RStart = R;
        REnd=RStart;
	} else {
        find_LogNorm_int_range(4, R, SIGMA_R, &RStart, &REnd, param);
	}
	if (SIGMA_T==0) {
        TStart = T;
        TEnd=TStart;
	} else {
        find_LogNorm_int_range(2, T, SIGMA_T, &TStart, &TEnd, param);
	}
	if (RStart==REnd && TStart==TEnd) return vesicle_pegulated_gauss(v,2,param);
    if (RStart==REnd ) return sasfit_integrate(TStart,TEnd,vesicle_pegulated_gauss_kernel_dQT,param);
    if (TStart==TEnd) return sasfit_integrate(RStart,REnd,vesicle_pegulated_gauss_kernel_dQR,param);

    intstart[0]=RStart;
    intend[0]  =REnd;
    intstart[1]=TStart;
    intend[1]=TEnd;
	sasfit_cubature(2,
			intstart,
			intend,
			&vesicle_pegulated_gauss_kernel,
			param,
			sasfit_eps_get_nriq(),
			&res2D,
			&err2D);
    return res2D;

}

scalar sasfit_ff_vesicle_peg_gaussian_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_vesicle_peg_gaussian_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

