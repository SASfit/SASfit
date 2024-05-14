/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define N	param->p[0]
#define D	param->p[1]
#define DELTA	param->p[2]
#define NU	param->p[3]

scalar sasfit_sq_paracrystalline(scalar q, sasfit_param * param)
{
    scalar DWf,DWf2,q2,s2, SQ1, SQ2,qd,qD2,w;
    long NN,k,n;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
//	SASFIT_CHECK_COND1((N < 1.0), param, "N(%lg) < 1",N); // modify condition to your needs
	SASFIT_CHECK_COND1((D < 0.0), param, "d(%lg) < 0",D); // modify condition to your needs
//	SASFIT_CHECK_COND1((DELTA < 0.0), param, "Delta(%lg) < 0",DELTA); // modify condition to your needs
	SASFIT_CHECK_COND1((NU < 0.0), param, "Nu(%lg) < 0",NU); // modify condition to your needs

	// insert your code here
    qd=q*D;
    qD2 = gsl_pow_2(q*DELTA);
    NN = lround(floor(abs(N)));
	w = N-floor(N);
if  (N>0) {
    SQ1=NN;SQ2=NN+1;
    for (k=1;k<NN;k++) {
        SQ1 = SQ1+2*(NN  -k)*cos(k*qd)*exp(-k*qD2/2.);
        SQ2 = SQ2+2*(NN+1-k)*cos(k*qd)*exp(-k*qD2/2.);
    }
    SQ2 = SQ2+2*cos(NN*qd)*exp(-NN*qD2/2.);
    return NU+(1-w)*SQ1+w*SQ2;
} else {
    return sasfit_sq_paracrystalline_f(q,param);
}
}

scalar sasfit_sq_paracrystalline_f(scalar q, sasfit_param * param)
{
    scalar DWf,DWf2,q2,s2, SQ1, SQ2,qd,qD2,w;
    long NN,k,n;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
    qd=q*D;
    qD2 = gsl_pow_2(q*DELTA);
    NN = lround(floor(abs(N)));
	w = N-floor(N);

    q2 = q*q;
    s2 = gsl_pow_2(DELTA);
if (qd*fabs(N)<1) {
    n = NN;
    SQ1 = n*n-1./12.*n*(n*n-1)*q2*(D*D*n+2*s2);
    n = NN+1;
    SQ2 = n*n-1./12.*n*(n*n-1)*q2*(D*D*n+2*s2);
} else {
    n = NN;
    SQ1= (2*(12*exp(((4 + n)*q2*s2)/2.) + 8*exp(((4 + n)*q2*s2)/2.)*cos(2*qd) -
       exp(((3 + n)*q2*s2)/2.)*(1 - n + exp(q2*s2)*(1 + n))*cos(3*qd) +
       exp((q2*s2)/2.)*cos(qd*(-1 + n)) - 2*exp(q2*s2)*(cos(qd*(-2 + n)) + 2*cos(qd*n)) +
       exp((7*q2*s2)/2.)*cos(qd*(1 + n)) +
       exp((3*q2*s2)/2.)*(cos(qd*(-3 + n)) + 8*cos(qd*(-1 + n)) + 6*cos(qd*(1 + n))) -
       2*exp(3*q2*s2)*(2*cos(qd*n) + cos(qd*(2 + n))) -
       4*exp(2*q2*s2)*(cos(qd*(-2 + n)) + 3*cos(qd*n) + cos(qd*(2 + n))) +
       exp((5*q2*s2)/2.)*(6*cos(qd*(-1 + n)) + 8*cos(qd*(1 + n)) + cos(qd*(3 + n))) +
       exp(((4 + n)*q2*s2)/2.)*(4*(2 + cos(2*qd))*cosh(q2*s2) -
          2*cos(qd)*(14*cosh((q2*s2)/2.) + cosh((3*q2*s2)/2.) +
             3*n*(3 + 2*cosh(q2*s2))*sinh((q2*s2)/2.)) +
          2*n*(4 + 3*cos(2*qd) + cosh(q2*s2))*sinh(q2*s2))))*exp(-(n*q2*s2)/2.)/
        (gsl_pow_4(1 + exp(q2*s2) - 2*exp((q2*s2)/2.)*cos(qd)));
    n = NN+1;
    SQ2= (2*(12*exp(((4 + n)*q2*s2)/2.) + 8*exp(((4 + n)*q2*s2)/2.)*cos(2*qd) -
       exp(((3 + n)*q2*s2)/2.)*(1 - n + exp(q2*s2)*(1 + n))*cos(3*qd) +
       exp((q2*s2)/2.)*cos(qd*(-1 + n)) - 2*exp(q2*s2)*(cos(qd*(-2 + n)) + 2*cos(qd*n)) +
       exp((7*q2*s2)/2.)*cos(qd*(1 + n)) +
       exp((3*q2*s2)/2.)*(cos(qd*(-3 + n)) + 8*cos(qd*(-1 + n)) + 6*cos(qd*(1 + n))) -
       2*exp(3*q2*s2)*(2*cos(qd*n) + cos(qd*(2 + n))) -
       4*exp(2*q2*s2)*(cos(qd*(-2 + n)) + 3*cos(qd*n) + cos(qd*(2 + n))) +
       exp((5*q2*s2)/2.)*(6*cos(qd*(-1 + n)) + 8*cos(qd*(1 + n)) + cos(qd*(3 + n))) +
       exp(((4 + n)*q2*s2)/2.)*(4*(2 + cos(2*qd))*cosh(q2*s2) -
          2*cos(qd)*(14*cosh((q2*s2)/2.) + cosh((3*q2*s2)/2.) +
             3*n*(3 + 2*cosh(q2*s2))*sinh((q2*s2)/2.)) +
          2*n*(4 + 3*cos(2*qd) + cosh(q2*s2))*sinh(q2*s2))))*exp(-(n*q2*s2)/2.)/
        (gsl_pow_4(1 + exp(q2*s2) - 2*exp((q2*s2)/2.)*cos(qd)));
    }
    return NU+(1-w)*SQ1+w*SQ2;
}

scalar sasfit_sq_paracrystalline_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

