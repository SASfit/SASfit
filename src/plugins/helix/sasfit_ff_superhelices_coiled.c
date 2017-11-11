/*
 * Author(s) of this file:
 *  Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *  10. Oct. 2017
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R1	param->p[0]
#define R2	param->p[1]
// #define DUMMY	param->p[2]
#define N	param->p[3]
// #define DUMMY	param->p[4]
#define P	param->p[5]
#define TURNS	param->p[6]
// #define DUMMY	param->p[7]
// #define DUMMY	param->p[8]
// #define DUMMY	param->p[9]
#define Q	param->p[MAXPAR-1]
#define L	param->p[MAXPAR-2]
#define A	param->p[MAXPAR-3]
#define GAM1 param->p[MAXPAR-4]
#define GAM2 param->p[MAXPAR-5]
#define ALPHA	param->p[MAXPAR-6]

void pitchangle(sasfit_param * param) {
scalar x;
	if (fabs(P) < 1e-6) {
		x = 2*M_PI*R1/P;
		ALPHA = M_PI/2. 
		       - x 
			   + gsl_pow_3(x)/3. 
		       - gsl_pow_5(x)/5.;
	} else {
		ALPHA = atan(x);
	}
}

scalar r2(scalar g1, scalar g2, void * pam) {
sasfit_param *param;
	param = (sasfit_param *) pam;
	return 2*(R2*R2+R1*R1+R2*R1*(cos(N*g2)+cos(N*g1)))
		 + gsl_pow_2(A*(g1-g2)) 
		 - 2*cos(g1-g2) * (R2*R2+R2*R1*(cos(N*g1)+cos(N*g2)) + R1*R1*(cos(N*g2)*cos(N*g1)+sin(N*g2)*sin(N*g1)*gsl_pow_2(cos(ALPHA)))) 
		 - 2*sin(g1-g2) * (R2*R1*cos(ALPHA)*(sin(N*g2)-sin(N*g1))-R1*R1*cos(ALPHA)*sin(g1-g2))
		 - 2*R1*R1*sin(N*g1)*sin(N*g2)*gsl_pow_2(sin(ALPHA))
		 + 2*A*R1*sin(ALPHA)*(sin(N*g2)-sin(N*g1))*(g1-g2)
		;
}

scalar sqrt_f(scalar gamma, void * pam) {
	scalar f;
	sasfit_param *param;
	param = (sasfit_param *) pam;
	f = R2*R2 + gsl_pow_2(R1*N) + R1*R1*(gsl_pow_2(cos(N*gamma))+gsl_pow_2(sin(N*gamma)*cos(ALPHA)))
		 + A*A + 2*R2*R1*cos(N*gamma)+2*R1*R1*N*cos(ALPHA);
	return sqrt(fabs(f));
}

scalar Kernel_coiled(const double *gam, void * pam) {
	sasfit_param *param;
	param = (sasfit_param *) pam;
	return sqrt_f(gam[0],pam)*sqrt_f(gam[1],pam)*sinc(Q*sqrt(fabs(r2(gam[0],gam[1],pam)))) / (L*L);
}


scalar Kernel_coiled_OOURA2(scalar x, void * pam) {
	sasfit_param *param;
	param = (sasfit_param *) pam;
	GAM2 = x;
	return sqrt_f(GAM1,pam)*sqrt_f(GAM2,pam)*sinc(Q*sqrt(fabs(r2(GAM1,GAM2,pam)))) / (L*L);
}

scalar Kernel_coiled_OOURA1(scalar x, void * pam) {
	sasfit_param *param;
	scalar sum, err, *aw;
	int lenaw;
	
	lenaw=10000;
	param = (sasfit_param *) pam;
	GAM1 = x;
	switch(sasfit_get_int_strategy()) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
			aw = (scalar *)malloc((lenaw)*sizeof(scalar));
			sasfit_intdeini(lenaw,GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
			sasfit_intde(&Kernel_coiled_OOURA2, 0.0, TURNS*2*M_PI, aw, &sum, &err,param);
			free(aw);
            break;
            } 
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
			aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
			sasfit_intccini(lenaw, aw);
			sasfit_intcc(&Kernel_coiled_OOURA2, 0.0, TURNS*2*M_PI, sasfit_eps_get_nriq(), lenaw, aw, &sum, &err,param);
			free(aw);
            break;
            }
	}
	return sum;
}

int f2D_cubature(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
	fval[0] = 0;
	if ((ndim < 2) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
    fval[0] = Kernel_coiled(x,pam);
    return 0;
} 

scalar sasfit_ff_superhelices_coiled(scalar q, sasfit_param * param)
{
	scalar cubxmin[2], cubxmax[2], fval[1], ferr[1];
	scalar sum, err, *aw;
	int lenaw,intstrategy;
	
	lenaw=10000;
    
	
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R1 < 0.0), param, "r1(%lg) < 0",R1); // modify condition to your needs
	SASFIT_CHECK_COND1((R2 < 0.0), param, "r2(%lg) < 0",R2); // modify condition to your needs
	SASFIT_CHECK_COND1((N < 0.0), param, "delta_1(%lg) < 0",N); // modify condition to your needs
	SASFIT_CHECK_COND1((TURNS < 0.0), param, "TURNS(%lg) < 0",TURNS); // modify condition to your needs
	SASFIT_CHECK_COND1((P < 0.0), param, "P(%lg) < 0",P); // modify condition to your needs

	// insert your code here
	Q=q;
	A = P/(2.*M_PI);
	pitchangle(param);
	
	aw = (scalar *)malloc((lenaw)*sizeof(scalar));
	sasfit_intdeini(lenaw,GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
	sasfit_intde(&sqrt_f, 0.0, TURNS*2*M_PI, aw, &sum, &err,param);
    free(aw);
	L = sum;
	
    intstrategy = sasfit_get_int_strategy();
	intstrategy=P_CUBATURE;
	switch(intstrategy) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
			aw = (scalar *)malloc((lenaw)*sizeof(scalar));
			sasfit_intdeini(lenaw,GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
			sasfit_intde(&Kernel_coiled_OOURA1, 0.0, TURNS*2*M_PI, aw, &sum, &err,param);
			free(aw);
            break;
            } 
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
			aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
			sasfit_intccini(lenaw, aw);
			sasfit_intcc(&Kernel_coiled_OOURA1, 0.0, TURNS*2*M_PI, sasfit_eps_get_nriq(), lenaw, aw, &sum, &err,param);
			free(aw);
            break;
            }
    case H_CUBATURE: {
			cubxmin[0]=0;
			cubxmax[0]=2*M_PI*TURNS;
			cubxmin[1]=0;
			cubxmax[1]=2*M_PI*TURNS;
			hcubature(1, &f2D_cubature,param,2, cubxmin, cubxmax, 
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED, 
				fval, ferr);
//	ERROR_PAIRED
//	ERROR_INDIVIDUAL
//  ERROR_L2
//  ERROR_L1
//  ERROR_LINF
			sum = fval[0];
            break;
            }
    case P_CUBATURE: {	
			cubxmin[0]=0;
			cubxmax[0]=2*M_PI*TURNS;
			cubxmin[1]=0;
			cubxmax[1]=2*M_PI*TURNS;
			pcubature(1, &f2D_cubature,param,2, cubxmin, cubxmax, 
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED, 
				fval, ferr);
//	ERROR_PAIRED
//	ERROR_INDIVIDUAL
//  ERROR_L2
//  ERROR_L1
//  ERROR_LINF
			sum = fval[0];
            break;
            }
    default: {
            sasfit_err("Unknown integration strategy\n");
            break;
            }
    }
    return sum*L*L;
}

scalar sasfit_ff_superhelices_coiled_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_superhelices_coiled_v(scalar q, sasfit_param * param, int dist)
{
	scalar sum, err, *aw;
	int lenaw;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	A = P/(2.*M_PI);
	pitchangle(param);
	
	sasfit_intdeini(lenaw,GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
	sasfit_intde(&sqrt_f, 0.0, TURNS, aw, &sum, &err,param);
    free(aw);
	L = sum;
	return L;
}

