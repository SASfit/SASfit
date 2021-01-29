/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   8.12.2014
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define DUMMY1 param->p[1] // nu
#define DUMMY2 param->p[2] // d
#define L	param->p[3]
#define BETA_R	param->p[4]
#define BETA_S	param->p[5]
#define NP	param->p[6]
#define DUMMY3 param->p[7] // g

scalar sasfit_ff_pearl_necklace(scalar q, sasfit_param * param)
{
    scalar  res, qR,ql,qA,Fpsi,Ppsi,w, LA,beta,Sss, Srr, Srs, gamma,sinql2_ql2,A;
    int n,m;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA_R < 0.0), param, "beta_r(%lg) < 0",BETA_R); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA_S < 0.0), param, "beta_s(%lg) < 0",BETA_S); // modify condition to your needs
	SASFIT_CHECK_COND1((NP < 0.0), param, "Np(%lg) < 0",NP); // modify condition to your needs

	// insert your code here

    A=L+2.*R;
	qR = q*R;
	ql = q*L;
	qA = q*A;
	Fpsi = Psi(qR);
	Ppsi=gsl_pow_2(Fpsi);
	LA = Lambda(ql,param);
	sinql2_ql2 = gsl_sf_bessel_j0(ql/2);
	gamma=gsl_sf_bessel_j0(qA);
    beta = LambdaExt(qR,ql,param);


	n = lround(floor(NP));
	w = NP-n;
	m=n-1;
    Sss = 2*gsl_pow_2(BETA_S)*Ppsi*(n/2.+n*gamma/(1-gamma)-(1-gsl_pow_int(gamma,n))*gamma/gsl_pow_2(1-gamma));
    Srr = gsl_pow_2(BETA_R)*(m*(2*LA-sinql2_ql2*sinql2_ql2)
                             +2*beta*beta*(m/(1-gamma)-(1-gsl_pow_int(gamma,m))/gsl_pow_2(1-gamma)));
    Srs = 4*BETA_S*beta*BETA_R*Fpsi*(m/(1-gamma)-(1-gsl_pow_int(gamma,m))*gamma/gsl_pow_2(1-gamma));
	res = (1-w)*(Sss+Srr+Srs)/gsl_pow_2(m*BETA_R+n*BETA_S);
	if (w!=0) {
        n++;
        m++;
        Sss = 2*gsl_pow_2(BETA_S)*Ppsi*(n/2.+n*gamma/(1-gamma)-(1-gsl_pow_int(gamma,n))*gamma/gsl_pow_2(1-gamma));
        Srr = gsl_pow_2(BETA_R)*(m*(2*LA-sinql2_ql2*sinql2_ql2)
                             +2*beta*beta*(m/(1-gamma)-(1-gsl_pow_int(gamma,m))/gsl_pow_2(1-gamma)));
        Srs = 4*BETA_S*beta*BETA_R*Fpsi*(m/(1-gamma)-(1-gsl_pow_int(gamma,m))*gamma/gsl_pow_2(1-gamma));
        res = res+w*(Sss+Srr+Srs)/gsl_pow_2(m*BETA_R+n*BETA_S);
	}
	return res;
}

scalar sasfit_ff_pearl_necklace_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_pearl_necklace_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

