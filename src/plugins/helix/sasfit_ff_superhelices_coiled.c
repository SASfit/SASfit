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
    x = R2/P/(2*M_PI);
    if (gsl_posinf()==x) ALPHA =  M_PI_2;
    if (gsl_neginf()==x) ALPHA = -M_PI_2;
	if (fabs(P) < 1e-6) {
		ALPHA = M_PI_2
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
		 - 2*cos(g1-g2) * (R2*R2+ R2*R1*(cos(N*g1)+cos(N*g2))
                                + R1*R1*(cos(N*g2)*cos(N*g1)
                                        + sin(N*g2)*sin(N*g1)*gsl_pow_2(cos(ALPHA))))
		 - 2*sin(g1-g2) * ( R2*R1*cos(ALPHA)*(sin(N*g2)-sin(N*g1))
                           -R1*R1*cos(ALPHA)*sin(g1-g2))
		 - 2*R1*R1*sin(N*g1)*sin(N*g2)*gsl_pow_2(sin(ALPHA))
		 + 2*A*R1*sin(ALPHA)*(sin(N*g2)-sin(N*g1))*(g1-g2);
}

scalar sqrt_f(scalar gamma, sasfit_param *param) {
	scalar f;
	f =   R2*R2
        + gsl_pow_2(R1*N)
        + R1*R1*(gsl_pow_2(cos(N*gamma))+gsl_pow_2(sin(N*gamma)*cos(ALPHA)))
		+ A*A
		+ 2*R2*R1*cos(N*gamma)
		+ 2*R1*R1*N*cos(ALPHA);
	return sqrt(fabs(f));
}

scalar f2D_cubature(const double *x, size_t ndim, void *pam) {
	sasfit_param *param;
	param = (sasfit_param *) pam;
	if (ndim < 2) {
		sasfit_out("false dimensions ndim:%d\n",ndim);
		return 1;
	}
    return sqrt_f(x[0],param)*sqrt_f(x[1],param)*sinc(Q*sqrt(fabs(r2(x[0],x[1],param))));
}

scalar sasfit_ff_superhelices_coiled(scalar q, sasfit_param * param)
{
	scalar cubxmin[2], cubxmax[2], res, err;
	int ierr;

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

    L=sasfit_integrate(0.0, TURNS*2*M_PI,&sqrt_f,param);
    sasfit_out("L:%.5lg\n",L);

    cubxmin[0]=0;
    cubxmax[0]=2*M_PI*TURNS;
    cubxmin[1]=0;
    cubxmax[1]=2*M_PI*TURNS;
    ierr = sasfit_cubature(2,cubxmin,cubxmax,&f2D_cubature,param,sasfit_eps_get_nriq(), &res, &err);

    sasfit_out("res:%.8lg, err:%.12lg, ierr:%d\n",res,err,ierr);

    return res;
}

scalar sasfit_ff_superhelices_coiled_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_superhelices_coiled_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	A = P/(2.*M_PI);
	pitchangle(param);

	return  sasfit_integrate(0.0, TURNS*2*M_PI,&sqrt_f,param);;
}

