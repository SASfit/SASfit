/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define XI	param->p[0]
#define EPSILON	param->p[1]
#define DUMMY	param->p[2]
#define ETA	param->p[3]

scalar sasfit_ff_dab_epsilon(scalar q, sasfit_param * param)
{
    scalar P,u,u2,u4,e2,gamma2,alpha,beta,gamma,m,p;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((XI < 0.0), param, "xi(%lg) < 0",XI); // modify condition to your needs
	SASFIT_CHECK_COND1((EPSILON <= 0.0), param, "epsilon(%lg) <= 0",EPSILON); // modify condition to your needs
	SASFIT_CHECK_COND1((DUMMY < 0.0), param, "dummy(%lg) < 0",DUMMY); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs

	// insert your code here
	P=1-2./3.*gsl_pow_2(XI*q)*(2+EPSILON*EPSILON);
	if (gsl_pow_2(XI*q)*(2+EPSILON*EPSILON)<1e-6) return gsl_pow_2(ETA*EPSILON*gsl_pow_3(XI*2)*M_PI)*P;

	u = sqrt(1+gsl_pow_2(q*XI));
	u2=u*u;
	e2 = gsl_pow_2(EPSILON);
	if (EPSILON==1) {
        P=1./gsl_pow_2(1+gsl_pow_2(q*XI));
	} else if (EPSILON<1) {
	    u4=u2*u2;
	    gamma=sqrt((1-e2)*(u2*(u2-1)));
	    gamma2=gamma*gamma;
	    m=u2-gamma;
	    p=u2+gamma;
	    P=(-2*gamma*u2 - (gamma2 + u2 - u4)*
      log(gsl_pow_2(gamma - u2)) - u2*log(1/(-gamma + u2)) +
     (gamma2 + u2 - u4)*log(-gamma + u2) +
     (gamma2 - u4)*log(gamma + u2))/
   (4.*u2*(gamma2*gamma - gamma*u4));
        P = 0.25*(log(p/m)/(gamma*u2)+2/(m*p));
	} else {
	    alpha = sqrt((e2-1)*(u*u-1))/u;
	    beta  = sqrt(1+e2*(u*u-1))/u;
	    P=(alpha + 2*gsl_pow_2(beta)*atan(alpha) -
          gsl_pow_2(beta)*(atan(alpha - beta) + atan(alpha + beta)))/
          (2.*u2*alpha*(1 + e2*(-1 + u2)));
	}
	return gsl_pow_2(ETA*EPSILON*gsl_pow_3(XI*2)*M_PI)*P;
}

scalar sasfit_ff_dab_epsilon_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_dab_epsilon_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

