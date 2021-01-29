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
#define DUMMY3 param->p[7]

scalar sasfit_ff_pearl_necklace_mod(scalar q, sasfit_param * param)
{
    scalar  qR,ql,qA,Fpsi,Ppsi, LA,Mr,mr,ms,beta,Sr,Sp,Sm,sinqA_qA,sinql2_ql2,A;
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
	Mr=NP-1.0;
	if (qR==0.0) {
        Fpsi = 1;
	} else {
        Fpsi=3.*(sin(qR)-qR*cos(qR))/gsl_pow_3(qR);
	}
	Ppsi=gsl_pow_2(Fpsi);
	if (ql == 0.0) {
        LA=1.0;
        sinql2_ql2= 1.0;
	} else {
	    LA=gsl_sf_Si(ql)/(ql);
	    sinql2_ql2 = sin(ql/2.0)*2/ql;
	}
	if (qA == 0.0) {
        sinqA_qA=1.;
	} else {
	    sinqA_qA=sin(qA)/(qA);
	}


    if (q==0) {
        beta = 1.0;
    } else if (A==2*R) {
        if (R==0) {
            beta = 1;
        } else {
            beta = sin(q*R)/(q*R);
        }
    } else {
        beta=(gsl_sf_Si(q*(A-R))-gsl_sf_Si(qR))/(ql);
    }
    Sp=2*pow(BETA_S,2)*Ppsi*((NP/(1-sinqA_qA))-(NP/2)-((1-pow(fabs(sinqA_qA),NP))/pow(1-sinqA_qA,2))*sinqA_qA);
    Sr=pow(BETA_R,2)*(Mr*(2*LA-pow(sinql2_ql2,2))+(2*Mr*pow(beta,2)/(1-sinqA_qA))- 2*pow(beta,2)*(1-pow(fabs(sinqA_qA),Mr))/pow(1-sinqA_qA,2));
    Sm=BETA_R*beta*BETA_S*Fpsi*4*(((NP-1)/(1-sinqA_qA))-((1-pow(fabs(sinqA_qA),NP-1))/pow(1-sinqA_qA,2))*sinqA_qA);
	return (Sp+Sr+Sm)/pow(Mr*BETA_R+NP*BETA_S,2);
}

scalar sasfit_ff_pearl_necklace_mod_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_pearl_necklace_mod_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

