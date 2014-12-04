/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define l	param->p[1]
#define NU	param->p[2]
#define B	param->p[3]
#define NP	param->p[4]

scalar sasfit_ff_pearl_necklace(scalar q, sasfit_param * param)
{
    scalar  qR,ql,qA,Psi,L,Mr,mr,ms,beta,Sr,Sp,Sm,sinqA_qA,sinql2_ql2,A;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs
	SASFIT_CHECK_COND1((NU < 0.0), param, "nu(%lg) < 0",NU); // modify condition to your needs
	SASFIT_CHECK_COND1((B < 0.0), param, "b(%lg) < 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((NP < 0.0), param, "Np(%lg) < 0",NP); // modify condition to your needs

	// insert your code here

    A=l+2.*R;
	qR = q*R;
	ql = q*l;
	qA = q*A;
	mr=l/B; // number of monomers in rod-like segment
    ms=4.0*M_PI*gsl_pow_3(R)/(3.*NU);
	Mr=NP-1.0;
	if (qR==0.0) {
        Psi = 1;
	} else {
        Psi=3.*(sin(qR)-qR*cos(qR))/gsl_pow_3(qR);
	}
	if (ql == 0.0) {
        L=1.0;
        sinql2_ql2= 1.0;
	} else {
	    L=gsl_sf_Si(ql)/(ql);
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
    Sp=2*pow(ms,2)*pow(Psi,2)*((NP/(1-sinqA_qA))-(NP/2)-((1-pow(fabs(sinqA_qA),NP))/pow(1-sinqA_qA,2))*sinqA_qA);
    Sr=pow(mr,2)*(Mr*(2*L-pow(sinql2_ql2,2))+(2*Mr*pow(beta,2)/(1-sinqA_qA))- 2*pow(beta,2)*(1-pow(fabs(sinqA_qA),Mr))/pow(1-sinqA_qA,2));
    Sm=mr*beta*ms*Psi*4*(((NP-1)/(1-sinqA_qA))-((1-pow(fabs(sinqA_qA),NP-1))/pow(1-sinqA_qA,2))*sinqA_qA);
	return (Sp+Sr+Sm)/pow(Mr*mr+NP*ms,2);
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

