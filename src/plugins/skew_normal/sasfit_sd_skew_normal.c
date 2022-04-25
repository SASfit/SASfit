/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables
#define N	param->p[0]
#define OMEGA	fabs(param->p[1])
#define XI	param->p[2]
#define ALPHA	param->p[3]

#define DELTA (ALPHA/gsl_hypot(1,ALPHA))

/******************************************************************************/

double alnorm ( double x, int upper )

/******************************************************************************/
/*
  Purpose:

    ALNORM computes the cumulative density of the standard normal distribution.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    17 January 2008

  Author:

    Original FORTRAN77 version by David Hill.
    C++ version by John Burkardt.

  Reference:

    David Hill,
    Algorithm AS 66:
    The Normal Integral,
    Applied Statistics,
    Volume 22, Number 3, 1973, pages 424-427.

  Parameters:

    Input, double X, is one endpoint of the semi-infinite interval
    over which the integration takes place.

    Input, int UPPER, determines whether the upper or lower
    interval is to be integrated:
    1  => integrate from X to + Infinity;
    0 => integrate from - Infinity to X.

    Output, double ALNORM, the integral of the standard normal
    distribution over the desired interval.
*/
{
  double a1 = 5.75885480458;
  double a2 = 2.62433121679;
  double a3 = 5.92885724438;
  double b1 = -29.8213557807;
  double b2 = 48.6959930692;
  double c1 = -0.000000038052;
  double c2 = 0.000398064794;
  double c3 = -0.151679116635;
  double c4 = 4.8385912808;
  double c5 = 0.742380924027;
  double c6 = 3.99019417011;
  double con = 1.28;
  double d1 = 1.00000615302;
  double d2 = 1.98615381364;
  double d3 = 5.29330324926;
  double d4 = -15.1508972451;
  double d5 = 30.789933034;
  double ltone = 7.0;
  double p = 0.398942280444;
  double q = 0.39990348504;
  double r = 0.398942280385;
  int up;
  double utzero = 18.66;
  double value;
  double y;
  double z;

  up = upper;
  z = x;

  if ( z < 0.0 )
  {
    up = !up;
    z = - z;
  }

  if ( ltone < z && ( ( !up ) || utzero < z ) )
  {
    if ( up )
    {
      value = 0.0;
    }
    else
    {
      value = 1.0;
    }
    return value;
  }

  y = 0.5 * z * z;

  if ( z <= con )
  {
    value = 0.5 - z * ( p - q * y
      / ( y + a1 + b1
      / ( y + a2 + b2
      / ( y + a3 ))));
  }
  else
  {
    value = r * exp ( - y )
      / ( z + c1 + d1
      / ( z + c2 + d2
      / ( z + c3 + d3
      / ( z + c4 + d4
      / ( z + c5 + d5
      / ( z + c6 ))))));
  }

  if ( !up )
  {
    value = 1.0 - value;
  }

  return value;
}
/******************************************************************************/

void owen_values ( int *n_data, double *h, double *a, double *t )

/******************************************************************************/
/*
  Purpose:

    OWEN_VALUES returns some values of Owen's T function.

  Discussion:

    Owen's T function is useful for computation of the bivariate normal
    distribution and the distribution of a skewed normal distribution.

    Although it was originally formulated in terms of the bivariate
    normal function, the function can be defined more directly as

      T(H,A) = 1 / ( 2 * pi ) *
        Integral ( 0 <= X <= A ) e^(H^2*(1+X^2)/2) / (1+X^2) dX

    In Mathematica, the function can be evaluated by:

      fx = 1/(2*Pi) * Integrate [ E^(-h^2*(1+x^2)/2)/(1+x^2), {x,0,a} ]

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    15 December 2011

  Author:

    John Burkardt

  Reference:

    Stephen Wolfram,
    The Mathematica Book,
    Fourth Edition,
    Cambridge University Press, 1999,
    ISBN: 0-521-64314-7,
    LC: QA76.95.W65.

  Parameters:

    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
    first call.  On each call, the routine increments N_DATA by 1, and
    returns the corresponding data; when there is no more data, the
    output value of N_DATA will be 0 again.

    Output, double *H, a parameter.

    Output, double *A, the upper limit of the integral.

    Output, double *T, the value of the function.
*/
{
# define N_MAX 28

  static double a_vec[N_MAX] = {
    0.2500000000000000E+00,
    0.4375000000000000E+00,
    0.9687500000000000E+00,
    0.0625000000000000E+00,
    0.5000000000000000E+00,
    0.9999975000000000E+00,
    0.5000000000000000E+00,
    0.1000000000000000E+01,
    0.2000000000000000E+01,
    0.3000000000000000E+01,
    0.5000000000000000E+00,
    0.1000000000000000E+01,
    0.2000000000000000E+01,
    0.3000000000000000E+01,
    0.5000000000000000E+00,
    0.1000000000000000E+01,
    0.2000000000000000E+01,
    0.3000000000000000E+01,
    0.5000000000000000E+00,
    0.1000000000000000E+01,
    0.2000000000000000E+01,
    0.3000000000000000E+01,
    0.5000000000000000E+00,
    0.1000000000000000E+01,
    0.2000000000000000E+01,
    0.3000000000000000E+01,
    0.1000000000000000E+02,
    0.1000000000000000E+03 };

  static double h_vec[N_MAX] = {
    0.0625000000000000E+00,
    6.5000000000000000E+00,
    7.0000000000000000E+00,
    4.7812500000000000E+00,
    2.0000000000000000E+00,
    1.0000000000000000E+00,
    0.1000000000000000E+01,
    0.1000000000000000E+01,
    0.1000000000000000E+01,
    0.1000000000000000E+01,
    0.5000000000000000E+00,
    0.5000000000000000E+00,
    0.5000000000000000E+00,
    0.5000000000000000E+00,
    0.2500000000000000E+00,
    0.2500000000000000E+00,
    0.2500000000000000E+00,
    0.2500000000000000E+00,
    0.1250000000000000E+00,
    0.1250000000000000E+00,
    0.1250000000000000E+00,
    0.1250000000000000E+00,
    0.7812500000000000E-02,
    0.7812500000000000E-02,
    0.7812500000000000E-02,
    0.7812500000000000E-02,
    0.7812500000000000E-02,
    0.7812500000000000E-02 };

  static double t_vec[N_MAX] = {
    3.8911930234701366E-02,
    2.0005773048508315E-11,
    6.3990627193898685E-13,
    1.0632974804687463E-07,
    8.6250779855215071E-03,
    6.6741808978228592E-02,
    0.4306469112078537E-01,
    0.6674188216570097E-01,
    0.7846818699308410E-01,
    0.7929950474887259E-01,
    0.6448860284750376E-01,
    0.1066710629614485E+00,
    0.1415806036539784E+00,
    0.1510840430760184E+00,
    0.7134663382271778E-01,
    0.1201285306350883E+00,
    0.1666128410939293E+00,
    0.1847501847929859E+00,
    0.7317273327500385E-01,
    0.1237630544953746E+00,
    0.1737438887583106E+00,
    0.1951190307092811E+00,
    0.7378938035365546E-01,
    0.1249951430754052E+00,
    0.1761984774738108E+00,
    0.1987772386442824E+00,
    0.2340886964802671E+00,
    0.2479460829231492E+00 };

  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *h = 0.0;
    *a = 0.0;
    *t = 0.0;
  }
  else
  {
    *h = h_vec[*n_data-1];
    *a = a_vec[*n_data-1];
    *t = t_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
/******************************************************************************/

double tfn ( double x, double fx )

/******************************************************************************/
/*
  Purpose:

    TFN calculates the T-function of Owen.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    02 November 2010

  Author:

    Original FORTRAN77 version by JC Young, Christoph Minder.
    C version by John Burkardt.

  Reference:

    MA Porter, DJ Winstanley,
    Remark AS R30:
    A Remark on Algorithm AS76:
    An Integral Useful in Calculating Noncentral T and Bivariate
    Normal Probabilities,
    Applied Statistics,
    Volume 28, Number 1, 1979, page 113.

    JC Young, Christoph Minder,
    Algorithm AS 76:
    An Algorithm Useful in Calculating Non-Central T and
    Bivariate Normal Distributions,
    Applied Statistics,
    Volume 23, Number 3, 1974, pages 455-457.

  Parameters:

    Input, double X, FX, the parameters of the function.

    Output, double TFN, the value of the T-function.
*/
{
# define NG 5

  double fxs;
  int i;
  double r[NG] = {
    0.1477621,
    0.1346334,
    0.1095432,
    0.0747257,
    0.0333357 };
  double r1;
  double r2;
  double rt;
  double tp = 0.159155;
  double tv1 = 1.0E-35;
  double tv2 = 15.0;
  double tv3 = 15.0;
  double tv4 = 1.0E-05;
  double u[NG] = {
    0.0744372,
    0.2166977,
    0.3397048,
    0.4325317,
    0.4869533 };
  double value;
  double x1;
  double x2;
  double xs;
/*
  Test for X near zero.
*/
  if ( fabs ( x ) < tv1 )
  {
    value = tp * atan ( fx );
    return value;
  }
/*
  Test for large values of abs(X).
*/
  if ( tv2 < fabs ( x ) )
  {
    value = 0.0;
    return value;
  }
/*
  Test for FX near zero.
*/
  if ( fabs ( fx ) < tv1 )
  {
    value = 0.0;
    return value;
  }
/*
  Test whether abs ( FX ) is so large that it must be truncated.
*/
  xs = - 0.5 * x * x;
  x2 = fx;
  fxs = fx * fx;
/*
  Computation of truncation point by Newton iteration.
*/
  if ( tv3 <= log ( 1.0 + fxs ) - xs * fxs )
  {
    x1 = 0.5 * fx;
    fxs = 0.25 * fxs;

    for ( ; ; )
    {
      rt = fxs + 1.0;

      x2 = x1 + ( xs * fxs + tv3 - log ( rt ) )
      / ( 2.0 * x1 * ( 1.0 / rt - xs ) );

      fxs = x2 * x2;

      if ( fabs ( x2 - x1 ) < tv4 )
      {
        break;
      }
      x1 = x2;
    }
  }
/*
  Gaussian quadrature.
*/
  rt = 0.0;
  for ( i = 0; i < NG; i++ )
  {
    r1 = 1.0 + fxs * pow ( 0.5 + u[i], 2 );
    r2 = 1.0 + fxs * pow ( 0.5 - u[i], 2 );

    rt = rt + r[i] * ( exp ( xs * r1 ) / r1 + exp ( xs * r2 ) / r2 );
  }

  value = rt * x2 * tp;

  return value;
# undef NG
}
/******************************************************************************/

double tha ( double h1, double h2, double a1, double a2 )

/******************************************************************************/
/*
  Purpose:

    THA computes Owen's T function.

  Discussion:

    This function computes T(H1/H2, A1/A2) for any real numbers H1, H2,
    A1 and A2.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    02 November 2010

  Author:

    Original FORTRAN77 version by JC Young, Christoph Minder.
    C version by John Burkardt.

  Reference:

    Richard Boys,
    Remark AS R80:
    A Remark on Algorithm AS76:
    An Integral Useful in Calculating Noncentral T and Bivariate
    Normal Probabilities,
    Applied Statistics,
    Volume 38, Number 3, 1989, pages 580-582.

    Youn-Min Chou,
    Remark AS R55:
    A Remark on Algorithm AS76:
    An Integral Useful in Calculating Noncentral T and Bivariate
    Normal Probabilities,
    Applied Statistics,
    Volume 34, Number 1, 1985, pages 100-101.

    PW Goedhart, MJW Jansen,
    Remark AS R89:
    A Remark on Algorithm AS76:
    An Integral Useful in Calculating Noncentral T and Bivariate
    Normal Probabilities,
    Applied Statistics,
    Volume 41, Number 2, 1992, pages 496-497.

    JC Young, Christoph Minder,
    Algorithm AS 76:
    An Algorithm Useful in Calculating Noncentral T and
    Bivariate Normal Distributions,
    Applied Statistics,
    Volume 23, Number 3, 1974, pages 455-457.

  Parameters:

    Input, double H1, H2, A1, A2, define the arguments
    of the T function.

    Output, double THA, the value of Owen's T function.
*/
{
  double a;
  double absa;
  double ah;
  double c1;
  double c2;
  double ex;
  double g;
  double gah;
  double gh;
  double h;
  double lam;
  double twopi = 6.2831853071795864769;
  double value;

  if ( h2 == 0.0 )
  {
    value = 0.0;
    return value;
  }

  h = h1 / h2;

  if ( a2 == 0.0 )
  {
    g = alnorm ( h, 0 );

    if ( h < 0.0 )
    {
      value = g / 2.0;
    }
    else
    {
      value = ( 1.0 - g ) / 2.0;
    }

    if ( a1 < 0.0 )
    {
      value = - value;
    }
    return value;
  }

  a = a1 / a2;

  if ( fabs ( h ) < 0.3 && 7.0 < fabs ( a ) )
  {
    lam = fabs ( a * h );
    ex = exp ( - lam * lam / 2.0 );
    g = alnorm ( lam, 0 );
    c1 = ( ex / lam + sqrt ( twopi ) * ( g - 0.5 ) ) / twopi;
    c2 = ( ( lam * lam + 2.0 ) * ex / lam / lam / lam
      + sqrt ( twopi ) * ( g - 0.5 ) ) / ( 6.0 * twopi );
    ah = fabs ( h );
    value = 0.25 - c1 * ah + c2 * ah * ah * ah;
    if ( a < 0.0 )
    {
      value = - fabs ( value );
    }
    else
    {
      value = fabs ( value );
    }
  }
  else
  {
    absa = fabs ( a );

    if ( absa <= 1.0 )
    {
      value = tfn ( h, a );
      return value;
    }

    ah = absa * h;
    gh = alnorm ( h, 0 );
    gah = alnorm ( ah, 0 );
    value = 0.5 * ( gh + gah ) - gh * gah - tfn ( ah, 1.0 / absa );

    if ( a < 0.0 )
    {
      value = - value;
    }
  }
  return value;
}
/******************************************************************************/


scalar phi(scalar x) {
	return 1/(sqrt(2*M_PI))*exp(-x*x/2);
}

scalar PHI(scalar x) {
	return 0.5*(1+gsl_sf_erf(x/sqrt(2)));
}

scalar skewness(sasfit_param *param) {
    return (4-M_PI)/2.0*gsl_pow_2(DELTA*sqrt(2./M_PI))/pow(1-2*DELTA*DELTA/M_PI,1.5);
}

scalar m0(sasfit_param *param) {
    scalar gamma1, gammaz, sigmaz;
    gamma1 = skewness(param);
    gammaz = sqrt(2/M_PI)*DELTA;
    sigmaz = sqrt(1-gammaz*gammaz);
    if (ALPHA==0) return gammaz-gamma1*sigmaz/2;
    return gammaz-gamma1*sigmaz/2-GSL_SIGN(ALPHA)/2.0*exp(-2*M_PI/fabs(ALPHA));
}

scalar sasfit_sd_skew_normal_mode(scalar x, sasfit_param * param) {
    return XI+OMEGA*m0(param);
}

scalar sasfit_sd_skew_normal_mean(scalar x, sasfit_param * param) {
    return XI+OMEGA*DELTA*sqrt(2.0/M_PI);
}

scalar sasfit_sd_skew_normal(scalar x, sasfit_param * param)
{
    scalar z;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((OMEGA <= 0.0), param, "omega(%lg) <= 0",OMEGA); // modify condition to your needs
	// insert your code here
	z=(x-XI)/OMEGA;
	return N*2.0/OMEGA*phi(z)*PHI(ALPHA*z)/(1-sasfit_sd_skew_normal_f(0,param));
}

scalar sasfit_sd_skew_normal_f(scalar x, sasfit_param * param)
{
    scalar z;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	z=(x-XI)/OMEGA;
	return PHI(z)-2*tfn(z,ALPHA);
}

scalar sasfit_sd_skew_normal_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

