/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R_POLAR	param->p[0]
#define K_POLAR	param->p[1]
#define R_EQUATORIAL	param->p[2]
#define K_EQUATORIAL	param->p[3]
#define DELTA_ETA	param->p[4]

#define Q   param->p[MAXPAR-1]
#define THETA_E param->p[MAXPAR-2]
#define THETA_P param->p[MAXPAR-3]

/* Inverse normalized incomplete gamma function
 *   invigamma(a, p) returns z such that igamma(a, z) = p
 * Following the logic of Numerical Recipes (6.2.1) we
 * use Halley's method to improve an initial guess at z
 * Ethan A Merritt - April 2020
 */

static double
inverse_incomplete_gamma( double a, double p, sasfit_param *param)
{
    double t, u, z;
    double err;
    double lngamma_a = gsl_sf_lngamma(a);
    double afac = exp( (a-1) * (log(a-1)-1.) - lngamma_a);
    int j;
    const double EPS = sqrt(GSL_MACH_EPS);
    bool undefined;

    SASFIT_CHECK_COND1((a <= 0.0), param, "a(%lg) <= 0, invigamma: a<=0 invalid",a);
	SASFIT_CHECK_COND1((p < 0.0), param, "p(%lg) < 0, invigamma: p<0 invalid",a);
	SASFIT_CHECK_COND1((p > 1.0), param, "p(%lg) > 1, invigamma: p>1 invalid",a);

	if (p == 1.) {
        return GSL_MAX( 100., a + 100.*sqrt(a) );
    }
    if (p == 0) {
        return 0;
    /* The normal case */
    }

    /* Initial guess based on Abramovitz & Stegun 26.2.22, 26.4.17 */
    if (a > 1.) {
        double pp = (p < 0.5) ? p : (1.-p);
	/* Abramowitz & Stegun 26.2.22 */
        t = sqrt(-2.*log(pp));
        z = t - (2.30753 + 0.27061*t) / (1. + 0.99229*t + 0.04481*t*t);
        if (p < 0.5) z = -z;
	/* Abramowitz & Stegun 26.4.17 */
            z = a * pow( (1. - 2./(9.*a) + z*sqrt(2./(9.*a))), 3 );

    /* Initial guess based on NR 6.2.8 6.2.9 */
    } else {
        t = 1. - a * (0.253 + a * 0.12);
        if (p < t)
            z = pow( p/t, 1./a);
        else
            z = 1. - log(1. - (p-t)/(1.-t));
    }

    /* Halley's method */
    for (j=0; j<12; j++) {
        if (z <= 0)
            return 0;
        err = gsl_sf_gamma_inc(a,z) - p;
        if (a > 1.)
            t = afac * exp( -(z-(a-1)) + (a-1)*(log(z)-log(a-1)));
        else
            t = exp( -z + (a-1)*log(z) - lngamma_a);
        u = err/t;
        t = u / (1. - 0.5*GSL_MIN( 1., u * ((a-1)/z - 1.)));
	/* FIXME: underflow OK? */
        SASFIT_CHECK_COND1(errno, param, "z = %g\n",z);
        z -= t;
        if (z <= 0)
            z = 0.5*(z+t);
        if (fabs(t) < z * EPS)
            break;
    }

    return z;
}


scalar find_gammaSD_int_range (scalar dim, scalar mode, scalar k, scalar *Xstart, scalar *Xend, sasfit_param *param)
{

/*
	scalar a1,a2,a3,a4, X_0, X_50, X_n, nperc,sigma;
	a2 = mode;
	R_max = a3*a3/a2*(a2/a3*a2/a3-1+dim);
    R_n = fabs(R_max);
    n = 1;
    tmp = 1.0;
    if (R_max > 0.0)  tmp=R_max;
	   while ( ((pow(n*R_n+tmp,dim) * sasfit_sd_gammaSD(n*R_n+tmp, &subParam) ) >
		 (pow(R_max,moment)     * n_percent / 100.0
					 * sasfit_sd_gammaSD(R_max, &subParam)) ) && (n < Nint-1)) {
				n++;
			}
		R_n = n*R_n;
		*Rstart = 0.0;
		*Rend   = R_n+tmp;
		if ((R_n+tmp) < R_max)  *Rstart=(R_max-(R_n+tmp));
		*n_intervals = Nint * n;

	SASFIT_CHECK_COND((X_50 == 0.0), param, "Can't guess good integration interval");

	*Xstart = 0.0;
	*Xend = X_n;

	if ((X_n-X_0) < X_0)
	{
		*Xend = X_n;
		*Xstart = 2.0*X_0-X_n;
	}
*/
	return 0;
}
scalar sasfit_ff_spheroid_w_dbl_sz_distr(scalar q, sasfit_param * param)
{
    /*
     *   THIS IS THE PROBABILITY DISTRIBUTION FUNCTION
     */
	scalar sum;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R_POLAR < 0.0), param, "R_polar(%lg) < 0",R_POLAR); // modify condition to your needs
	SASFIT_CHECK_COND1((K_POLAR < 0.0), param, "k_polar(%lg) < 0",K_POLAR); // modify condition to your needs
	SASFIT_CHECK_COND1((R_EQUATORIAL < 0.0), param, "R_equatorial(%lg) < 0",R_EQUATORIAL); // modify condition to your needs
	SASFIT_CHECK_COND1((K_EQUATORIAL < 0.0), param, "k_equatorial(%lg) < 0",K_EQUATORIAL); // modify condition to your needs
	SASFIT_CHECK_COND1((DELTA_ETA < 0.0), param, "Delta_eta(%lg) < 0",DELTA_ETA); // modify condition to your needs

    THETA_E = R_EQUATORIAL/(K_EQUATORIAL-1);
    THETA_P = R_POLAR/(K_POLAR-1);


    Q=q;
	sum=1;
	return sum;

}

scalar sasfit_ff_spheroid_w_dbl_sz_distr_f(scalar q, sasfit_param * param)
{
    /*
     *   THIS IS THE CUMMULITATIVE DISTRIBUTION FUNCTION
     */

	scalar sum;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
    THETA_E = R_EQUATORIAL/(K_EQUATORIAL-1);
    THETA_P = R_POLAR/(K_POLAR-1);
    Q=q;
	sum=1;
	return sum;

}

scalar sasfit_ff_spheroid_w_dbl_sz_distr_v(scalar r, sasfit_param * param, int dist)
{
    /*
     *   THIS IS THE QUANTILE DISTRIBUTION FUNCTION
     */
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 4./3.*M_PI*R_EQUATORIAL*R_EQUATORIAL*R_POLAR;
}

