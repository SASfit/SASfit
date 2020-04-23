#include "float.h"
/*
 * src/sasfit_peaks/sasfit_peak_LogisticAmplitude.c
 *
 * Copyright (c) 2008-2009, Paul Scherrer Institute (PSI)
 *
 * This file is part of SASfit.
 *
 * SASfit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SASfit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SASfit.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

#define AMPLITUDE	param->p[0]
#define XMIN	param->p[1]
#define XMAX	param->p[2]
// #define MODE  	param->p[3]
#define MU  	param->p[3]
#define SIGMA	fabs(param->p[4])
#define BACKGR	param->p[5]
//#define MU      param->p[MAXPAR-1]
#define ZMODE   param->p[MAXPAR-2]
scalar Logit(scalar x) {
    if (x <= 0) return GSL_NEGINF;
    if (x>=1) return GSL_POSINF;
    return log(x/(1-x));
}

scalar D1LogitNormal(scalar x, scalar sigma, scalar mu) {
    return (mu + gsl_pow_2(sigma)*(-1 + 2*x) - Logit(x))/
   (exp(gsl_pow_2(mu - Logit(x))/
       (2.*gsl_pow_2(sigma)))*sqrt(2*M_PI)*gsl_pow_3(sigma)*
     gsl_pow_2(-1 + x)*gsl_pow_2(x));
}

scalar D2LogitNormal(scalar x, scalar sigma, scalar mu) {
    return (-gsl_pow_2(mu) + gsl_pow_2(sigma) +
     3*mu*gsl_pow_2(sigma)*(1 - 2*x) +
     gsl_pow_4(sigma)*(-2 - 6*(-1 + x)*x) +
     (2*mu + 3*gsl_pow_2(sigma)*(-1 + 2*x) -
        Logit(x))*Logit(x))/
   (exp(gsl_pow_2(mu - Logit(x))/
       (2.*gsl_pow_2(sigma)))*sqrt(2*M_PI)*gsl_pow_5(sigma)*
     gsl_pow_3(-1 + x)*gsl_pow_3(x));
}

scalar LogitMode(scalar x, void * pam) {
    sasfit_param * param;
    param = (sasfit_param *) pam;
    if (x<=0) return 0;
    if (x>=1) return 0;
    return -exp(-gsl_pow_2((Logit(x)-MU)/SIGMA)/2.) /(x*(1-x));
}

scalar LogitMu(scalar x, void * pam) {
    sasfit_param * param;
    param = (sasfit_param *) pam;
    return -exp(-gsl_pow_2((Logit(ZMODE)-x)/SIGMA)/2.) /(ZMODE*(1-ZMODE));
}

scalar sasfit_peak_logit_amplitude(scalar x, sasfit_param * param)
{
	scalar z,scale, xmin, xmax, a,b, m;
	int status, maxiter,iter;
    const gsl_min_fminimizer_type *Tmin;
    gsl_min_fminimizer *smin;
    gsl_function F;

	SASFIT_ASSERT_PTR( param );

	if (XMIN>XMAX) {
		xmin = XMAX;
		xmax = XMIN;
	} else {
		xmin = XMIN;
		xmax = XMAX;
	}
	SASFIT_CHECK_COND2((xmin == xmax), param, "xmin(%lg) == xmax(%lg)",xmin,xmax);
	SASFIT_CHECK_COND1((SIGMA == 0), param, "sigma(%lg) == 0",SIGMA);
//	SASFIT_CHECK_COND2((MODE <= xmin), param, "mode(%lg) <= xmin",MODE,xmin);
//	SASFIT_CHECK_COND2((MODE >= xmax), param, "mode(%lg) >= xmax",MODE,xmax);

	if (x<=xmin) return BACKGR;
	if (x>=xmax) return BACKGR;

	z = (x-xmin)/(xmax-xmin);
/*
	ZMODE = (MODE-xmin)/(xmax-xmin);
    MU = Logit(ZMODE)-SIGMA*SIGMA*(2*ZMODE-1.0);
    if (gsl_fcmp(D1LogitNormal(ZMODE,SIGMA,MU),0,DBL_EPSILON) && D2LogitNormal(ZMODE,SIGMA,MU) < 0) {
        scale = exp(-gsl_pow_2((Logit(ZMODE)-MU)/SIGMA)/2.) /(ZMODE*(1-ZMODE));
    } else {
        scale = 1;
        F.function = &LogitMu;
        F.params = param;
        Tmin = gsl_min_fminimizer_brent;
        smin = gsl_min_fminimizer_alloc (Tmin);
        if (ZMODE<0.5) {
            a = -1./DBL_EPSILON;
            b = -DBL_EPSILON;
            m = -5.;
        } else {
            a = DBL_EPSILON;
            b = 1./DBL_EPSILON;
            m = 5.;
        }
        sasfit_out("0st a %lf,b %lf\n",a,b);
        gsl_min_fminimizer_set (smin, &F, m, a, b);
        iter = 0;
        maxiter = 100;
        sasfit_out("1st a %lf,b %lf\n",a,b);
        do {
         iter++;
         status = gsl_min_fminimizer_iterate (smin);

         m = gsl_min_fminimizer_x_minimum (smin);
         a = gsl_min_fminimizer_x_lower (smin);
         b = gsl_min_fminimizer_x_upper (smin);

         status = gsl_min_test_interval (a, b, 1e-6, 0.0);
         sasfit_out("a %lf,b %lf\n",a,b);
        } while (status == GSL_CONTINUE && iter < maxiter);
        gsl_min_fminimizer_free (smin);
        MU = m;
        scale = exp(-gsl_pow_2((Logit(ZMODE)-MU)/SIGMA)/2.) /(ZMODE*(1-ZMODE));
    }
*/
    F.function = &LogitMode;
    F.params = param;
    Tmin = gsl_min_fminimizer_brent;
    Tmin = gsl_min_fminimizer_quad_golden;
    smin = gsl_min_fminimizer_alloc (Tmin);
    if (MU<0) {
        a = 0;
        b = 0.5;
        m = 0.25;
    } else {
        a = 0.5;
        b = 1;
        m = 0.75;
    }
    gsl_set_error_handler_off();
    status = gsl_min_fminimizer_set (smin, &F, m, a, b);
    if (status) {
        if (status == GSL_EINVAL) {
            sasfit_out ( "invalid argument, f(m) %lf, f(a) %lf f(b) %lf\n",LogitMu(m,param),LogitMu(a,param),LogitMu(b,param));
        } else {
            sasfit_out ( "failed, gsl_errno=%d\n", status);
        }
        return 0;
    }
    iter = 0;
    maxiter = 1000;
//    sasfit_out("1st a %lf,b %lf\n",a,b);
    do {
        iter++;
        status = gsl_min_fminimizer_iterate (smin);

        m = gsl_min_fminimizer_x_minimum (smin);
        a = gsl_min_fminimizer_x_lower (smin);
        b = gsl_min_fminimizer_x_upper (smin);

        status = gsl_min_test_interval (a, b, sasfit_eps_get_nriq(), 0.0);
//        sasfit_out("a %lf,b %lf\n",a,b);
    } while (status == GSL_CONTINUE && iter < maxiter);
    gsl_min_fminimizer_free (smin);
    ZMODE = m;
    scale = exp(-gsl_pow_2((Logit(ZMODE)-MU)/SIGMA)/2.) /(ZMODE*(1-ZMODE));
	return BACKGR
			+ AMPLITUDE * exp(-gsl_pow_2((Logit(z)-MU)/SIGMA)/2.) / (scale*z*(1-z));
}

scalar sasfit_peak_logit_amplitude_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_peak_logit_amplitude_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}
