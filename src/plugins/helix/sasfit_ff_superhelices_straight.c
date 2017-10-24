/*
 * Author(s) of this file:
 *  Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *  10. Oct. 2017
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R1	param->p[0]
#define DELTA1	param->p[1]
#define R2	param->p[2]
#define DELTA2	param->p[3]
#define ALPHA	param->p[4]*M_PI/180.
#define P	param->p[5]
#define H	param->p[6]
#define ETA_1	param->p[7]
#define ETA_2	param->p[8]
#define ETA_SOLV	param->p[9]
#define Q	param->p[MAXPAR-1]
#define L	param->p[MAXPAR-2]
#define A	param->p[MAXPAR-3]

scalar Kernel_w(scalar w, void * pam) {
	scalar Psi;
	sasfit_param *param;
	param = (sasfit_param *) pam;

	Psi = sqrt(2.*R1*R1*(1.-cos(w/sqrt(R1*R1+A*A)))+A*A*w*w/(R1*R1+A*A)); 
//	sasfit_out("a:%lg\t P:%lg\t R:%lg\t H:%lg\t L:%lg\t q:%lg\t\n",A,P,R1,H,L,Q);
	return 2./(L*L) * (L-w)*sinc(Q*Psi);
}

scalar sasfit_ff_superhelices(scalar q, sasfit_param * param)
{
	scalar sum,err, *aw;
	int lenaw;
	
	lenaw=4000;
    aw = (scalar *)malloc((lenaw)*sizeof(scalar));
	
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R1 < 0.0), param, "r1(%lg) < 0",R1); // modify condition to your needs
	SASFIT_CHECK_COND1((R2 < 0.0), param, "r2(%lg) < 0",R2); // modify condition to your needs
	SASFIT_CHECK_COND1((DELTA1 < 0.0), param, "delta_1(%lg) < 0",DELTA1); // modify condition to your needs
	SASFIT_CHECK_COND1((DELTA2 < 0.0), param, "delta_2(%lg) < 0",DELTA2); // modify condition to your needs
	SASFIT_CHECK_COND1((H < 0.0), param, "H(%lg) < 0",H); // modify condition to your needs
	SASFIT_CHECK_COND1((P < 0.0), param, "P(%lg) < 0",P); // modify condition to your needs

	// insert your code here
	Q=q;
	A = P/(2.*M_PI);
	L=H/A*sqrt(R1*R1+A*A);
//	sum = Kernel_w(0,param);
	
	sasfit_intdeini(lenaw,GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
	sasfit_intde(&Kernel_w, 0.0, L, aw, &sum, &err,param);
//	sasfit_out("a:%lg\t P:%lg\t R:%lg\t H:%lg\t L:%lg\t q:%lg\t I:%lg\t Delta_I:%lg\t\n",A,P,R1,H,L,q,sum,err);
//	sum = sasfit_integrate(0,L,&Kernel_w,param);
	

    free(aw);
	return sum;
}

scalar sasfit_ff_superhelices_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_superhelices_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

