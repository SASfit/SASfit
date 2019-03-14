/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define I0	param->p[0]
#define RG	param->p[1]
#define D0	param->p[2]
#define NU	param->p[3]
#define LAMBDA	param->p[4]
#define DUMMY	param->p[5]
#define PSI_DEG	param->p[6]
#define DELTA	(param->p[7]*M_PI/180.)
#define Q	param->p[MAXPAR-1]
#define PSI	param->p[MAXPAR-2]

scalar tube_model_WEring(scalar x, sasfit_param *param) {
	scalar Zmu[3], lambda_mu[3], xi[3], dmu[3], Prod,ij;
	int i;
	ij=x*(1-x);
	lambda_mu[0]=LAMBDA;
	lambda_mu[1]=1./sqrt(LAMBDA);
	lambda_mu[2]=lambda_mu[1];
	Zmu[0]=Q*cos(PSI-DELTA)*RG;
	Zmu[1]=Q*sin(PSI-DELTA)*RG;
	Zmu[2]=0;
	Prod=0;
	for (i=0;i<3;i++) {
		dmu[i]=D0*pow(lambda_mu[i],NU);
		xi[i]=gsl_pow_2(dmu[i]/RG)/(2.*sqrt(6.));
		Prod=Prod
			   -gsl_pow_2(Zmu[i]*lambda_mu[i])*ij
		       +gsl_pow_2(Zmu[i]*lambda_mu[i])*xi[i]*(1-exp(-ij/(     xi[i])))
               -gsl_pow_2(Zmu[i]             )*xi[i]*(1-exp(-ij/(     xi[i]))) ;
	}
	return 2*(1-x)*exp(Prod);
}
scalar sasfit_ff_we__deformed_ringpolym__netw_(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((D0 < 0.0), param, "d0(%lg) < 0",D0); // modify condition to your needs
	SASFIT_CHECK_COND1((NU <= 0.0), param, "nu(%lg) <= 0",NU); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA < 0.0), param, "lambda(%lg) < 0",LAMBDA); // modify condition to your needs
	SASFIT_CHECK_COND1((DUMMY < 0.0), param, "dummy(%lg) < 0",DUMMY); // modify condition to your needs
	SASFIT_CHECK_COND1((PSI < 0.0), param, "psi(%lg) < 0",PSI); // modify condition to your needs
	SASFIT_CHECK_COND1((DELTA < 0.0), param, "delta(%lg) < 0",DELTA); // modify condition to your needs

	// insert your code here
	Q=q;
	PSI = sasfit_param_override_get_psi(PSI_DEG*M_PI/180.);
	return I0*sasfit_integrate(0.0,1.0,&tube_model_WEring,param);
}

scalar sasfit_ff_we__deformed_ringpolym__netw__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_we__deformed_ringpolym__netw__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

