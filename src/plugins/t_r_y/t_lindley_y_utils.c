/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   Ingo Bressler (ingo.bressler@bam.de)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

scalar Q_Uniform(scalar p) {
    scalar a=0, b=1;
    return a+p*(b-a);
}

scalar F_Uniform(scalar x) {
    scalar a=0, b=1;
    if (x<a) return 0;
    if (x>b) return 1;
    return (x-a)/(b-a);
}

scalar f_Uniform(scalar x) {
    scalar a=0, b=1;
    if (x<a) return 0;
    if (x>b) return 1;
    return 1/(b-a);
}

scalar Q_Weibull(scalar p, scalar alpha, scalar beta) {
    return beta*pow(-log(1-p),1/alpha);
}
scalar F_Weibull(scalar x, scalar alpha, scalar beta) {
    if (x<0) return 0;
    return 1-exp(-pow(x/beta,alpha));
}
scalar f_Weibull(scalar x, scalar alpha, scalar beta) {
    if (x<0) return 0;
    return alpha/beta*pow(x/beta,alpha-1)*exp(-pow(x/beta,alpha));
}

scalar Q_Exponential(scalar p, scalar lambda) {
    // lambda > 0
    return -1/lambda*log(1-p);
}
scalar F_Exponential(scalar x, scalar lambda) {
    // lambda,x > 0
    return 1-exp(-x*lambda);
}
scalar f_Exponential(scalar x, scalar lambda) {
    // lambda,x > 0
    return lambda*exp(-lambda*x);
}

scalar Q_ExtremeValue(scalar p, scalar a, scalar b) {
    // b > 0
    return a+b*log(-log(1-p));
}

scalar Q_Lindley(scalar p, scalar theta) {
    // theta > 0
    if (p==0) return 0;
    if (p==1) return 0;
    return -(1+theta+gsl_sf_lambert_Wm1(exp(-(1+theta))*(1+theta)*(p-1)))/theta;
}
scalar f_Lindley(scalar x, scalar theta) {
    // theta,x > 0
    return gsl_pow_2(theta)/(theta+1)*(1+x)*exp(-theta*x);
}
scalar F_Lindley(scalar x, scalar theta) {
    // theta,x > 0
    return 1-exp(-theta*x)*(1+theta+theta*x)/(1+theta);
}

scalar Q_LogLogistic(scalar p, scalar a, scalar b) {
    // a,b > 0
    return a*pow(p/(1-p),1./b);
}
scalar f_LogLogistic(scalar x, scalar a, scalar b) {
    // a,b > 0
    return b/a*pow(x/a,b-1)/gsl_pow_2(1+pow(x/a,b));
}
scalar F_LogLogistic(scalar x, scalar a, scalar b) {
    // a,b > 0
    return 1/(1+pow(x/a,-b));
}

scalar Q_Logistic(scalar p, scalar mu, scalar sigma) {
    // mu,sigma > 0
    return mu+sigma*log(p/(1-p));
}
scalar f_Logistic(scalar x, scalar mu, scalar sigma) {
    // mu,sigma > 0
    return exp(-(x-mu)/sigma)/(sigma*gsl_pow_2(1+exp(-(x-mu)/sigma)));
}
scalar F_Logistic(scalar x, scalar mu, scalar sigma) {
    // mu,sigma > 0
    return 1./(1+exp(-(x-mu)/sigma));
}

scalar Q_Cauchy(scalar p) {
    scalar gamma=1;
    return gamma*tan(M_PI*(p-0.5));
}
scalar f_Cauchy(scalar x) {
    scalar gamma=1;
    return 1/(M_PI*gamma*(1+gsl_pow_2(x/gamma)));
}
scalar F_Cauchy(scalar x) {
    scalar gamma=1;
    return 1/M_PI*atan(x/gamma)+0.5;
}

scalar Q_Gumbel(scalar p) {
    scalar sigma=1, mu=0;
//    return log(-log(1-p));
    return mu-sigma*log(-log(p));
}
scalar f_Gumbel(scalar x) {
    scalar sigma=1, mu=0;;
    return exp((x-mu)/sigma-exp((x-mu)/sigma));
}
scalar F_Gumbel(scalar x) {
    scalar sigma=1, mu=0;;
    return exp(-exp(-(x-mu)/sigma));
}

scalar Q_Gamma(scalar y, scalar alpha, scalar beta) {
    return gsl_sf_gamma(alpha)*sasfit_gammaincinv(alpha,y)/beta;
}
scalar f_Gamma(scalar x, scalar alpha, scalar beta) {
    return beta/gsl_sf_gamma(alpha)*pow(x*beta,alpha-1)*exp(-x*beta);
}
scalar F_Gamma(scalar x, scalar alpha, scalar beta) {
    return gsl_sf_gamma_inc_P(alpha,beta*x);
}

scalar Q_Frechet(scalar p, scalar k) {
    scalar lambda=1;
    return lambda*pow(-log(p),-1/k);
}
scalar f_Frechet(scalar x, scalar k) {
    scalar lambda=1;
    return exp(-pow(x/lambda,-k))*k/lambda*pow(x/lambda,-1-k);
}
scalar F_Frechet(scalar x, scalar k) {
    scalar lambda=1;
    return exp(-pow(x/lambda,-k));
}

scalar Q_Rayleigh(scalar p) {
    scalar sigma=1;
    return sigma*sqrt(-2*log(1-p));
}
scalar f_Rayleigh(scalar x) {
    scalar sigma=1;
    return x/gsl_pow_2(sigma) * exp(-0.5*gsl_pow_2(x/sigma));
}
scalar F_Rayleigh(scalar x) {
    scalar sigma=1;
    return 1-exp(-0.5*gsl_pow_2(x/sigma));
}

scalar Q_Pareto(scalar p, scalar alpha) {
    scalar mu=1;
    return mu*pow(1-p,-1/alpha);
}
scalar f_Pareto(scalar x, scalar alpha) {
    scalar mu=1;
    return alpha/x*pow(mu/x,alpha);
}
scalar F_Pareto(scalar x, scalar alpha) {
    scalar mu=1;
    return 1-pow(mu/x,alpha);
}

scalar KWm1(scalar Z, scalar theta) {
    return -(1+theta)/theta-1/theta*gsl_sf_lambert_Wm1(-Z*(theta+1)*exp(-(theta+1)));
}

scalar Qx (scalar y, sasfit_func_one_t *Fy, sasfit_func_one_t *QT, sasfit_param *param) {
    return KWm1(1-(*Fy)((*QT)(y,param),param),THETA);
}

scalar QxWLE(scalar y, sasfit_param *param) {
    return -(1+THETA)/THETA-1/THETA*gsl_sf_lambert_Wm1(-(THETA+1)*exp(-((THETA+1)+pow(-log(1-y),1/ALPHA)/BETA)));
}

scalar Lindley_f_R(scalar x, scalar theta) {
	if (x<0) return 0;
	return theta*theta/(1+theta)*(1+x)*exp(-theta*x);
}

scalar Lindley_F_R(scalar x, scalar theta) {
	if (x<=0) return 0;
	return 1-(1+theta*x/(theta+1))*exp(-theta*x);
}
