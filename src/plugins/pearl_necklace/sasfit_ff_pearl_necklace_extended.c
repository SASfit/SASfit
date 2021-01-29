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
#define G param->p[7]

scalar PN(scalar q, scalar F, scalar P, int g, int n, sasfit_param *param) {
    scalar qR,ql,qA, LA, beta, Sss, Srr, Srs, gamma,gammag, sinql2_ql2, sinqA2_qA2,A;
    int k,m;

    A=L+2.*R;
	qR = q*R;
	ql = q*L;
	qA = q*A;
	LA = Lambda(qA,param);
	sinqA2_qA2 = gsl_sf_bessel_j0(qA/2);
	gamma=gsl_sf_bessel_j0(qA);
    beta = LambdaExt(qR,ql,param);

    m=g*(n-1);
    gammag = gsl_pow_int(gamma,g);
    Sss = 2*gsl_pow_2(BETA_S)*P*(n/2.+n*gammag/(1-gammag)-(1-gsl_pow_int(gammag,n))*gammag/gsl_pow_2(1-gammag));
    Srr = gsl_pow_2(BETA_R)*(m*(2*LA-sinqA2_qA2*sinqA2_qA2)
                             +2*LA*LA*(m/(1-gamma)-(1-gsl_pow_int(gamma,m))/gsl_pow_2(1-gamma)));
    Srs = (n-1)*(1-gsl_pow_int(gamma,m))/(1-gamma);
    for (k=g;k<m;k++) Srs=Srs-gsl_pow_int(gamma,k)*floor(k/g);
    Srs = Srs*4*BETA_S*beta*BETA_R*F;
    return (Sss+Srr+Srs)/gsl_pow_2(m*BETA_R+n*BETA_S);
};

scalar sasfit_ff_pearl_necklace_ext(scalar q, sasfit_param * param)
{
    scalar  res, w, wg, F, P;
    int n,g;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA_R < 0.0), param, "beta_r(%lg) < 0",BETA_R); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA_S < 0.0), param, "beta_s(%lg) < 0",BETA_S); // modify condition to your needs
	SASFIT_CHECK_COND1((NP < 1.0), param, "Np(%lg) < 1",NP); // modify condition to your needs
    SASFIT_CHECK_COND1((G < 1.0), param, "g(%lg) < 1",G); // modify condition to your needs

	// insert your code here
    n = lround(floor(NP));
	w = NP-n;
	g = lround(floor(G));
	wg = G-g;

	F = Psi(q*R);
	P = F*F;

    res = (1-wg)*(1-w)*PN(q,F,P,g,n,param);
	if (wg!=0) {
        res = res+wg*(1-w)*PN(q,F,P,g+1,n,param);
 	}
	if (w!=0) {
        res=res+(1-wg)*w*PN(q,F,P,g,n+1,param);
    	if (wg!=0) {
            res=res+wg*w*PN(q,F,P,g+1,n+1,param);
        }
	}
	return res;
}

scalar sasfit_ff_pearl_necklace_ext_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_pearl_necklace_ext_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

